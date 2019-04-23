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
  ];

in
pkgs.mkShell {

  buildInputs = libraries ++ buildTools ++ ide;

  shellHook = /* sh */ ''
  HISTFILE=${toString ./.}/.history
  '';
}
