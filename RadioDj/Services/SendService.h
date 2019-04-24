//
// Created by palo on 4/24/19.
//

#ifndef RADIODJ_SENDSERVICE_H
#define RADIODJ_SENDSERVICE_H


#include "Encoder.h"
#include "ShoutService.h"
#include <memory>

class SendService {
public:

    SendService();

    void sendBlocking(int32_t *left, int32_t *right, size_t size);

private:
    Encoder encoder = Encoder();
    std::shared_ptr<ShoutService> shouter;
    std::queue<unsigned char> *encodedQueue;
};


#endif //RADIODJ_SENDSERVICE_H
