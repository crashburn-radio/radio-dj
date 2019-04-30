#include <utility>
#include <cstdio>
#include <cstring>
#include "Decoder.h"


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

Decoder::Decoder(Path filename) : filename(std::move(filename)) {

    // todo : delay this through a load function
    readSession = sox_open_read(
            this->filename->c_str(), nullptr,
            nullptr,
            nullptr);
}
