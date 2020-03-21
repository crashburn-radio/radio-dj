{ pkgs ?  import <nixpkgs> {} }:
pkgs.mkShell {

  buildInputs = [
    pkgs.pulseaudioFull
    pkgs.pkgconfig
  ];

}
