#include <memory>
#include <iostream>
#include "NextTrackService.h"

std::shared_ptr<Track> NextTrackService::getNextFile(const char *command) {

    const std::string string = exec(command);

    json_error_t error;
    json_t *root = json_loads(string.c_str(), 0, &error);
    if (!root) {
        fprintf(stderr, "error: on line %d: %s\n", error.line, error.text);
        // todo : try 5 times before exit(1);
        exit(1);
        // return nullptr;
    }

    if (!json_is_object(root)) {
        fprintf(stderr, "expected json object\n");
        // todo : try 5 times before exit(1);
        exit(1);
        //return nullptr;
    }

    std::string filename = std::string(json_string_value(json_object_get(root, "filename")));
    long cue_in = (long) json_integer_value(json_object_get(root, "cueIn"));
    long cue_out = (long) json_integer_value(json_object_get(root, "cueOut"));

    std::shared_ptr<Track> result = std::make_shared<Track>(
            filename, cue_in, cue_out
    );

    json_delete(root);
    return result;

}

/**
 * execute command
 *
 * @param command  to execute
 * @return  string containing the output of command
 */
std::string NextTrackService::exec(const char *command) {
    std::array<char, 128> buffer{};
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

