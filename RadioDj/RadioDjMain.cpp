
#include "Services/Deck.h"
#include "Services/SendService.h"
#include "Services/ThreadSend.h"
#include "Services/ThreadMix.h"

#include <math.h>
#include <thread>
#include <string>
#include <memory>
#include <cstdio>
#include <iostream>
#include <stdexcept>

#include "Utils/Types.h"
#include "Services/NextTrackService.h"


int main(int argc, char **argv) {

    const char *command = argv[1];

    NextTrackService service;
    std::shared_ptr<Track> track = service.getNextFile(command);
    //printf("filename : %s\n", track->filename.c_str());
    //printf("cue in   : %li\n", track->cueIn);
    //printf("filename : %li\n", track->cueOut);
    //exit(1337)


    ThreadSend threadSend;
    threadSend.setup();

    ThreadMix threadMix;
    threadMix.setup(&threadSend);
    threadMix.load(service.getNextFile(command));
    threadMix.load(service.getNextFile(command));
    threadMix.load(service.getNextFile(command));

    std::thread t2(&ThreadMix::loop, &threadMix);
    std::thread t1(&ThreadSend::loop, &threadSend);

    while (true) {
        threadMix.wait();
        threadMix.load(service.getNextFile(command));
    }

    return 0;
}



