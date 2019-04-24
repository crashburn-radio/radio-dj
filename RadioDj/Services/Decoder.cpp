//
// Created by palo on 4/23/19.
//

#include <cstdio>
#include "Decoder.h"

Decoder::Decoder(const char *filename) {
    readSession = sox_open_read(filename, NULL, NULL, NULL);
}

size_t Decoder::read(sox_sample_t *buffer, size_t size) {
    return sox_read(readSession, buffer, size);
}

Decoder::~Decoder() {
    sox_close(readSession);
}

void Decoder::seekToPosition(long sample) {
    // todo : handle problem
    sox_seek(readSession, sample, SOX_SEEK_SET);
}
