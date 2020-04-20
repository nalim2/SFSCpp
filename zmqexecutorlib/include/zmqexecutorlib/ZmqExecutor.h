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

/**
 * Socket Factory, also acts as executor for the created Sockets. If closed, the created sockets wont work any longer.
 * Since basic zmq sockets are not threadsafe, we achieve thread safety by executor model implemented in this class.
 * Works as follows: We have a CommandExecutor and an NotificationInjector.
 * CommandExecutor: There is a central thread polling on every socket and calling the corresponding inbox
 * receive(message) method every time a message is received. With Command Executor alone, we can only receive messages,
 * since there is no way to let the thread do the other stuff we want. Only way to communicate with it is a zmq socket.
 *
 * NotificationInjector: To be able to also send messages/bind/connect/... there is the NotificationInjector. It
 * consists of a pair of sockets and one thread. One socket is added to the CommandExecutor, and one exists inside
 * the thread. The sockets are connected. The thread blocks until a command is added, then the threads socket sends a
 * notification to its partner socket inside the CommandExecutor. Every time this socket receives a message, it knows
 * that there is a new command to execute, and the corresponding receipt method executing the command will be called
 * from the CommandExecutor thread.
 */
class ZmqExecutor {

private:
    /**
     * This class contains the actually executing thread.
     */
    class CommandExecutor {
        zmqpp::loop loop;
        std::set<zmqpp::socket *> sockets; //needed to close them automatically on shutdown so context termination doenst block
        ZmqExecutor &executor;

    public:
        CommandExecutor(ZmqExecutor &executor);

        /**
         * Starts the executing thread. To be called only once on creation.
         */
        void start();

        /**
         * Creates a new Socket async.
         * @param socketType type of socket to create
         * @param inbox inbox object, whose receive(message) method will be called every time a message is received
         * @return the future containing the pointer to the generated socket.
         */
        std::future<Socket *> createReactiveSocket(zmqpp::socket_type socketType, std::shared_ptr<Inbox> inbox);
    };

private:
    /**
     * This class contains the thread needed to notify the CommandExecutor that there is a new task.
     */
    class NotificationInjector {
        ZmqExecutor &executor;

    public:
        NotificationInjector(ZmqExecutor &executor);

        /**
         * Starts the injecting thread. To be called only once on creation.
         */
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
    Notifier closeNotifier; // Dont ever call notify() on this object, only close()
    CommandExecutor commandExecutor = CommandExecutor(*this);
    NotificationInjector notificationInjector = NotificationInjector(*this);
    std::shared_ptr<Callback> shutdownCallback;

    ZmqExecutor(std::shared_ptr<Callback> shutdownCallback);

    /**
     * Initializes CommandExecutor and NotificationInjector. To be called only once on creation.
     */
    void start();

    friend class Socket; //Socket class needs to access the following execute() method
    /**
     * Queues the task for execution.
     * @param task task to execute async
     */
    void execute(std::function<void()> *task);

    /**
     * Initializes a close. Idempotent, so can be called multiple times, but will be executed only once.
     * Closes CommandExecutor and NotificationInjector, then calls the Shutdown Callback.
     */
    void initClose();

    /**
     * Blocks until closed. Already returns when Shutdown Callback is still being executed.
     */
    void awaitClosed();

public:
    /**
     * Creates new ZmqExecutor.
     * @param shutdownCallback Object whose callback() method is called in case of a shutdown, either in case of an
     * exception do to destructor call of ZmqExecutor.
     * @return the created ZmqExecutor
     */
    static ZmqExecutor *create(std::shared_ptr<Callback> shutdownCallback);

    /**
     * Creates Publisher Socket.
     * @param inbox inbox object, whose receive(message) method will be called every time a message is received
     * @return the future containing the pointer to the generated socket.
     */
    std::future<Socket *> createPublisher(std::shared_ptr<Inbox> inbox);

    /**
     * Creates Subscriber Socket.
     * @param inbox inbox object, whose receive(message) method will be called every time a message is received
     * @return the future containing the pointer to the generated socket.
     */
    std::future<Socket *> createSubscriber(std::shared_ptr<Inbox> inbox);

    ~ZmqExecutor();
};

#endif //ZMQEXECUTORLIB_ZMQEXECUTOR_H