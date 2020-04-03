#ifndef ZMQLOOPLIB_CALLBACK_H
#define ZMQLOOPLIB_CALLBACK_H

class Callback {
public:
    virtual void callback() = 0;

    virtual ~Callback() = default;
};

#endif //ZMQLOOPLIB_CALLBACK_H
