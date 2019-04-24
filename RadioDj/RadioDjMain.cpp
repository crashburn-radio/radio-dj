#include <math.h>

#include "Services/Deck.h"
#include "Services/SendService.h"


#define bufferSize 512
#define  SampleRate 44100

int main() {


    auto *sendService = new SendService();

    auto deckA = new Deck();
    auto deckB = new Deck();
    auto deckC = new Deck();

    const char *filename = "/home/palo/input.ogg";
    deckA->load(filename);
    deckB->load(filename);
    deckC->load(filename);

    int32_t deckALeft[bufferSize];
    int32_t deckARight[bufferSize];

    int32_t deckBLeft[bufferSize];
    int32_t deckBRight[bufferSize];

    int32_t mixLeft[bufferSize];
    int32_t mixRight[bufferSize];

    double_t rampFactor = 1.0 / (5.0 * SampleRate);
    long rampCounter = 0;

    while (true) {

        /* reading */
        size_t readLength = deckA->read(
                (int32_t *) &deckALeft,
                (int32_t *) &deckARight,
                bufferSize);

        if (readLength == 0) {
            break;
        }

        if (!deckA->hitCue()) {
            sendService->sendBlocking(
                    (int32_t *) &deckALeft,
                    (int32_t *) &deckARight,
                    readLength);

            continue;
        }

        /* load second deck */
        size_t length = deckB->read(
                (int32_t *) &deckBLeft,
                (int32_t *) &deckBRight,
                readLength);

        if (length != readLength) {
            printf("can't handle different bufer sizes for reading %i != %i\n", length, readLength);
            exit(1);
        }


        /* mixing */
        double_t rampA;
        double_t rampB;
        for (int sample = 0; sample < readLength; sample++) {
            rampB = rampCounter * rampFactor;
            rampA = 1 - rampB;
            if (rampB >= 1) {
                rampB = 1;
                rampA = 0;
            }
            mixLeft[sample] = round(deckALeft[sample] * rampA) + round(deckBLeft[sample] * rampB);
            mixRight[sample] = round(deckARight[sample] * rampA) + round(deckBRight[sample] * rampB);
            rampCounter++;
        }

        sendService->sendBlocking(
                (int32_t *) &mixLeft,
                (int32_t *) &mixRight,
                readLength);

        if (rampA == 0) {
            // todo : hacky -> memory leak
            deckA = deckB;
            deckB = deckC;
            deckC = new Deck();
            deckC->load(filename);
            printf("switched Decks\n");
            rampCounter = 0;
        }

    }


    return 0;
}


