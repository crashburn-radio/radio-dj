#ifndef RADIODJ_DECODER_H
#define RADIODJ_DECODER_H

#include <sox.h>
#include <memory>
#include <string>
#include "../Utils/Types.h"

class Decoder {

public:

    explicit Decoder(Path filename);

    void seekToPosition(long sample);

    /**
     * read from session and write to buffer
     *
     * @param buffer  buffer to write to
     * @param size  how much to write
     * @return  how much was actually written
     */
    size_t read(sox_sample_t *buffer, size_t size);

    /**
     * destroy Decoder takes care of closing everything
     */
    ~Decoder();


private:
    sox_format_t *readSession;
    Path filename;
};


#endif //RADIODJ_DECODER_H
