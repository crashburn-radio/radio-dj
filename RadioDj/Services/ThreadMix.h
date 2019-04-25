//
// Created by palo on 4/26/19.
//

#ifndef RADIODJ_THREADMIX_H
#define RADIODJ_THREADMIX_H

#include "ThreadSend.h"
#include "Deck.h"
#include "../Utils/Types.h"

/**
 * A class that mixes tracks and sends them to
 * the Send thread
 */
class ThreadMix {

public:

    /*
     * setup called before the main loop
     */
    void setup(ThreadSend *threadSend);

    /*
     * main loop
     * 3 decks must be loaded before calling this command
     */
    void loop();

    /**
    * blocks until thread is ready to accept more commands
    */
    void wait();

    void load(Path filename);

private:

    ThreadSend *threadSend;

    std::shared_ptr<Deck> deckA;
    std::shared_ptr<Deck> deckB;
    std::shared_ptr<Deck> deckC;

    /**
     * command message queue
     */
    BlockingQueue<std::shared_ptr<Deck>> commandQueue;

    /* back pressure */
    std::condition_variable backpressureConditional;
    std::mutex backpressureMutex;
};


#endif //RADIODJ_THREADMIX_H
