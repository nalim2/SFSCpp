cmake_minimum_required(VERSION 3.14)
project(SFSCpp)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread") #needed for multithreading

link_libraries(zmqpp zmqpp) #zmqpp library needs to be installed before

find_package(JNI REQUIRED) #jni is usually automatically installed with the jvm
include_directories(${JNI_INCLUDE_DIRS})

add_library(ZmqExecutorLib SHARED
        zmqexecutorlib/include/zmqexecutorlib/Socket.h
        zmqexecutorlib/include/zmqexecutorlib/Inbox.h
        zmqexecutorlib/include/zmqexecutorlib/ZmqExecutor.h
        zmqexecutorlib/include/zmqexecutorlib/Callback.h

        zmqexecutorlib/src/notifier.h
        zmqexecutorlib/src/taskQueue.h
        zmqexecutorlib/src/ZmqExecutor.cpp
        zmqexecutorlib/src/Socket.cpp)
target_include_directories(ZmqExecutorLib PUBLIC zmqexecutorlib/include)
install(TARGETS ZmqExecutorLib)

add_library(JniZmq SHARED
        jnizmq/src/impl/de_unistuttgart_isw_sfsc_commonjava_zmq_reactor_jni_JniReactiveSocket.cpp
        jnizmq/src/impl/de_unistuttgart_isw_sfsc_commonjava_zmq_reactor_jni_JniReactor.cpp
        jnizmq/src/impl/JavaArrayRefReleaser.h
        jnizmq/src/impl/JvmManager.cpp
        jnizmq/src/impl/JvmManager.h
        jnizmq/src/impl/util.h
        jnizmq/src/impl/JavaInboxProxy.h
        jnizmq/src/impl/JavaShutdownCallbackProxy.h)
target_include_directories(JniZmq PUBLIC jnizmq)
target_link_libraries(JniZmq PUBLIC ZmqExecutorLib)
install(TARGETS JniZmq)
