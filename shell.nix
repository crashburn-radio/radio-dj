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


  /* the radio-dj tool set */
  radioPkgs = pkgs.callPackage ./default.nix {};

  /* the script that spits out the next track to play */
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

  tmpFile = "/dev/shm/render.wav";


  /* an exmaple script on how to create a service that runs this the RadioDj */
  runRadio = folder:
  pkgs.writeShellScriptBin "radio-run" /* sh */ ''
  ${radioPkgs}/bin/RadioDj \
    localhost \
    8000 \
    /radio.mp3 \
    username \
    password \
    ${nextTrackScript folder}/bin/nextTrack
  '';

in

pkgs.mkShell {

  buildInputs =
    libraries
    ++ buildTools
    ++ ide
    ++ [
      radioPkgs
      (nextTrackScript "./.playlist1" )
      (runRadio "./.playlist1")
      pkgs.pup
    ]
    ;

  shellHook = /* sh */ ''
  HISTFILE=${toString ./.}/.history
  '';
}
