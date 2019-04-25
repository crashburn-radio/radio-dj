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
    script
  ];

  rust = pkgs.symlinkJoin {
    name = "rust";
    paths = [
      pkgs.rustc
        pkgs.rust-cbindgen
        pkgs.rustup
        pkgs.cargo
    ];
  };


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

  buildInputs = libraries ++ buildTools ++ ide ++ [ rust ];

  shellHook = /* sh */ ''
  HISTFILE=${toString ./.}/.history
  rm .rust-runtime || true
  ln -s ${rust} .rust-runtime
  '';
}
