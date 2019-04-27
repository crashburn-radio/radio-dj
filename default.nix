{ stdenv, fetchurl, cmake, pkgconfig,
libshout, sox, ffmpeg-full, jansson, symlinkJoin,
writeShellScriptBin, aubio,
...}:

let
  version = "0.1.0";

  tmpFile = "/dev/shm/tmpfile.wav";

  radioDjBin =
    stdenv.mkDerivation {
      version = version;
      name = "radio-dj-bin-${version}";

      src = ./.;

      enableParallelBuilding = true;

      nativeBuildInputs = [ cmake pkgconfig ];

      buildInputs = [
        libshout
        sox
        ffmpeg-full
        jansson
      ];

    };

  /* use this tool as first agrument */
  printTrack = writeShellScriptBin "print-track" /* sh */ ''
    FILE=$1

    (rm ${tmpFile} &> /dev/null) || true

    ${sox}/bin/sox -q "$FILE" ${tmpFile} &> /dev/null

    CUE_IN=`
      ${aubio}/bin/aubiotrack \
                    --time-format samples \
                    --input ${tmpFile} | head -n10 | tail -n1`

    CUE_OUT=`
      ${aubio}/bin/aubiotrack \
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

symlinkJoin {
  name = "radio-dj-${version}";
  paths = [
    radioDjBin
    printTrack
  ];
  meta = with stdenv.lib; {
    description = "automatic radio dj tool set";
    homepage = https://git.ingolf-wagner.de/crashburn_radio/radio_dj;
    license = licenses.gpl3;
    maintainers = with maintainers; [ palo ];
    platforms = [ "x86_64-linux" "i686-linux" ];
  };
}
