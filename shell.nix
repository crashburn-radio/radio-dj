{ pkgs ?  import <nixpkgs> {} }:

let

  libraries = [
    pkgs.libshout
    pkgs.sox
    pkgs.ffmpeg-full
    pkgs.jansson
  ];

  buildTools = [
    pkgs.cmake
    pkgs.cmakeCurses
    pkgs.pkgconfig
  ];

  ide = [
    pkgs.idea.clion
    pkgs.hexyl
    pkgs.aubio
  ];

  radioPkgs = pkgs.callPackage ./default.nix {};

  # todo : spaces in files are still a problem!
  nextTrackScript = folder:
  pkgs.writeShellScriptBin "nextTrack" /* sh */ ''
    export PATH="${pkgs.lib.makeBinPath [
      pkgs.coreutils
      pkgs.findutils
      pkgs.gnugrep
    ]}"
    TRACK=$(find ${folder} -type f  | egrep "(mp3$|ogg$)" | shuf -n 1)
    ${radioPkgs}/bin/print-track "$TRACK"
  '';

  runRadio = pkgs.writeShellScriptBin "radio-run" /* sh */ ''
  ${radioPkgs}/bin/RadioDj \
    localhost \
    8000 \
    /radio.ogg \
    palo \
    palo \
    ${nextTrackScript "/home/palo/music-library/techno"}/bin/nextTrack
  '';

in

pkgs.mkShell {

  buildInputs =
    libraries
    ++ buildTools
    ++ ide
    ++ [
      radioPkgs
      (nextTrackScript "/home/palo/music-library" )
      runRadio
    ]
    ;

  shellHook = /* sh */ ''
  HISTFILE=${toString ./.}/.history
  '';
}
