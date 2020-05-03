#ifndef ZMQEXECUTORLIB_INBOX_H
#define ZMQEXECUTORLIB_INBOX_H

#include <zmqpp/message.hpp>
#include <memory>

/**
 * Virtual Class, used for callback on message receipt.
 */
class Inbox {
public:
    /**
     * This method will be called every time a message is received.
     * @param message the received message
     */
    virtual void receive(std::unique_ptr<zmqpp::message> message) = 0;

    virtual ~Inbox() = default;
};

#endif //ZMQEXECUTORLIB_INBOX_H
