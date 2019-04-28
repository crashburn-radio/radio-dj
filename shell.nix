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


  runRadio = pkgs.writeShellScriptBin "radio-run" /* sh */ ''
  ${radioPkgs}/bin/RadioDj \
    localhost \
    8000 \
    /radio.ogg \
    palo \
    palo \
    ${nextTrackScript "/home/palo/music-library/techno"}/bin/nextTrack
  '';

  review = let
    moveToDir = key: dir: pkgs.writeText "move-with-${key}.lua" ''
      tmp_dir = "${dir}"

      function move_current_track_${key}()
        track = mp.get_property("path")
        os.execute("mkdir -p '" .. tmp_dir .. "'")
        os.execute("mv '" .. track .. "' '" .. tmp_dir .. "'")
        os.execute("mv '" .. track .. ".rdj' '" .. tmp_dir .. "'")
        print("moved '" .. track .. "' to " .. tmp_dir)
      end

      mp.add_key_binding("${key}", "move_current_track_${key}", move_current_track_${key})
    '';

    delete = moveToDir "D" "./.graveyard";
    good = moveToDir "L" "./.good";

    cue_in_out = pkgs.writeText "cue_in_out.lua" ''
      in_cue = 0
      function save_in_cue()
        in_cue = mp.get_property_number("stream-pos")
      end

      function save_out_cue()
        out_cue = mp.get_property_number("stream-pos")
        track = mp.get_property("path")
        os.execute("echo '{\"cueInApprox\":" .. in_cue .. ",\"cueOutApprox\":" .. out_cue .. "}' | tee " .. track .. ".rdj")
      end

      mp.add_forced_key_binding("i", "save_in_cue", save_in_cue)
      mp.add_forced_key_binding("o", "save_out_cue", save_out_cue)
    '';

  in pkgs.writers.writeDashBin "review" ''
    exec ${pkgs.mpv}/bin/mpv --no-config --script=${delete} --script=${good} --script=${cue_in_out} "$@"
  '';


in

pkgs.mkShell {

  buildInputs =
    libraries
    ++ buildTools
    ++ ide
    ++ [
      review
      radioPkgs
      (nextTrackScript "/home/palo/music-library" )
      runRadio
      (testNextTrackScript "/home/palo/music/029_-_01_-_Busted.mp3")
    ]
    ;

  shellHook = /* sh */ ''
  HISTFILE=${toString ./.}/.history
  '';
}
