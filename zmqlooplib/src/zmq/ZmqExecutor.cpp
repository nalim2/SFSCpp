#include <zmqlooplib/ZmqExecutor.h>

#include <utility>
#include <zmqlooplib/Callback.h>

ZmqExecutor::CommandExecutor::CommandExecutor(ZmqExecutor &executor) : executor(executor) {}

void ZmqExecutor::CommandExecutor::start() {
    std::thread([&]() {
        zmqpp::message notificationThrowaway; // we cant use address of temp
        zmqpp::socket notificationReceiver(executor.context, zmqpp::socket_type::pair);
        notificationReceiver.bind(executor.notificationAddress);
        loop.add(notificationReceiver, [&]() -> bool {
            try {
                notificationReceiver.receive(notificationThrowaway);
            } catch (zmqpp::zmq_internal_exception &e) {
                if (e.zmq_error() != ETERM) {
                    std::cerr << "unexpected exception " << e.zmq_error() << " " << e.what() << std::endl;
                }
                return false;
            }
            try {
                executor.taskQueue.execute();
            } catch (std::exception &e) {
                std::cerr << "unexpected exception from task " << e.what() << std::endl;
                return false;
            }
            return true;
        });
        executor.commandExecutorNotifier.notify();
        try {
            loop.start();
        } catch (zmqpp::zmq_internal_exception &e) {
            if (e.zmq_error() != ETERM) {
                std::cerr << "unexpected exception " << e.zmq_error() << " " << e.what() << std::endl;
            }
        }
        executor.initClose();
        loop.remove(notificationReceiver);
        notificationReceiver.close();
        for (auto socket : sockets){
            socket->close();
        }
        std::cout << "loop stopped" << std::endl;
    }).detach();
}

std::future<Socket *> ZmqExecutor::CommandExecutor::createReactiveSocket
        (zmqpp::socket_type socketType, std::shared_ptr<Inbox> inbox) {
    auto promise = std::make_shared<std::promise<Socket *>>();
    auto future = promise->get_future();
    auto task = new std::function<void()>([=, this]() mutable {
        auto zmqSocket = std::make_shared<zmqpp::socket>(executor.context, socketType);
        loop.add(*zmqSocket, [=]() {
            auto message = std::make_unique<zmqpp::message>();
            zmqSocket->receive(*message);
            inbox->receive(std::move(message));
            return true;
        });
        auto closer = std::make_shared<std::function<void()>>([=, this]() {
            loop.remove(*zmqSocket);
            sockets.erase(&*zmqSocket);
        });
        auto socket = new Socket(zmqSocket, std::move(closer), executor);
        sockets.insert(&*zmqSocket);
        promise->set_value(socket);
    });
    executor.inject(task);
    return future;
}


ZmqExecutor::NotificationInjector::NotificationInjector(ZmqExecutor &executor) : executor(executor) {}

void ZmqExecutor::NotificationInjector::start() {
    std::thread([&]() {
        zmqpp::socket notificationSender(executor.context, zmqpp::socket_type::pair);
        notificationSender.connect(executor.notificationAddress);
        executor.notificationInjectorNotifier.notify();
        while (true) {
            try {
                zmqpp::message notification(true);
                executor.taskNotifier.wait();
                notificationSender.send(notification);
            } catch (NotifierClosedException &e) {
                break;
            } catch (zmqpp::zmq_internal_exception &e) {
                if (e.zmq_error() != ETERM) {
                    std::cerr << "unexpected exception " << e.zmq_error() << e.what() << std::endl;
                    executor.initClose();
                }
                break;
            }
        }
        notificationSender.close();
        std::cout << "notification thread stopped" << std::endl;
    }).detach();
}

void ZmqExecutor::start() {
    commandExecutor.start();
    commandExecutorNotifier.wait();
    notificationInjector.start();
    notificationInjectorNotifier.wait();
}

void ZmqExecutor::inject(std::function<void()> *task) {
    if (!stopRequested) {
        taskQueue.add(task);
        taskNotifier.notify();
    }
}

ZmqExecutor::ZmqExecutor(std::shared_ptr<Callback> shutdownCallback) :
        shutdownCallback(std::move(shutdownCallback)) {}

ZmqExecutor *ZmqExecutor::create(std::shared_ptr<Callback> shutdownCallback) {
    auto zmqExecutor = new ZmqExecutor(std::move(shutdownCallback));
    zmqExecutor->start();
    return zmqExecutor;
}

void ZmqExecutor::injectionTest(std::function<void()> *task) { //todo rename / remove / evaluate "friend class"
    inject(task);
}

std::future<Socket *> ZmqExecutor::createPublisher(std::shared_ptr<Inbox> inbox) {
    return commandExecutor.createReactiveSocket(zmqpp::socket_type::xpub, std::move(inbox));
}

std::future<Socket *> ZmqExecutor::createSubscriber(std::shared_ptr<Inbox> inbox) {
    return commandExecutor.createReactiveSocket(zmqpp::socket_type::xsub, std::move(inbox));
}

void ZmqExecutor::initClose() {
    static bool expected = false;
    if (stopRequested.compare_exchange_strong(expected, true)) {
        std::thread([&, shutdownCallback = std::move(shutdownCallback)]() {
            taskNotifier.close();
            commandExecutorNotifier.close();
            notificationInjectorNotifier.close();
            context.terminate();
            closeNotifier.close();
            shutdownCallback->callback();
        }).detach();
    }
}

void ZmqExecutor::awaitClosed() {
    try {
        closeNotifier.wait();
    } catch (NotifierClosedException &e) {
        return;
    }
}

ZmqExecutor::~ZmqExecutor() {
    initClose();
    awaitClosed();
}