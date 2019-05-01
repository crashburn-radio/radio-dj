#ifndef RADIODJ_ENCODERSERVICE_H
#define RADIODJ_ENCODERSERVICE_H

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/channel_layout.h>
#include <libavutil/common.h>
#include <libavutil/frame.h>
#include <libavutil/samplefmt.h>
}

#include <queue>
#include <memory>
#include <vector>

#define SAMPLE_RATE 44100

class EncoderService {
public:

    void setup();

    /**
     * encode stereo signal, output will be found in the encodedQueue
     *
     * @param leftInput buffer holding left channel samples
     * @param sizeLeft size of left buffer
     * @param rightInput buffer holding right channel samples
     * @param size size of left buffer
     */
    std::shared_ptr<std::vector<unsigned char>> encode(int32_t *leftInput, int32_t *rightInput, size_t size);

private:

    const AVCodec *codec = nullptr;
    AVCodecContext *pCodecContext = nullptr;
    AVFrame *frame = nullptr;

    /**
     * encode a frame properly setup by the encode function
     */
    std::shared_ptr<std::vector<unsigned char>> encodeFrame();

    std::queue<int32_t> *leftQueue = new std::queue<int32_t>;
    std::queue<int32_t> *rightQueue = new std::queue<int32_t>;

};


#endif //RADIODJ_ENCODERSERVICE_H
