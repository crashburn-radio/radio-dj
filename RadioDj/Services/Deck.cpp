#include <utility>
#include <iostream>

//
// Created by palo on 4/24/19.
//

#include "Deck.h"

#define bufferSize 4096
#define SampleRate 44100

void Deck::load() {

    decoder = std::make_shared<Decoder>(track->filename);
    position = 0;

    cueIn = track->cueIn;
    cueOut = track->cueOut;

    decoder->seekToPosition(cueIn);
    position = cueIn;
    std::cout << "Loaded Track : " << * track->filename << "\n";
}

size_t Deck::read(int32_t *left, int32_t *right, size_t size) {

    int32_t mixBuffer[bufferSize];

    size_t readSamples = size;
    if (!hitCue()) {
        readSamples = std::min((size_t) cueOut - position, size);
        if (readSamples == 0) {
            readSamples = size;
        }
    }
    size_t toRead = std::min(2 * readSamples, (size_t) bufferSize);
    size_t readLength = decoder->read(mixBuffer, toRead);

    for (int index = 0; index < (readLength / 2); index++) {
        left[index] = (int32_t) mixBuffer[2 * index];
        right[index] = (int32_t) mixBuffer[2 * index + 1];
    }

    position += (readLength / 2);

    return readLength / 2;
}

bool Deck::hitCue() {
    return position > cueOut;
}

std::shared_ptr<std::string> Deck::getFilename() {
    return this->track->filename;
}

Deck::Deck(std::shared_ptr<Track> track) : track(std::move(track)) {

}

