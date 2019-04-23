//
// Created by palo on 4/22/19.
//

#include <shout/shout.h>
#include <cstdio>
#include <stdexcept>
#include <unistd.h>
#include "ShoutService.h"

void ShoutService::setup() {
    long rett;

    shout_init();

    if (!(shout = shout_new())) {
        printf("Could not allocate shout_t\n");
        throw std::runtime_error("penis");
    }

    if (shout_set_host(shout, "127.0.0.1") != SHOUTERR_SUCCESS) {
        printf("Error setting hostname: %s\n", shout_get_error(shout));
        throw std::runtime_error("penis");
    }

    if (shout_set_protocol(shout, SHOUT_PROTOCOL_HTTP) != SHOUTERR_SUCCESS) {
        printf("Error setting protocol: %s\n", shout_get_error(shout));
        throw std::runtime_error("penis");
    }

    if (shout_set_port(shout, 8000) != SHOUTERR_SUCCESS) {
        printf("Error setting port: %s\n", shout_get_error(shout));
        throw std::runtime_error("penis");
    }

    if (shout_set_password(shout, "palo") != SHOUTERR_SUCCESS) {
        printf("Error setting password: %s\n", shout_get_error(shout));
        throw std::runtime_error("penis");
    }
    if (shout_set_mount(shout, "/radio.ogg") != SHOUTERR_SUCCESS) {
        printf("Error setting mount: %s\n", shout_get_error(shout));
        throw std::runtime_error("penis");
    }

    if (shout_set_user(shout, "palo") != SHOUTERR_SUCCESS) {
        printf("Error setting user: %s\n", shout_get_error(shout));
        throw std::runtime_error("penis");
    }

    if (shout_set_format(shout, SHOUT_FORMAT_MP3) != SHOUTERR_SUCCESS) {
        printf("Error setting user: %s\n", shout_get_error(shout));
        throw std::runtime_error("penis");
    }

    //if (shout_set_nonblocking(shout, 1) != SHOUTERR_SUCCESS) {
    //    printf("Error setting non-blocking mode: %s\n", shout_get_error(shout));
    //    throw std::runtime_error("penis");
    //}

    rett = shout_open(shout);

    if (rett == SHOUTERR_SUCCESS) {
        rett = SHOUTERR_CONNECTED;
    }

    while (rett == SHOUTERR_BUSY) {
        printf("Connection pending. Sleeping...\n");
        sleep(1);
        rett = shout_get_connected(shout);
    }

    if (rett == SHOUTERR_CONNECTED) {
        printf("Connected to server...\n");
    } else {
        printf("Error connecting: %s\n", shout_get_error(shout));
        throw std::runtime_error("penis");
    }


}

void ShoutService::send(unsigned char *buff, long read) {
    long rett = shout_send(shout, buff, read);
    if (rett != SHOUTERR_SUCCESS) {
        printf("DEBUG: Send error: %s\n", shout_get_error(shout));
        throw std::runtime_error("penis");
    }

    if (shout_queuelen(shout) > 0) {
        printf("DEBUG: queue length: %d\n", (int) shout_queuelen(shout));
    }

}

void ShoutService::sync() {
    shout_sync(shout);
}

void ShoutService::shutdown() {
    shout_close(shout);
    shout_shutdown();

}
