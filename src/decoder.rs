extern crate minimp3;

mod mp3_decoder;

pub use crate::decoder::mp3_decoder::Mp3Decoder;

#[derive(Debug, PartialEq)]
pub enum DecoderStatus {
    HasContent,
    Empty,
}

/// should always be of size BUFFER_SIZE
pub type AudioBuffer = Vec<AudioSegment>;

#[derive(Debug, PartialEq)]
pub struct AudioSegment {
    pub(crate) left: i16,
    pub(crate) right: i16,
}

impl AudioSegment {
    pub fn new(left: i16, right: i16) -> Self {
        AudioSegment {
            left: left,
            right: right,
        }
    }

    /// helper function to easily create AudioBuffers
    pub fn new_audio_buffer(audio_samples: Vec<i16>) -> AudioBuffer {
        assert_eq!(audio_samples.len() % 2, 0);
        let mut result = Vec::with_capacity(audio_samples.len() / 2);
        let mut iter = audio_samples.iter();
        loop {
            match (iter.next(), iter.next()) {
                (Some(left), Some(right)) => result.push(AudioSegment {
                    left: *left,
                    right: *right,
                }),
                _ => break,
            }
        }
        result
    }
}

use mockall::automock;

#[automock]
pub trait Decoder {
    /// decode next AudioBuffer
    fn next(&mut self) -> (DecoderStatus, AudioBuffer);

    fn sample_rate(&self) -> u32;
}
