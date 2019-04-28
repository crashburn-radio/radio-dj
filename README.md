# Radio Dj

Auto Radio Dj tool set, with the following feature:

* cross-fading on beat-grid cue points
* you can use your favorite programming language to determine the next track and cue points

## Commands in the nix package

* `RadioDj` is an icecast streamer, that needs a callback function as argument to play the next track (see `shell.nix`)
* `review-track` a tool to create crates with manually adjusted cue points.
* `print-track` a script that returns `json` for `RadioDj` to read in the next track to play.

## Folder structure

* `./default.nix` contains the nix package, which holds all needed tools
* `./shell.nix` is used for development, but also shows how to use the package. look for the `radio-run` script.
* `CMakeList.txt` the cmake build script.


