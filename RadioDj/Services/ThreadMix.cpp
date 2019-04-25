//
// Created by palo on 4/26/19.
//

#include <cstdint>
#include <cmath>
#include "ThreadMix.h"
#include "ThreadSend.h"

#define bufferSize 512
#define  SampleRate 44100

void ThreadMix::setup(ThreadSend *threadSend) {
    // todo use in constructor
    this->threadSend = threadSend;
}

void ThreadMix::loop() {

    int32_t deckALeft[bufferSize];
    int32_t deckARight[bufferSize];

    int32_t deckBLeft[bufferSize];
    int32_t deckBRight[bufferSize];

    int32_t mixLeft[bufferSize];
    int32_t mixRight[bufferSize];

    int32_t * sendLeft;
    int32_t * sendRight;


    double_t rampFactor = 1.0 / (5.0 * SampleRate);
    long rampSampleCounter = 0;

    deckA = commandQueue.pop();
    deckA->load();
    deckB = commandQueue.pop();
    deckB->load();

    double_t deckARampValue = 0;
    double_t deckBRampValue;

    int commandQueueBufferSize = 0;

    while (true) {

        /* stop if threadSend is full */
        commandQueueBufferSize++;
        if (commandQueueBufferSize > 30) {
            threadSend->wait();
            commandQueueBufferSize = 0;
        }

        /* pull new Deck if needed */
        if (commandQueue.isFilled() && deckC == nullptr) {
            deckC = commandQueue.pop();
            deckC->load();
        }

        /* reading Main Deck */
        size_t readLength = deckA->read(
                (int32_t *) &deckALeft,
                (int32_t *) &deckARight,
                bufferSize);

        // todo something here
        if (readLength == 0) {
            break;
        }

        /* just play first deck */
        if (!deckA->hitCue()) {
            threadSend->send(
                    (int32_t *) &deckALeft,
                    (int32_t *) &deckARight,
                    readLength);
            continue;
        }

        /* load second deck */
        size_t length = deckB->read(
                (int32_t *) &deckBLeft,
                (int32_t *) &deckBRight,
                readLength);
        if (length != readLength) {
            printf("can't handle different bufer sizes for reading %li != %li\n", length, readLength);
            exit(1);
        }


        /* mix both tracks */
        for (int sample = 0; sample < readLength; sample++) {
            deckBRampValue = rampSampleCounter * rampFactor;
            deckARampValue = 1 - deckBRampValue;
            // deal with calculation errors
            if (deckBRampValue >= 1) {
                deckBRampValue = 1;
                deckARampValue = 0;
            }
            mixLeft[sample] = round(deckALeft[sample] * deckARampValue) + round(deckBLeft[sample] * deckBRampValue);
            mixRight[sample] = round(deckARight[sample] * deckARampValue) + round(deckBRight[sample] * deckBRampValue);
            rampSampleCounter++;
        }

        /* send mixed tracks */
        threadSend->send(
                (int32_t *) &mixLeft,
                (int32_t *) &mixRight,
                readLength);


        /* change main deck */
        if (deckARampValue == 0) {
            deckA = deckB;
            deckB = deckC;
            deckC = nullptr;
            backpressureConditional.notify_all();
            printf("switched Decks\n");
            rampSampleCounter = 0;
        }

    }
}

void ThreadMix::wait() {
    std::unique_lock<std::mutex> lock(backpressureMutex);
    backpressureConditional.wait(lock);
}

void ThreadMix::load(Path filename) {
    commandQueue.push(std::make_shared<Deck>(filename));
}

