
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/channel_layout.h>
#include <libavutil/common.h>
#include <libavutil/frame.h>
#include <libavutil/samplefmt.h>
}

#include "Encoder.h"
#include <math.h>
#include <cassert>

#define bitrate 320000

/* check that a given sample format is supported by the encoder */
static int check_sample_fmt(const AVCodec *codec,
                            enum AVSampleFormat sampleFormat) {

    const enum AVSampleFormat *pSampleFormat = codec->sample_fmts;

    while (*pSampleFormat != AV_SAMPLE_FMT_NONE) {
        // todo : only log if NDEBUG is set
        printf("Allowed SampleFormat by Codec : %s\n", av_get_sample_fmt_name(*pSampleFormat));
        if (*pSampleFormat == sampleFormat) {
            return 1;
        }
        pSampleFormat++;
    }

    return 0;
}

/* just pick the highest supported samplerate */
static int select_sample_rate(const AVCodec *codec) {
    const int *pCurrentSampleRatePossibility;
    int bestSampleRate = 0;
    if (!codec->supported_samplerates) {
        return SAMPLE_RATE;
    }
    pCurrentSampleRatePossibility = codec->supported_samplerates;
    while (*pCurrentSampleRatePossibility) {
        if (!bestSampleRate || abs(SAMPLE_RATE - *pCurrentSampleRatePossibility) < abs(SAMPLE_RATE - bestSampleRate))
            bestSampleRate = *pCurrentSampleRatePossibility;
        pCurrentSampleRatePossibility++;
    }
    return bestSampleRate;
}

/* select layout with the highest channel count */
static int select_channel_layout(const AVCodec *codec) {
    if (!codec->channel_layouts) {
        return AV_CH_LAYOUT_STEREO;
    }
    const uint64_t *pCurrentChannelLayout = codec->channel_layouts;
    int bestNbChannels = 0;
    uint64_t bestChannelLayout = 0;
    while (*pCurrentChannelLayout) {
        int nbChannels = av_get_channel_layout_nb_channels(*pCurrentChannelLayout);
        if (nbChannels > bestNbChannels) {
            bestChannelLayout = *pCurrentChannelLayout;
            bestNbChannels = nbChannels;
        }
        pCurrentChannelLayout++;
    }
    return bestChannelLayout;
}

void Encoder::setup() {
    int returnValue;

    codec = avcodec_find_encoder(AV_CODEC_ID_MP3);
    if (!codec) {
        fprintf(stderr, "Codec not found\n");
        exit(1);
    }
    pCodecContext = avcodec_alloc_context3(codec);
    if (!pCodecContext) {
        fprintf(stderr, "Could not allocate audio codec context\n");
        exit(1);
    }

    /* put sample parameters */
    pCodecContext->bit_rate = bitrate;
    pCodecContext->sample_fmt = AV_SAMPLE_FMT_S32P;
    if (!check_sample_fmt(codec, pCodecContext->sample_fmt)) {
        fprintf(stderr, "Encoder does not support sample format %s",
                av_get_sample_fmt_name(pCodecContext->sample_fmt));
        exit(1);
    }

    /* select other audio parameters supported by the encoder */
    pCodecContext->sample_rate = select_sample_rate(codec);
    pCodecContext->channel_layout = select_channel_layout(codec);
    pCodecContext->channels = av_get_channel_layout_nb_channels(pCodecContext->channel_layout);

    /* open it */
    if (avcodec_open2(pCodecContext, codec, NULL) < 0) {
        fprintf(stderr, "Could not open codec\n");
        exit(1);
    }

    /* packet for holding encoded output */
    /* frame containing input raw audio */
    frame = av_frame_alloc();
    if (!frame) {
        fprintf(stderr, "Could not allocate audio frame\n");
        exit(1);
    }
    frame->nb_samples = pCodecContext->frame_size;
    frame->format = pCodecContext->sample_fmt;
    frame->channel_layout = pCodecContext->channel_layout;

    /* allocate the data buffers */
    returnValue = av_frame_get_buffer(frame, 0);
    if (returnValue < 0) {
        fprintf(stderr, "Could not allocate audio data buffers\n");
        exit(1);
    }

    returnValue = av_frame_make_writable(frame);
    if (returnValue < 0) {
        fprintf(stderr, "Could not make frame writable\n");
        exit(1);
    }

}

void Encoder::encode(int32_t *leftInput,
                     int32_t *rightInput,
                     size_t size) {

    for (int index = 0; index < size; index++) {
        leftQueue->push(leftInput[index]);
        rightQueue->push(rightInput[index]);
    }

    auto *left = (int32_t *) frame->data[0];
    auto *right = (int32_t *) frame->data[1];

    int frameSize = pCodecContext->frame_size;

    while (rightQueue->size() > frameSize) {

        for (int index = 0; index < frameSize; index++) {

            left[index] = leftQueue->front();
            leftQueue->pop();

            right[index] = rightQueue->front();
            rightQueue->pop();

        }

        encodeFrame();

    }

}

void Encoder::encodeFrame() {
    int returnValue;

    /* send the frame for encoding */
    returnValue = avcodec_send_frame(pCodecContext, frame);
    if (returnValue < 0) {
        fprintf(stderr, "Error sending the frame to the encoder\n");
        exit(1);
    }

    /* read all the available output packets (in general there may be any
     * number of them */
    AVPacket *pPacket = av_packet_alloc();
    if (!pPacket) {
        fprintf(stderr, "could not allocate the packet\n");
        exit(1);
    }
    while (returnValue >= 0) {
        returnValue = avcodec_receive_packet(pCodecContext, pPacket);
        if (returnValue == AVERROR(EAGAIN) || returnValue == AVERROR_EOF) {
            return;
        } else if (returnValue < 0) {
            fprintf(stderr, "Error encoding audio frame\n");
            exit(1);
        }

        /* fill output buffer */
        for (int index = 0; index < pPacket->size; index++) {
            queue->push(pPacket->data[index]);
        }

        av_packet_unref(pPacket);
    }

}

std::queue<unsigned char> *Encoder::getQueue() const {
    return queue;
}

