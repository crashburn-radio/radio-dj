//
// Created by palo on 4/24/19.
//

#ifndef RADIODJ_DECK_H
#define RADIODJ_DECK_H

#include <cstdint>
#include <memory>
#include "Decoder.h"
#include "../Utils/Types.h"

/**
 * A dec holds a sound file and can be mixed with other decks
 */
class Deck {

public:

    explicit Deck(std::shared_ptr<Track> track);

    /**
     * load a file to the deck ready for reading from it.
     *
     * @param filename path to the file to load
     */
    void load();


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

    /**
     * filename of currently loaded track
     */
    std::shared_ptr<std::string> getFilename();


private:

    std::shared_ptr<Track> track;

    std::shared_ptr<Decoder> decoder;

    long position = 0;

    /**
     * cue sample to start track
     */
    long cueIn{};

    /**
     * cue sample to start fadeout
     */
    long cueOut{};
};


#endif //RADIODJ_DECK_H
