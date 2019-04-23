//
// Created by palo on 4/22/19.
//

#ifndef RADIODJ_SHOUTSERVICE_H
#define RADIODJ_SHOUTSERVICE_H


class ShoutService {
public:
    /**
     * setup shout service
     */
    void setup();


    /**
     * send a buffer
     */
    void send(unsigned char *buff, long read);


    /**
     * blocking command that syncs with icecast
     */
    void sync();

    /**
     * shut down system
     */
    void shutdown();

private:
    shout_t *shout;

};


#endif //RADIODJ_SHOUTSERVICE_H
