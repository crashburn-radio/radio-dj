extern crate libpulse_binding as pulse;
extern crate libpulse_simple_binding as psimple;
extern crate minimp3;

mod decoder;

use crate::decoder::DecoderStatus;
use crate::decoder::Mp3DecoderFacade;
use crate::decoder::RadioDecoder;

use minimp3::{Decoder, Error, Frame};

use std::fs::File;

use psimple::Simple;
use pulse::sample;
use pulse::stream::Direction;

fn main() {
    let spec = sample::Spec {
        format: sample::Format::S16le,
        channels: 2,
        rate: 44100,
    };
    assert!(spec.is_valid());

    let pulse_simple = Simple::new(
        None,                // Use the default server
        "FooApp",            // Our application’s name
        Direction::Playback, // We want a playback stream
        None,                // Use the default device
        "Music",             // Description of our stream
        &spec,               // Our sample format
        None,                // Use default channel map
        None,                // Use default buffering attributes
    )
    .unwrap();

    const BUFFER_SIZE: usize = 512;

    let mut radio_decoder = RadioDecoder::new("assets/sample1.mp3").unwrap();

    loop {
        let mut buffer: [i16; 2 * BUFFER_SIZE] = [0; 2 * BUFFER_SIZE];
        let decoder_statue = radio_decoder.fill_next(&mut buffer);

        let output_data: [u8; 2 * 2 * BUFFER_SIZE] = unsafe { std::mem::transmute(buffer) };
        pulse_simple.write(output_data.as_ref()).unwrap();

        match decoder_statue {
            DecoderStatus::Empty => break,
            DecoderStatus::HasContent => continue,
        }
    }

    pulse_simple.drain().unwrap();
    pulse_simple.flush().unwrap();
}

fn f32_to_i16(f: f32) -> i16 {
    // prefer to clip the input rather than be excessively loud.
    (f.max(-1.0).min(1.0) * i16::max_value() as f32) as i16
}

/// Returns a 24 bit WAV int as an i16. Note that this is a 24 bit integer, not a
/// 32 bit one. 24 bit ints are in the range [−8,388,608, 8,388,607] while i16s
/// are in the range [-32768, 32767]. Note that this function definitely causes
/// precision loss but hopefully this isn't too audiable when actually playing?
fn i24_to_i16(i: i32) -> i16 {
    (i >> 8) as i16
}
