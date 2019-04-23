/* -*- c-basic-offset: 8; -*-
 * example.c: Demonstration of the libshout API.
 * $Id: nonblocking.c 11584 2006-06-18 14:45:07Z msmith $
 */

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <vector>
#include <fstream>      // std::ifstream

#include <math.h>

extern "C" {
#include <libavcodec/avcodec.h>
//#include <libavfilter/avcodec.h>
//#include <libavfilter/avfiltergraph.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libavformat/avformat.h>
#include <libavutil/channel_layout.h>
#include <libavutil/common.h>
#include <libavutil/frame.h>
#include <libavutil/mem.h>
#include <libavutil/samplefmt.h>
}

#include <shout/shout.h>
#include "Services/ShoutService.h"
#include "Services/Encoder.h"

#include <sox.h>


int main() {

    const char *filename = "/home/palo/input.mp3";

    printf("setup Shout Service \n");
    ShoutService shouter = ShoutService();
    shouter.setup();

    Encoder encoder = Encoder();
    encoder.setup();

    sox_init();
    sox_format_init();

    /* create file reader */
    sox_format_t *openRead = sox_open_read(filename, NULL, NULL, NULL);

    /* mixing */
    sox_sample_t mixBuffer[1024 * 1024];

    int channels = encoder.channels();
    int frameSize = encoder.frameSize();
    int sampleRate = encoder.sample_rate();

    printf("frameSize %i", frameSize);
    std::queue<unsigned char> queue;

    unsigned char sendBuffer[512];
    // FILE *pFile = fopen("/home/palo/test.mp3", "w"); // todo delete

    int32_t *dataLeft = encoder.getDeprecatedDataLeft();
    int32_t *dataRight = encoder.getDeprecatedDataRight();

    while (true) {
        size_t readLength = sox_read(openRead, mixBuffer, 2 * frameSize);


        for (int index = 0; index < frameSize; index++) {
            dataLeft[index] = (int32_t) mixBuffer[2 * index];
            dataRight[index] = (int32_t) mixBuffer[2 * index + 1];
            //dataLeft[index] = 0;
            //dataRight[index] = 0;
        }
        // printf("sample :  %i,%i \n", (int32_t) mixBuffer[0], (int32_t) mixBuffer[1]);

        encoder.encodeToQueue(&queue);

        while (!queue.empty()) {

            long read = std::min(sizeof(sendBuffer), queue.size());

            for (int i = 0; i < read; i++) {
                sendBuffer[i] = (unsigned char) queue.front();
                queue.pop();
            }

            if (read > 0) {
                shouter.send(sendBuffer, read);

                // fwrite(sendBuffer, 1, read, pFile);
                // test exit
                //readLength = 0;
                //break;
            }

            shouter.sync();
        }

        if (readLength == 0) {
            break;
        }

    }
    //fclose(pFile);

    shouter.shutdown();

    return 0;
}


