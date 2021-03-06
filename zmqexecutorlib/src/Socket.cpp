#include <zmqexecutorlib/Socket.h>
#include <zmqexecutorlib/ZmqExecutor.h>

#include <utility>

Socket::Socket(std::shared_ptr<zmqpp::socket> zmqSocket,
               std::shared_ptr<std::function<void()>> closer,
               ZmqExecutor &executor)
        : zmqSocket(std::move(zmqSocket)), closer(std::move(closer)), executor(executor) {}

Socket::~Socket() {
    //wont get executed in case of executor is already shutdown, but doesnt matter since then not needed anymore
    executor.execute(std::make_unique<std::function<void()>>(
            [closer = closer] { closer->operator()(); }));
}

void Socket::connect(std::string endpoint) {
    executor.execute(std::make_unique<std::function<void()>>(
            [zmqSocket = zmqSocket, endpoint = std::move(endpoint)] { zmqSocket->connect(endpoint); }));
}

void Socket::disconnect(std::string endpoint) {
    executor.execute(std::make_unique<std::function<void()>>(
            [zmqSocket = zmqSocket, endpoint = std::move(endpoint)] { zmqSocket->disconnect(endpoint); }));
}

void Socket::bind(std::string endpoint) {
    executor.execute(std::make_unique<std::function<void()>>(
            [zmqSocket = zmqSocket, endpoint = std::move(endpoint)] { zmqSocket->bind(endpoint); }));
}

void Socket::unbind(std::string endpoint) {
    executor.execute(std::make_unique<std::function<void()>>(
            [zmqSocket = zmqSocket, endpoint = std::move(endpoint)] { zmqSocket->unbind(endpoint); }));
}

void Socket::setXPubVerbose() {
    executor.execute(std::make_unique<std::function<void()>>(
            [zmqSocket = zmqSocket] {
                if (zmqSocket->type() == zmqpp::socket_type::xpub) {
                    zmqSocket->set(zmqpp::socket_option::xpub_verbose, true);
                }
            }));
}

void Socket::send(std::shared_ptr<std::function<zmqpp::message()>> messageSupplier) {
    executor.execute(std::make_unique<std::function<void()>>(
            [zmqSocket = zmqSocket, messageSupplier = std::move(messageSupplier)] {
                zmqpp::message message = messageSupplier->operator()();
                zmqSocket->send(message);
            }
    ));
}





