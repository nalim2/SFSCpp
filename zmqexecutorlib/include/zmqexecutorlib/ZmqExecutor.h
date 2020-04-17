#ifndef ZMQEXECUTORLIB_ZMQEXECUTOR_H
#define ZMQEXECUTORLIB_ZMQEXECUTOR_H

#include "Socket.h"
#include "Inbox.h"
#include "../../src/taskQueue.h"
#include "../../src/notifier.h"
#include "Callback.h"

#include <zmqpp/zmqpp.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <iostream>
#include <future>
#include <set>

class ZmqExecutor {

private:
    class CommandExecutor {
        zmqpp::loop loop;
        std::set<zmqpp::socket *> sockets;
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
    Notifier closeNotifier;
    CommandExecutor commandExecutor = CommandExecutor(*this);
    NotificationInjector notificationInjector = NotificationInjector(*this);
    std::shared_ptr<Callback> shutdownCallback;

    ZmqExecutor(std::shared_ptr<Callback> shutdownCallback);

    void start();

    friend class Socket;
    void inject(std::function<void()> *task);

    void initClose();

    void awaitClosed();

public:
    static ZmqExecutor *create(std::shared_ptr<Callback> shutdownCallback);

    std::future<Socket *> createPublisher(std::shared_ptr<Inbox> inbox);

    std::future<Socket *> createSubscriber(std::shared_ptr<Inbox> inbox);

    ~ZmqExecutor();
};

#endif //ZMQEXECUTORLIB_ZMQEXECUTOR_H