#include <utility>

//
// Created by palo on 4/24/19.
//

#include "Deck.h"

#define bufferSize 4096
#define SampleRate 44100

void Deck::load() {

    decoder = std::make_shared<Decoder>(filename);
    position = 0;

    // todo : for testing
    //cue_in = 2 * SampleRate;
    cue_in = 202333;

    //cue_out = 15 * SampleRate;
    cue_out = 4420414;

    decoder->seekToPosition(cue_in);
    position = cue_in;

}

size_t Deck::read(int32_t *left, int32_t *right, size_t size) {

    int32_t mixBuffer[bufferSize];


    size_t readSamples = size;
    if (!hitCue()) {
        readSamples = std::min((size_t) cue_out - position, size);
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

long Deck::getPosition() const {
    return position;
}

bool Deck::hitCue() {
    return position > cue_out;
}

Deck::Deck(Path filename) : filename(std::move(filename)) {

}

