#include <utility>

#include <utility>

#ifndef RADIODJ_SENDSERVICE_H
#define RADIODJ_SENDSERVICE_H


#include "EncoderService.h"
#include "ShoutService.h"
#include <memory>

class SendService {
public:

    SendService(std::shared_ptr<EncoderService> encoder, std::shared_ptr<ShoutService> shouter)
            : encoderService(std::move(encoder)), shoutService(std::move(shouter)) {}

    void setup();

    void sendBlocking(int32_t *left, int32_t *right, size_t size);

private:
    std::shared_ptr<EncoderService> encoderService;
    std::shared_ptr<ShoutService> shoutService;
};


#endif //RADIODJ_SENDSERVICE_H
