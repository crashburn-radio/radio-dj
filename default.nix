{ stdenv, fetchurl, cmake, pkgconfig,
libshout, sox, ffmpeg-full, jansson,
jq, symlinkJoin,
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
  BASENAME="$( echo "$FILE" | xargs basename -s .mp3 | xargs basename -s .ogg )"
  DIRNAME="$( dirname "$FILE" )"
  JSON_FILE="$DIRNAME/$BASENAME.rdj"

  # create tmp file
  ${sox}/bin/sox -q "$FILE" ${tmpFile} &> /dev/null

  if [[ -f "$JSON_FILE" ]]
  then
    cueInApprox=$( cat $JSON_FILE | ${jq}/bin/jq '.cueInApprox' )
    cueOutApprox=$( cat $JSON_FILE | ${jq}/bin/jq '.cueOutApprox' )

    CUE_IN=$(
      ${aubio}/bin/aubiotrack \
                    --time-format samples \
                    --input ${tmpFile} | \
        while read line; do test $line -le $cueInApprox && echo $line; done | \
        tail -n1 )
    CUE_OUT=$(
      ${aubio}/bin/aubiotrack \
                    --time-format samples \
                    --input ${tmpFile} | \
        while read line; do test $line -ge $cueOutApprox && echo $line; done | \
        head -n1 )

  else

    CUE_IN=$(
      ${aubio}/bin/aubiotrack \
                    --time-format samples \
                    --input ${tmpFile} | head -n10 | tail -n1)
    CUE_OUT=$(
      ${aubio}/bin/aubiotrack \
                    --time-format samples \
                    --input ${tmpFile} | tail -n10| head -n1)

  fi

  # delete tmp file
  rm ${tmpFile} || true &> /dev/null

  # output
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
