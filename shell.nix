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


  scripts =
  let

  eyeD3 = "${pkgs.python36Packages.eyeD3}/bin/eyeD3";

  setCuePoints =
    let
      tmpFile = "/dev/shm/tmpfile.wav";
    in
    pkgs.writeShellScriptBin "set-cue-points" /* sh */ ''

    FILE=$1

    echo "cleanup"
    rm ${tmpFile} || true

    echo "decode File"
    ${pkgs.sox}/bin/sox "$FILE" ${tmpFile}

    echo "first cue"
    CUE_IN=`
      ${pkgs.aubio}/bin/aubiotrack \
                    --time-format samples \
                    --input ${tmpFile} | head -n10 | tail -n1`

    echo "last cue"
    CUE_OUT=`
      ${pkgs.aubio}/bin/aubiotrack \
                    --time-format samples \
                    --input ${tmpFile} | tail -n10| head -n1`

    echo "cleanup"
    rm ${tmpFile} || true

    ${eyeD3} \
        --encoding=utf8 \
        --user-text-frame=CueIn:"$CUE_IN" \
        --user-text-frame=CueOut:"$CUE_OUT" \
        "$FILE"

    '';

  printTrack = 
    let
      tmpFile = "/dev/shm/tmpfile.wav";
    in

    pkgs.writeShellScriptBin "print-track" /* sh */ ''
    FILE=$1

    (rm ${tmpFile} &> /dev/null) || true 

    ${pkgs.sox}/bin/sox -q "$FILE" ${tmpFile} &> /dev/null

    CUE_IN=`
      ${pkgs.aubio}/bin/aubiotrack \
                    --time-format samples \
                    --input ${tmpFile} | head -n10 | tail -n1`

    CUE_OUT=`
      ${pkgs.aubio}/bin/aubiotrack \
                    --time-format samples \
                    --input ${tmpFile} | tail -n10| head -n1`

    rm ${tmpFile} || true &> /dev/null

    cat <<EOF
    {
      "filename":"$FILE",
      "cueIn":$CUE_IN,
      "cueOut":$CUE_OUT
    }
    EOF
  '';

  in
    [ setCuePoints printTrack ];

in

pkgs.mkShell {

  buildInputs = libraries ++ buildTools ++ ide ++ [ scripts ];

  shellHook = /* sh */ ''
  HISTFILE=${toString ./.}/.history
  '';
}
