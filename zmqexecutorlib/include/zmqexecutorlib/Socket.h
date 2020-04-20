#ifndef ZMQEXECUTORLIB_SOCKET_H
#define ZMQEXECUTORLIB_SOCKET_H

#include <string>
#include <zmqpp/socket.hpp>
#include <memory>
#include <functional>

class ZmqExecutor; //forward declaration

/**
 * Socket class to interact with. Safe for multi threaded access.
 * Every method is executed async via ZmqExecutor.
 */
class Socket {
private:
    std::shared_ptr<zmqpp::socket> zmqSocket;
    std::shared_ptr<std::function<void()>> closer; //cleanup function to be called on shutdown
    ZmqExecutor &executor;

public:
    Socket(std::shared_ptr<zmqpp::socket> zmqSocket,
           std::shared_ptr<std::function<void()>> closer,
           ZmqExecutor &executor);

    ~Socket();

    /**
     * Connects the socket to the specified endpoint.
     * @param endpoint endpoint to connect to
     */
    void connect(std::string endpoint);

    /**
     * Disconnects the socket from the specified endpoint.
     * @param endpoint endpoint to disconnect from
     */
    void disconnect(std::string endpoint);

    /**
     * Binds the socket to the specified endpoint.
     * @param endpoint endpoint to bind to
     */
    void bind(std::string endpoint);

    /**
     * Unbinds the socket from the specified endpoint.
     * @param endpoint endpoint to unbind from
     */
    void unbind(std::string endpoint);

    /**
     * If publisher socket, sets the xpub_verbose option. If another socket type, nothing happens.
     */
    void setXPubVerbose();

    /**
     * The handed function will be executed exactly once to generate a message, then sends this message.
     * We dont pass the message directly since zmq objects arent thread safe. Instead we create it in the correct thread
     * when we need it.
     * @param messageSupplier function called once to generate the message to be sent.
     */
    void send(std::shared_ptr<std::function<zmqpp::message()>> messageSupplier);

};

#endif //ZMQEXECUTORLIB_SOCKET_H
