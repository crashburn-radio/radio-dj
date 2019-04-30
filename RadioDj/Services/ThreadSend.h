#ifndef RADIODJ_THREADSEND_H
#define RADIODJ_THREADSEND_H

#include <memory>
#include <vector>
#include <functional>
#include <condition_variable>
#include "../Utils/BlockingQueue.tpp"
#include "SendService.h"


struct SendCommand {
    std::shared_ptr<std::vector<int32_t>> left;
    std::shared_ptr<std::vector<int32_t>> right;
};

class ThreadSend {


public:

    ThreadSend(const char *host, int port, const char *mount, const char *username, const char *password);

    void setup();

    void loop();

    /**
     * send buffers to be send to icecast
     * @param left  channel
     * @param right  channel
     * @param size samples to be read
     */
    void send(int32_t *left, int32_t *right, size_t size);

    /**
     * blocks until thread is ready to accept more commands
     */
    void wait();

private:

    /**
     * service to run in thread
     */
    std::shared_ptr<SendService> sendService;

    /**
     * command message queue
     *
     * todo : use lambdas here if more types of commands are needed
     */
    BlockingQueue<SendCommand> commandQueue;


    /* back pressure */
    std::condition_variable backpressureConditional;
    std::mutex backpressureMutex;
};


#endif //RADIODJ_THREADSEND_H
