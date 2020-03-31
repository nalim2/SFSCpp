#include <zmqlooplib/Socket.h>
#include <zmqlooplib/ZmqExecutor.h>

Socket::Socket(std::shared_ptr<zmqpp::socket> zmqSocket,
               std::shared_ptr<std::function<void()>> closer,
               ZmqExecutor &executor)
        : zmqSocket(zmqSocket), closer(closer), executor(executor) {}

Socket::~Socket() {
    executor.injectionTest(new std::function<void()>([&] { closer->operator()(); }));
}

void Socket::connect(std::string endpoint) {
    executor.injectionTest(new std::function<void()>([&] { zmqSocket->connect(endpoint); }));
}

void Socket::disconnect(std::string endpoint) {
    executor.injectionTest(new std::function<void()>([&] { zmqSocket->disconnect(endpoint); }));
}

void Socket::bind(std::string endpoint) {
    executor.injectionTest(new std::function<void()>([&] { zmqSocket->bind(endpoint); }));
}

void Socket::unbind(std::string endpoint) {
    executor.injectionTest(new std::function<void()>([&] { zmqSocket->unbind(endpoint); }));
}

void Socket::setXPubVerbose() {
    executor.injectionTest(new std::function<void()>([&] {
        if (zmqSocket->type() == zmqpp::socket_type::xpub) {
            zmqSocket->set(zmqpp::socket_option::xpub_verbose, true);
        }
    }));
}

void Socket::send(std::unique_ptr<std::function<zmqpp::message()>> messageSupplier) {
    executor.injectionTest(new std::function<void()>([&] {
        zmqpp::message message = messageSupplier->operator()(); //todo dont copy
        zmqSocket->send(message);
    }));
}





