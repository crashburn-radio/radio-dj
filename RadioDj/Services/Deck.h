//
// Created by palo on 4/24/19.
//

#ifndef RADIODJ_DECK_H
#define RADIODJ_DECK_H

#include <cstdint>
#include <memory>
#include "Decoder.h"

/**
 * A dec holds a sound file and can be mixed with other decks
 */
class Deck {

public:
    /**
     * position for playback.
     * @return
     */
    long getPosition() const;

    /**
     * load a file to the deck ready for reading from it.
     *
     * @param filename path to the file to load
     */
    void load(const char *filename);


    /**
     * read samples from the deck.
     * Will stop when hitting the cue_out point,
     * so you can start mixing, right after it.
     *
     * @param left  channel to read to
     * @param right  channel to read to
     * @param size  how much samples to read on each channel
     * @return actual read samples for each channel (not in sum)
     */
    size_t read(int32_t *left, int32_t *right, size_t size);

    /**
     * @return true if cue_out point is passed
     */
    bool hitCue();

private:

    std::shared_ptr<Decoder> decoder;

    long position = 0;

    /**
     * cue sample to start track
     */
    long cue_in;

    /**
     * cue sample to start fadeout
     */
    long cue_out;
};


#endif //RADIODJ_DECK_H
