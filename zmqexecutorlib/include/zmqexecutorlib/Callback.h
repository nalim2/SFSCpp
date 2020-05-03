#ifndef ZMQEXECUTORLIB_CALLBACK_H
#define ZMQEXECUTORLIB_CALLBACK_H

/**
 * Virtual Class, used for callback on shutdown.
 */
class Callback {
public:
    /**
     * This method will be executed once on shutdown.
     */
    virtual void callback() = 0;

    virtual ~Callback() = default;
};

#endif //ZMQEXECUTORLIB_CALLBACK_H
