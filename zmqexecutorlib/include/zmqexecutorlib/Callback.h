#ifndef ZMQEXECUTORLIB_CALLBACK_H
#define ZMQEXECUTORLIB_CALLBACK_H

class Callback {
public:
    virtual void callback() = 0;

    virtual ~Callback() = default;
};

#endif //ZMQEXECUTORLIB_CALLBACK_H
