
#include "Services/Deck.h"
#include "Services/SendService.h"
#include "Services/ThreadSend.h"
#include "Services/ThreadMix.h"

#include <math.h>
#include <thread>
#include <string>
#include <memory>

#include "Utils/Types.h"

#define bufferSize 512
#define  SampleRate 44100

int main() {

    ThreadSend threadSend;
    threadSend.setup();

    Path filename = std::make_shared<std::string>("/home/palo/input.ogg");
    ThreadMix threadMix;
    threadMix.setup(&threadSend);
    threadMix.load(filename);
    threadMix.load(filename);
    threadMix.load(filename);

    std::thread t2(&ThreadMix::loop, &threadMix);
    std::thread t1(&ThreadSend::loop, &threadSend);

    while (true) {
        threadMix.wait();
        threadMix.load(filename);
    }

    return 0;
}


