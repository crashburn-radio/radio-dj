extern crate minimp3;

mod mp3_decoder;

pub use crate::decoder::mp3_decoder::Mp3Decoder;

#[derive(Debug, PartialEq)]
pub enum DecoderStatus {
    HasContent,
    Empty,
}

pub type AudioBuffer = Vec<AudioSegment>;

#[derive(Debug, PartialEq)]
pub struct AudioSegment {
    pub(crate) left: i16,
    pub(crate) right: i16,
}

pub trait Decoder {
    /// decode next AudioBuffer
    fn next(&mut self) -> (DecoderStatus, AudioBuffer);
}
