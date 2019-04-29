#include <cstdio>
#include <stdexcept>
#include <unistd.h>
#include "ShoutService.h"

void ShoutService::connect() {
    long returnValue;

    shout_init();

    if (!(shout = shout_new())) {
        printf("Could not allocate shout_t\n");
        throw std::runtime_error("Could not allocate shout_t");
    }

    if (shout_set_host(shout, host) != SHOUTERR_SUCCESS) {
        printf("Error setting hostname: %s\n", shout_get_error(shout));
        throw std::runtime_error("Error setting hostname");
    }

    if (shout_set_protocol(shout, SHOUT_PROTOCOL_HTTP) != SHOUTERR_SUCCESS) {
        printf("Error setting protocol: %s\n", shout_get_error(shout));
        throw std::runtime_error("Error setting protocol");
    }

    if (shout_set_port(shout, port) != SHOUTERR_SUCCESS) {
        printf("Error setting port: %s\n", shout_get_error(shout));
        throw std::runtime_error("Error setting port");
    }

    if (shout_set_password(shout, password) != SHOUTERR_SUCCESS) {
        printf("Error setting password: %s\n", shout_get_error(shout));
        throw std::runtime_error("Error setting password");
    }

    if (shout_set_mount(shout, mount) != SHOUTERR_SUCCESS) {
        printf("Error setting mount: %s\n", shout_get_error(shout));
        throw std::runtime_error("Error setting mount");
    }

    if (shout_set_user(shout, username) != SHOUTERR_SUCCESS) {
        printf("Error setting user: %s\n", shout_get_error(shout));
        throw std::runtime_error("Error setting username");
    }

    if (shout_set_format(shout, format) != SHOUTERR_SUCCESS) {
        printf("Error setting format: %s\n", shout_get_error(shout));
        throw std::runtime_error("Error setting format");
    }

    returnValue = shout_open(shout);

    if (returnValue == SHOUTERR_SUCCESS) {
        returnValue = SHOUTERR_CONNECTED;
    }

    while (returnValue == SHOUTERR_BUSY) {
        printf("Connection pending. Sleeping...\n");
        sleep(1);
        returnValue = shout_get_connected(shout);
    }

    if (returnValue == SHOUTERR_CONNECTED) {
        printf("Connected to server...\n");
    } else {
        printf("Error connecting: %s\n", shout_get_error(shout));
        throw std::runtime_error("Error connecting");
    }

}

void ShoutService::send(unsigned char *buff, long read) {
    long returnValue = shout_send(shout, buff, read);
    if (returnValue != SHOUTERR_SUCCESS) {
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

