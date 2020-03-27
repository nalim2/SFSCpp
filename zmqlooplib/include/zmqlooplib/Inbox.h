#ifndef ZMQLOOPLIB_INBOX_H
#define ZMQLOOPLIB_INBOX_H

#include <zmqpp/message.hpp>
#include <memory>

class Inbox {
public:
    virtual void receive(std::unique_ptr<zmqpp::message> message) = 0;

    virtual ~Inbox() = default;
};

#endif //ZMQLOOPLIB_INBOX_H
