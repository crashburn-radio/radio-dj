{ pkgs ?  import <nixpkgs> {} }:

let

  libraries = [
    pkgs.libshout
    pkgs.sox
    pkgs.ffmpeg-full
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
    script
  ];



  script =
  let
  tmpFile = "/dev/shm/tmpfile.wav";
  in
  pkgs.writeShellScriptBin "track-kick" /* sh */
  ''
  FILE=$1

  echo "cleanup"
  rm ${tmpFile} || true

  echo "decode File"
  ${pkgs.sox}/bin/sox "$FILE" ${tmpFile}

  ${pkgs.aubio}/bin/aubiotrack \
                --time-format samples \
                --input ${tmpFile}

  rm ${tmpFile} || true
  '';

in
pkgs.mkShell {

  buildInputs = libraries ++ buildTools ++ ide;

  shellHook = /* sh */ ''
  HISTFILE=${toString ./.}/.history
  '';
}
