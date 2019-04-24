//
// Created by palo on 4/24/19.
//

#include "SendService.h"

#define bufferSize 1024

SendService::SendService() {

    shouter = std::make_shared<ShoutService>(
            "127.0.0.1",
            8000,
            "/radio.ogg",
            "palo",
            "palo",
            SHOUT_FORMAT_MP3
    );

    encoder.setup();
    encodedQueue = encoder.getQueue();

}

void SendService::sendBlocking(int32_t *left, int32_t *right, size_t size) {

    /* mixing,encoding */
    encoder.encode(left, right, size);

    unsigned char sendBuffer[bufferSize];

    /* sending */
    while (!encodedQueue->empty()) {

        long sendSize = std::min(sizeof(sendBuffer), encodedQueue->size());

        for (int i = 0; i < sendSize; i++) {
            sendBuffer[i] = (unsigned char) encodedQueue->front();
            encodedQueue->pop();
        }

        shouter->send(sendBuffer, sendSize);

        // blocking here
        shouter->sync();
    }

}
