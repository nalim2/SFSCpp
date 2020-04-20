#include <zmqexecutorlib/ZmqExecutor.h>

#include <utility>
#include <zmqexecutorlib/Callback.h>

ZmqExecutor::CommandExecutor::CommandExecutor(ZmqExecutor &executor) : executor(executor) {}

void ZmqExecutor::CommandExecutor::start() {
    std::thread([&]() {
        zmqpp::message notificationThrowaway; //temp
        zmqpp::socket notificationReceiver(executor.context, zmqpp::socket_type::pair);
        notificationReceiver.bind(executor.notificationAddress);
        loop.add(notificationReceiver, [&]() -> bool {
            /*
             * while (true) {
             * receive(notification); //blocks
             * executeTask();
             * }
             */
            try {
                notificationReceiver.receive(notificationThrowaway);
            } catch (zmqpp::zmq_internal_exception &e) {
                if (e.zmq_error() != ETERM) {
                    std::cerr << "unexpected exception " << e.zmq_error() << " " << e.what() << std::endl;
                }
                return false; //ends loop
            }
            try {
                executor.taskQueue.execute();
            } catch (std::exception &e) {
                std::cerr << "unexpected exception from task " << e.what() << std::endl;
                return false; //ends loop
            }
            return true; //continues
        });
        //notify that we have successfully bound, so notificationInjector can connect (inproc bind/connect order!)
        executor.commandExecutorNotifier.notify();
        try {
            loop.start(); //starts loop, so thread is blocked while running
        } catch (zmqpp::zmq_internal_exception &e) {
            if (e.zmq_error() != ETERM) {
                std::cerr << "unexpected exception " << e.zmq_error() << " " << e.what() << std::endl;
            }
        }

        // This point is only reached after loop terminates, so in case of failure inside loop or on close call.
        // For first case, we need to init a complete close
        executor.initClose();

        //closes stuff
        loop.remove(notificationReceiver);
        notificationReceiver.close();
        //closes all remaining sockets, so the context.close() call terminates
        // Note that we are still on one thread, so its okay to do this
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
    //note that this is run inside the loop thread
    auto task = new std::function<void()>([=, this]() mutable { //mutable because of promise->set_value()
        //shared pointer just for easy handling, still not thread safe!
        auto zmqSocket = std::make_shared<zmqpp::socket>(executor.context, socketType);
        loop.add(*zmqSocket, [=]() { //receive message, then call inbox method
            auto message = std::make_unique<zmqpp::message>();
            zmqSocket->receive(*message);
            inbox->receive(std::move(message));
            return true;
        });
        auto closer = std::make_shared<std::function<void()>>([=, this]() { //cleanup function
            loop.remove(*zmqSocket);
            sockets.erase(&*zmqSocket);
        });
        auto socket = new Socket(zmqSocket, std::move(closer), executor);
        sockets.insert(&*zmqSocket);
        promise->set_value(socket);
    });
    executor.execute(task); // execute in the correct thread
    return future;
}


ZmqExecutor::NotificationInjector::NotificationInjector(ZmqExecutor &executor) : executor(executor) {}

void ZmqExecutor::NotificationInjector::start() {
    std::thread([&]() {
        zmqpp::socket notificationSender(executor.context, zmqpp::socket_type::pair);
        //needs to be called after command executor socket bind
        notificationSender.connect(executor.notificationAddress);
        executor.notificationInjectorNotifier.notify();
        while (true) {
            try {
                zmqpp::message notification(true); //message cant be empty, so just put true inside
                executor.taskNotifier.wait(); // wait until there is a task to do, so we receive a notification
                notificationSender.send(notification); //then notify the commandExecutor
            } catch (NotifierClosedException &e) {
                break; //notifier closed -> shutdown
            } catch (zmqpp::zmq_internal_exception &e) {
                if (e.zmq_error() != ETERM) {
                    std::cerr << "unexpected exception " << e.zmq_error() << e.what() << std::endl;
                    executor.initClose();
                }
                break; //shutdown
            }
        }
        notificationSender.close();
        std::cout << "notification thread stopped" << std::endl;
    }).detach();
}

void ZmqExecutor::start() { //we need to bind socket of commandExecutor, then connect afterwards
    commandExecutor.start();
    commandExecutorNotifier.wait();
    notificationInjector.start();
    notificationInjectorNotifier.wait();
}

void ZmqExecutor::execute(std::function<void()> *task) {
    if (!stopRequested) { //do nothing if stopped
        taskQueue.add(task); //add to queue
        taskNotifier.notify(); //then tell notificationInjector to tell commandExecutor about new task
    }
}

ZmqExecutor::ZmqExecutor(std::shared_ptr<Callback> shutdownCallback) :
        shutdownCallback(std::move(shutdownCallback)) {}

ZmqExecutor *ZmqExecutor::create(std::shared_ptr<Callback> shutdownCallback) {
    auto zmqExecutor = new ZmqExecutor(std::move(shutdownCallback));
    zmqExecutor->start();
    return zmqExecutor;
}

std::future<Socket *> ZmqExecutor::createPublisher(std::shared_ptr<Inbox> inbox) {
    return commandExecutor.createReactiveSocket(zmqpp::socket_type::xpub, std::move(inbox));
}

std::future<Socket *> ZmqExecutor::createSubscriber(std::shared_ptr<Inbox> inbox) {
    return commandExecutor.createReactiveSocket(zmqpp::socket_type::xsub, std::move(inbox));
}

void ZmqExecutor::initClose() {
    bool expected = false;
    if (stopRequested.compare_exchange_strong(expected, true)) { //make idempotent, so only execute once
        std::thread([&, shutdownCallback = std::move(shutdownCallback)]() {
            taskNotifier.close();
            commandExecutorNotifier.close();
            notificationInjectorNotifier.close();
            context.terminate(); //blocks until all sockets are closed
            closeNotifier.close(); //sets awaitClosed() to notBlocking
            shutdownCallback->callback(); //perform callback after shutdown
        }).detach();
    }
}

void ZmqExecutor::awaitClosed() {
    try {
        closeNotifier.wait(); //blocks until exception is thrown.
    } catch (NotifierClosedException &expected) {
        return; //expected, so return
    }
}

ZmqExecutor::~ZmqExecutor() {
    initClose();
    awaitClosed(); //blocks until completed
}