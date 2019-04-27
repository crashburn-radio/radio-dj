//
// Created by palo on 4/22/19.
//

#ifndef RADIODJ_SHOUTSERVICE_H
#define RADIODJ_SHOUTSERVICE_H

#include <shout/shout.h>

/**
 * A Service to talk to the IceCast Server
 */
class ShoutService {

public:

    /**
     * Create a ShoutService,
     * this Constructor already logging into
     * the server.
     *
     * @param host
     * @param port usually 8000
     * @param mount e.g.: "/radio.ogg"
     * @param password
     * @param username
     * @param format should be SHOUT_FORMAT_MP3
     */
    ShoutService(const char *host, int port, const char *mount, const char *username, const char *password, int format)
            : host(host),
              port(port),
              password(password),
              mount(mount),
              username(username),
              format(format) {
    }

    /**
     * Destroy ShoutService, will take care of
     * closing connection to the server
     */
    ~ShoutService() {
        this->shutdown();
    }


    /**
     * send a buffer to host
     */
    void send(unsigned char *buff, long read);

    /**
     * blocking command that syncs with icecast
     */
    void sync();


    void setup();

private:

    void shutdown();

    shout_t *shout;
    const char *host;
    int port;
    const char *password;
    const char *mount;
    const char *username;
    int format;
};


#endif //RADIODJ_SHOUTSERVICE_H
