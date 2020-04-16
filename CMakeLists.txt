cmake_minimum_required(VERSION 3.15)
project(ZmqLoopLib)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

link_libraries(zmqpp zmqpp)

find_package(JNI REQUIRED)
include_directories(${JNI_INCLUDE_DIRS})

add_library(zmqLoopLib SHARED
        zmqlooplib/include/zmqlooplib/Socket.h
        zmqlooplib/include/zmqlooplib/Inbox.h
        zmqlooplib/include/zmqlooplib/ZmqExecutor.h
        zmqlooplib/include/zmqlooplib/Callback.h

        zmqlooplib/src/util/notifier.cpp
        zmqlooplib/src/util/notifier.h
        zmqlooplib/src/util/taskQueue.cpp
        zmqlooplib/src/util/taskQueue.h

        zmqlooplib/src/zmq/ZmqExecutor.cpp
        zmqlooplib/src/zmq/Socket.cpp)
target_include_directories(zmqLoopLib PUBLIC zmqlooplib/include)

add_library(jniZmq SHARED

        jnizmq/src/impl/de_unistuttgart_isw_sfsc_commonjava_zmq_reactor_jni_JniReactiveSocket.cpp
        jnizmq/src/impl/de_unistuttgart_isw_sfsc_commonjava_zmq_reactor_jni_JniReactor.cpp
        jnizmq/src/impl/JavaArrayRefReleaser.h
        jnizmq/src/impl/JvmManager.cpp
        jnizmq/src/impl/JvmManager.h
        jnizmq/src/impl/util.h
        jnizmq/src/impl/JavaInboxProxy.h
        jnizmq/src/impl/JavaShutdownCallbackProxy.h)
target_include_directories(jniZmq PUBLIC jnizmq)
target_link_libraries(jniZmq PUBLIC zmqLoopLib)