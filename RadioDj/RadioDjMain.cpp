
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
#include <sstream>

#include "Utils/Types.h"
#include "Services/NextTrackService.h"


int main(int argc, char **argv) {


    NextTrackService service;


    char *host = argv[1];

    std::stringstream convert(
            argv[2]); // set up a stringstream variable named convert, initialized with the input from argv[1]
    int port;
    if (!(convert >> port)) { // do the conversion
        std::cerr << "Port is not a number" << port << "\n";
        exit(1);
    }

    char *mount = argv[3];
    char *username = argv[4];
    char *password = argv[5];
    const char *command = argv[6];

    /* icecast sender thread */
    ThreadSend threadSend(host, port, mount, username, password);
    threadSend.setup();

    /* preload decks */
    ThreadMix threadMix;
    threadMix.setup(&threadSend);
    threadMix.load(service.getNextFile(command));
    threadMix.load(service.getNextFile(command));
    threadMix.load(service.getNextFile(command));

    /* start playback and mixing threads */
    std::thread t2(&ThreadMix::loop, &threadMix);
    std::thread t1(&ThreadSend::loop, &threadSend);


    /* load new tracks, when deckA is switched to deckB */
    while (true) {
        threadMix.wait();
        threadMix.load(service.getNextFile(command));
    }

    return 0;
}



