#include <zmqlooplib/ZmqExecutor.h>

ZmqExecutor::CommandExecutor::CommandExecutor(ZmqExecutor &executor) : executor(executor) {}

void ZmqExecutor::CommandExecutor::start() {
    std::thread([&]() {
        zmqpp::message notificationThrowaway; // we cant use address of temp
        zmqpp::socket notificationReceiver(executor.context, zmqpp::socket_type::pair);
        notificationReceiver.bind(executor.notificationAddress);//todo what happens with exception here?
        loop.add(notificationReceiver, [&]() -> bool {
            try {
                notificationReceiver.receive(notificationThrowaway);
                executor.taskQueue.execute();
                return true;
            }
            catch (zmqpp::zmq_internal_exception &e) {
                if (e.zmq_error() != ETERM) {
                    std::cerr << "unexpected exception " << e.zmq_error() << " " << e.what() << std::endl;
                    executor.close();
                }
                return false;
            }
            catch (std::exception &e) {
                std::cerr << "unexpected exception from task " << e.what() << std::endl;
                return false;
            }
        });
        executor.commandExecutorNotifier.notify();
        try {
            loop.start();
        } catch (zmqpp::zmq_internal_exception &e) {
            if (e.zmq_error() != ETERM) {
                std::cerr << "unexpected exception " << e.zmq_error() << " " << e.what() << std::endl;
                executor.close();
            }
        }
        notificationReceiver.close();
        std::cout << "loop stopped" << std::endl;
    }).detach();
}

std::future<Socket *> ZmqExecutor::CommandExecutor::createReactiveSocket
        (zmqpp::socket_type socketType, std::shared_ptr<Inbox> inbox) {
    auto promise = std::make_shared<std::promise<Socket *>>();
    auto future = promise->get_future();
    auto task = new std::function<void()>([&, promise]() mutable {
        auto zmqSocket = std::make_shared<zmqpp::socket>(executor.context, socketType);
        loop.add(*zmqSocket, [zmqSocket, inbox]() {
            auto message = std::make_unique<zmqpp::message>();
            zmqSocket->receive(*message);
            inbox->receive(std::move(message));
            return true;
        });
        auto closer = std::make_shared<std::function<void()>>([this, zmqSocket]() {
            loop.remove(*zmqSocket);
        });
        auto socket = new Socket(zmqSocket, closer, executor);
        promise->set_value(socket);
    });
    executor.inject(task);
    return future;
}


ZmqExecutor::NotificationInjector::NotificationInjector(ZmqExecutor &executor) : executor(executor) {}

void ZmqExecutor::NotificationInjector::start() {
    std::thread([&]() {
                    zmqpp::socket notificationSender(executor.context, zmqpp::socket_type::pair);
                    notificationSender.connect(executor.notificationAddress); //todo what happens with exception here?
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
                                executor.close();
                            }
                            break;
                        }
                    }
                    notificationSender.close();
                    std::cout << "notification thread stopped" << std::endl;
                }
    ).detach();
}

void ZmqExecutor::start() {
    commandExecutor.start();
    commandExecutorNotifier.wait();
    notificationInjector.start();
    notificationInjectorNotifier.wait();
}

void ZmqExecutor::inject(std::function<void()> *task) {
    taskQueue.add(task);
    taskNotifier.notify();
}

ZmqExecutor *ZmqExecutor::create() {
    auto zmqExecutor = new ZmqExecutor();
    zmqExecutor->start();
    return zmqExecutor;
}

void ZmqExecutor::injectionTest(std::function<void()> *task) { //todo rename / remove / evaluate "friend class"
    inject(task);
}

std::future<Socket *> ZmqExecutor::createPublisher(std::shared_ptr<Inbox> inbox) {
    return commandExecutor.createReactiveSocket(zmqpp::socket_type::xpub, inbox);
}

std::future<Socket *> ZmqExecutor::createSubscriber(std::shared_ptr<Inbox> inbox) {
    return commandExecutor.createReactiveSocket(zmqpp::socket_type::xsub, inbox);
}

void ZmqExecutor::close() {
    bool expected = false;
    if (stopRequested.compare_exchange_strong(expected, true)) {
        std::thread([&]() {
            taskNotifier.close();
            commandExecutorNotifier.close();
            notificationInjectorNotifier.close();
            context.terminate();
        }).detach();
    }
}
