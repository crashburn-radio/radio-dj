{ stdenv, fetchurl, writeScript, writeShellScriptBin, writeText,
cmake, pkgconfig, libshout, sox, ffmpeg-full, mpv,
jansson, jq, symlinkJoin, aubio, pup, youtube-dl, curl,
# wav file stored for track analysis
tmpFile ? "/dev/shm/tmpfile.wav",
...}:

let

  version = "0.1.4";

  /* The radio dj software */
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

  /* download from bandcamp */
  bandCampScript = writeShellScriptBin "download-bandcamp" /* sh */ ''
    URL="$1"
    foldername="$( basename "$URL" )"
    mkdir -p "$foldername"
    cd $foldername
    ${curl}/bin/curl $URL \
      | ${pup}/bin/pup '#license json{}' \
      | ${jq}/bin/jq ' .[]
        | .children | [ .[] | .href ]
        | {
            "license" :  (if .[0] then .[0] else "most likely unfree" end)
          , "url": "'$URL'"
          }' \
      > metadata.json
    ${youtube-dl}/bin/youtube-dl \
      --extract-audio \
      --audio-quality 0 \
      --audio-format mp3 \
      $URL
  '';

  /* use this tool as first argument */
  printTrack = writeShellScriptBin "print-track" /* sh */ ''
  FILE="$1"
  JSON_FILE="$FILE.rdj"

  # create tmp file
  ${sox}/bin/sox -q "$FILE" ${tmpFile} &> /dev/null

  if [[ -f "$JSON_FILE" ]]
  then
    cueInApprox=$( cat "$JSON_FILE" | ${jq}/bin/jq '.cueInApprox' )
    cueOutApprox=$( cat "$JSON_FILE" | ${jq}/bin/jq '.cueOutApprox' )

    CUE_IN=$(
      ${aubio}/bin/aubiotrack \
                    --time-format samples \
                    --input ${tmpFile} | \
        while read line; do test $line -ge $cueInApprox && echo $line; done | \
        head -n1 )
    CUE_OUT=$(
      ${aubio}/bin/aubiotrack \
                    --time-format samples \
                    --input ${tmpFile} | \
        while read line; do test $line -le $cueOutApprox && echo $line; done | \
        tail -n1 )

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

  /* A script to create approximations of cue points */
  review = let

    mergeMetaData = writeScript "move-track" /* sh */ ''
      FILE="$1"
      TARGET="$2"
      JSON_FILE="$FILE.rdj"
      METADATA_FILE="$( dirname "$FILE" )/metadata.json"
      TARGET_JSON_FILE="$TARGET/$( basename "$FILE" ).rdj"

      # create target directory
      mkdir -p "$TARGET"

      # create metadata json file
      if [[ -f "$JSON_FILE" ]] && [[ -f "$METADATA_FILE" ]]
      then
        jq -s '.[0] * .[1]' "$JSON_FILE" "$METADATA_FILE" > "$TARGET_JSON_FILE"
        rm "$JSON_FILE"
      else
        if [[ -f "$JSON_FILE" ]]
        then
          mv "$JSON_FILE" "$TARGET_JSON_FILE"
        fi
        if [[ -f "$METADATA_FILE" ]]
        then
          cp "$METADATA_FILE" "$TARGET_JSON_FILE"
        fi
      fi

      # move track
      mv "$FILE" "$TARGET/$( basename "$FILE" )"

    '';

    moveToDir = key: dir: writeText "move-with-${key}.lua" /* lua */ ''
      tmp_dir = "${dir}"

      function move_current_track_${key}()
        track = mp.get_property("path")
        os.execute("${mergeMetaData} '" .. track .. "' '" .. tmp_dir .. "'")
        print("moved '" .. track .. "' to " .. tmp_dir)
      end

      mp.add_key_binding("${key}", "move_current_track_${key}", move_current_track_${key})
    '';

    playlist1= moveToDir "1" "./.playlist1";
    playlist2= moveToDir "2" "./.playlist2";
    playlist3= moveToDir "3" "./.playlist3";
    playlist4= moveToDir "4" "./.playlist4";
    delete1 = moveToDir "d" "./.graveyard";
    delete2 = moveToDir "D" "./.graveyard";
    like1 = moveToDir "l" "./.good";
    like2 = moveToDir "L" "./.good";

    cue_in_out = writeText "cue_in_out.lua" /* lua */ ''
      in_cue = 0
      function save_in_cue()
        in_cue = mp.get_property_number("stream-pos")
      end

      function save_out_cue()
        out_cue = mp.get_property_number("stream-pos")
        track = mp.get_property("path")
        os.execute("echo '{\"cueInApprox\":" .. in_cue .. ",\"cueOutApprox\":" .. out_cue .. "}' | tee '" .. track .. ".rdj'")
      end

      mp.add_forced_key_binding("i", "save_in_cue", save_in_cue)
      mp.add_forced_key_binding("o", "save_out_cue", save_out_cue)
    '';

  in writeShellScriptBin "review-tracks" /* sh */ ''
  exec ${mpv}/bin/mpv \
       --no-config \
       --script=${delete1} \
       --script=${delete2} \
       --script=${like1} \
       --script=${like2} \
       --script=${playlist1} \
       --script=${playlist2} \
       --script=${playlist3} \
       --script=${playlist4} \
       --script=${cue_in_out} "$@"
  '';

in

symlinkJoin {
  name = "radio-dj-${version}";
  paths = [
    radioDjBin
    printTrack
    review
    bandCampScript
  ];
  meta = with stdenv.lib; {
    description = "automatic radio dj tool set";
    homepage = https://github.com/crashburn-radio;
    license = licenses.gpl3;
    maintainers = with maintainers; [ mrVanDalo lassulus ];
    platforms = [ "x86_64-linux" "i686-linux" ];
  };
}
