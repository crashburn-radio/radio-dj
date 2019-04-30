#include <utility>

#ifndef RADIODJ_TYPES_H
#define RADIODJ_TYPES_H

#include <memory>
#include <string>

typedef std::shared_ptr<std::string> Path;

/*
 * Holding Audio Track Information for loading
 * it to a Deck
 */
struct Track {

    Path filename;

    /* cue point to seek to and fade in from */
    long cueIn;

    /* cue point to start fade out */
    long cueOut;

    Track(std::string filename, long cueIn, long cueOut) {
        this->filename = std::make_shared<std::string>(filename);
        this->cueIn = cueIn;
        this->cueOut = cueOut;
    }

};

#endif //RADIODJ_TYPES_H
