#ifndef ZMQLOOPLIB_SOCKET_H
#define ZMQLOOPLIB_SOCKET_H

#include <string>
#include <zmqpp/socket.hpp>
#include <memory>
#include <functional>

class ZmqExecutor; //todo needed / other ways?

class Socket {
private:
    std::shared_ptr<zmqpp::socket> zmqSocket;
    std::shared_ptr<std::function<void()>> closer;
    ZmqExecutor &executor;

public:

    Socket(std::shared_ptr<zmqpp::socket> zmqSocket,
           std::shared_ptr<std::function<void()>> closer,
           ZmqExecutor &executor);

    ~Socket();

    void connect(std::string endpoint);;

    void disconnect(std::string endpoint);

    void bind(std::string endpoint);

    void unbind(std::string endpoint);

    void setXPubVerbose();

    void send(std::shared_ptr<std::function<zmqpp::message()>> messageSupplier);

};


#endif //ZMQLOOPLIB_SOCKET_H
