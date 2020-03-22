use psimple::Simple;
use pulse::sample;
use pulse::stream::Direction;

use crate::decoder::AudioBuffer;
use crate::output::{OutputBackend, BUFFER_SIZE};

const APPLICATION_NAME: &str = "Radio Dj";
const SAMPLE_FORMAT: sample::Format = sample::Format::S16le;
const SAMPLE_RATE: u32 = 44100;

pub struct PulseAudioOutput {
    simple: Simple,
}

impl PulseAudioOutput {
    pub fn new() -> Self {
        let spec = sample::Spec {
            format: SAMPLE_FORMAT,
            channels: 2,
            rate: SAMPLE_RATE,
        };
        assert!(spec.is_valid());

        let pulse_simple = Simple::new(
            // Use the default server
            None,
            // Our application’s name
            APPLICATION_NAME,
            // We want a playback stream
            Direction::Playback,
            // Use the default device
            None,
            // Description of our stream
            "Music",
            // Our sample format
            &spec,
            // Use default channel map
            None,
            // Use default buffering attributes
            None,
        )
        .unwrap();

        PulseAudioOutput {
            simple: pulse_simple,
        }
    }
}

impl OutputBackend for PulseAudioOutput {
    // todo : handle audio buffers with different sizes
    fn write(&self, buffer: AudioBuffer) {
        let mut output_data: [i16; 2 * BUFFER_SIZE] = [0; 2 * BUFFER_SIZE];
        assert!(buffer.len() <= BUFFER_SIZE);

        let mut counter = 0;
        for audio_segment in buffer.iter() {
            output_data[counter * 2 + 0] = audio_segment.left;
            output_data[counter * 2 + 1] = audio_segment.right;
            counter = counter + 1;
        }

        let output_data: [u8; 4 * BUFFER_SIZE] = unsafe { std::mem::transmute(output_data) };
        self.simple.write(output_data.as_ref()).unwrap();
    }

    fn close(&self) {
        self.simple.drain().unwrap();
        self.simple.flush().unwrap();
    }
}
