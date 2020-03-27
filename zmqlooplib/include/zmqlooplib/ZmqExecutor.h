#ifndef ZMQLOOPLIB_ZMQEXECUTOR_H
#define ZMQLOOPLIB_ZMQEXECUTOR_H

#include "Socket.h"
#include "Inbox.h"
#include "../../src/util/taskQueue.h" //todo .. ?
#include "../../src/util/notifier.h"  //todo .. ?

#include <zmqpp/zmqpp.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <iostream>
#include <future>

class ZmqExecutor {

private:
    class CommandExecutor {
        zmqpp::loop loop;
        ZmqExecutor &executor;

    public:
        CommandExecutor(ZmqExecutor &executor);

        void start();

        std::future<Socket *> createReactiveSocket(zmqpp::socket_type socketType, std::shared_ptr<Inbox> inbox);
    };

private:
    class NotificationInjector {
        ZmqExecutor &executor;

    public:
        NotificationInjector(ZmqExecutor &executor);

        void start();
    };


private:
    const std::string notificationAddress = "inproc://" + boost::uuids::to_string(boost::uuids::random_generator()());
    std::atomic_bool stopRequested = false;
    zmqpp::context context;
    TaskQueue taskQueue;
    Notifier taskNotifier;
    Notifier commandExecutorNotifier;
    Notifier notificationInjectorNotifier;
    CommandExecutor commandExecutor = CommandExecutor(*this);
    NotificationInjector notificationInjector = NotificationInjector(*this);

    void start();

    void inject(std::function<void()> *task);

    ZmqExecutor() = default;

    void close();

public:
    static ZmqExecutor *create();

    void injectionTest(std::function<void()> *task);

    std::future<Socket *> createPublisher(std::shared_ptr<Inbox> inbox);

    std::future<Socket *> createSubscriber(std::shared_ptr<Inbox> inbox);

//    void addShutdownListener(); todo instead call java executeAll sth

    ~ZmqExecutor() {
        close();
    }
};

#endif //ZMQLOOPLIB_ZMQEXECUTOR_H