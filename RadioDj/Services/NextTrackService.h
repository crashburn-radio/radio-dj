//
// Created by palo on 4/26/19.
//

#ifndef RADIODJ_NEXTTRACKSERVICE_H
#define RADIODJ_NEXTTRACKSERVICE_H

#include <string>
#include <array>
#include <jansson.h>
#include "../Utils/Types.h"


class NextTrackService {

public:
    std::shared_ptr<Track> getNextFile(const char *command);

private:
    std::string exec(const char *command);
};


#endif //RADIODJ_NEXTTRACKSERVICE_H
