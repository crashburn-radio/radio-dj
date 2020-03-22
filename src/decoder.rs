extern crate minimp3;

use minimp3::{Decoder as Mp3Decoder, Error, Frame};
use mockall::predicate::*;
use mockall::*;
use std::fs::File;

#[derive(Debug, PartialEq)]
pub enum DecoderStatus {
    HasContent,
    Empty,
}

#[automock]
/// need a facade, to write tests
pub trait DecoderFacade {
    fn next_frame(&mut self) -> Result<Frame, Error>;
}

pub struct Mp3DecoderFacade {
    decoder: Mp3Decoder<File>,
}
impl Mp3DecoderFacade {
    pub fn new(path: &str) -> Self {
        let decoder = Mp3Decoder::new(File::open(path).unwrap());
        Mp3DecoderFacade { decoder }
    }
}
impl DecoderFacade for Mp3DecoderFacade {
    fn next_frame(&mut self) -> Result<Frame, Error> {
        self.decoder.next_frame()
    }
}

pub struct RadioDecoder<R> {
    decoder: Box<R>,
    current_frame: Frame,
    current_frame_offset: usize,
}

impl RadioDecoder<Mp3DecoderFacade> {
    /// create a Radio decoder vor MP3s
    pub fn new(path: &str) -> Result<Self, ()> {
        let mp3_decoder = Mp3DecoderFacade::new(path);
        RadioDecoder::create(Box::new(mp3_decoder))
    }
}

impl<D: DecoderFacade> RadioDecoder<D> {
    pub fn create(mut decoder: Box<D>) -> Result<Self, ()> {
        let current_frame = decoder.next_frame().map_err(|_| ())?;
        Ok(RadioDecoder {
            decoder,
            current_frame,
            current_frame_offset: 0,
        })
    }

    /// fill a buffer with the next part
    /// return false if there is nothing more left
    /// expect the input buffer to be zeroed.
    /// todo : return a Vector instead using an input buffer
    pub fn fill_next(&mut self, buffer: &mut [i16]) -> DecoderStatus {
        let mut buffer_index = 0;
        loop {
            if self.current_frame_offset >= self.current_frame.data.len() {
                if self.next_frame() {
                    continue;
                } else {
                    return DecoderStatus::Empty;
                }
            }

            if buffer_index >= buffer.len() {
                return DecoderStatus::HasContent;
            }

            // todo : handle different sample rates
            if self.current_frame.channels == 1 {
                buffer[buffer_index] = self.current_frame.data[self.current_frame_offset];
                buffer[buffer_index + 1] = self.current_frame.data[self.current_frame_offset];
                buffer_index = buffer_index + 2;
            } else if self.current_frame.channels == 2 {
                buffer[buffer_index] = self.current_frame.data[self.current_frame_offset];
                buffer_index = buffer_index + 1;
            } else {
                if (self.current_frame_offset % self.current_frame.channels) < 2 {
                    buffer[buffer_index] = self.current_frame.data[self.current_frame_offset];
                    buffer_index = buffer_index + 1;
                }
            }

            self.current_frame_offset = self.current_frame_offset + 1;
        }
    }

    /// go to next frame
    fn next_frame(&mut self) -> bool {
        match self.decoder.next_frame() {
            Ok(frame) => {
                self.current_frame = frame;
                self.current_frame_offset = 0;
                true
            }
            _ => false,
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use minimp3::{Error, Frame};

    #[test]
    fn test_channel_stereo() {
        let mut mock = MockDecoderFacade::new();
        mock.expect_next_frame()
            .times(3)
            .returning(|| -> Result<Frame, Error> {
                let frame = Frame {
                    data: vec![1, 2, 3, 4],
                    sample_rate: 44100,
                    channels: 2,
                    layer: 0,
                    bitrate: 0,
                };
                Ok(frame)
            });
        let mut radio_decoder = RadioDecoder::create(Box::new(mock)).unwrap();
        const BUFFER_SIZE: usize = 4;
        let mut buffer: [i16; 2 * BUFFER_SIZE] = [0; 2 * BUFFER_SIZE];
        let decoder_status = radio_decoder.fill_next(&mut buffer);
        assert_eq!(decoder_status, DecoderStatus::HasContent);
        assert_eq!(buffer, [1, 2, 3, 4, 1, 2, 3, 4])
    }

    #[test]
    fn test_channel_mono() {
        let mut mock = MockDecoderFacade::new();
        mock.expect_next_frame()
            .times(2)
            .returning(|| -> Result<Frame, Error> {
                let frame = Frame {
                    data: vec![1, 2, 3, 4],
                    sample_rate: 44100,
                    channels: 1,
                    layer: 0,
                    bitrate: 0,
                };
                Ok(frame)
            });
        let mut radio_decoder = RadioDecoder::create(Box::new(mock)).unwrap();
        const BUFFER_SIZE: usize = 4;
        let mut buffer: [i16; 2 * BUFFER_SIZE] = [0; 2 * BUFFER_SIZE];
        let decoder_status = radio_decoder.fill_next(&mut buffer);
        assert_eq!(decoder_status, DecoderStatus::HasContent);
        assert_eq!(buffer, [1, 1, 2, 2, 3, 3, 4, 4])
    }

    #[test]
    fn test_channel_2p1() {
        let mut mock = MockDecoderFacade::new();
        mock.expect_next_frame()
            .returning(|| -> Result<Frame, Error> {
                let frame = Frame {
                    data: vec![1, 2, 3, 4, 5, 6],
                    sample_rate: 44100,
                    channels: 3,
                    layer: 0,
                    bitrate: 0,
                };
                Ok(frame)
            });
        let mut radio_decoder = RadioDecoder::create(Box::new(mock)).unwrap();
        const BUFFER_SIZE: usize = 4;
        let mut buffer: [i16; 2 * BUFFER_SIZE] = [0; 2 * BUFFER_SIZE];
        let decoder_status = radio_decoder.fill_next(&mut buffer);
        assert_eq!(decoder_status, DecoderStatus::HasContent);
        assert_eq!(buffer, [1, 2, 4, 5, 1, 2, 4, 5])
    }
}
