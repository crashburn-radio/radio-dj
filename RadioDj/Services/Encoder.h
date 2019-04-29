#ifndef RADIODJ_ENCODER_H
#define RADIODJ_ENCODER_H

#define SAMPLE_RATE 44100
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/channel_layout.h>
#include <libavutil/common.h>
#include <libavutil/frame.h>
#include <libavutil/samplefmt.h>
}

#include <queue>

class Encoder {
public:

    void setup();

    // todo : create a queue of vectors or a return vector or something
    std::queue<unsigned char> *getQueue() const;

    /**
     * encode stereo signal, output will be found in the encodedQueue
     *
     * @param leftInput buffer holding left channel samples
     * @param sizeLeft size of left buffer
     * @param rightInput buffer holding right channel samples
     * @param size size of left buffer
     */
    void encode(int32_t *leftInput, int32_t *rightInput, size_t size);

private:

    const AVCodec *codec = nullptr;
    AVCodecContext *pCodecContext = nullptr;
    AVFrame *frame = nullptr;

    /**
     * encode a frame properly setup by the encode function
     */
    void encodeFrame();

    std::queue<unsigned char> *queue = new std::queue<unsigned char>;
    std::queue<int32_t> *leftQueue = new std::queue<int32_t>;
    std::queue<int32_t> *rightQueue = new std::queue<int32_t>;


};


#endif //RADIODJ_ENCODER_H
