//
// Created by palo on 4/22/19.
//

#ifndef RADIODJ_ENCODER_H
#define RADIODJ_ENCODER_H

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

    int sample_rate();

    int frameSize();

    int32_t *getDeprecatedDataLeft();

    int32_t *getDeprecatedDataRight();

    int channels();

    void encodeToQueue(std::queue<unsigned char> *queue);

private:
    const AVCodec *codec;
    AVCodecContext *c = NULL;
    AVFrame *frame;
    AVPacket *pkt;
};


#endif //RADIODJ_ENCODER_H
