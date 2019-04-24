//
// Created by palo on 4/23/19.
//

#ifndef RADIODJ_DECODER_H
#define RADIODJ_DECODER_H

#include <sox.h>

class Decoder {

public:

    Decoder(const char *filename);


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
};


#endif //RADIODJ_DECODER_H
