extern crate minimp3;

use minimp3::{Decoder, Error, Frame};
use std::fs::File;

pub enum DecoderStatus {
    HasContent,
    Empty,
}

pub struct RadioDecoder {
    decoder: Decoder<File>,
    current_frame: Frame,
    current_frame_offset: usize,
}

impl RadioDecoder {
    pub fn new(path: &str) -> Result<Self, ()> {
        let mut decoder = Decoder::new(File::open(path).unwrap());
        let current_frame = decoder.next_frame().map_err(|_| ())?;
        Ok(RadioDecoder {
            decoder,
            current_frame,
            current_frame_offset: 0,
        })
    }

    /// fill a buffer with the next part
    /// return false if there is nothing more left
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

            // todo : handle channels != 2
            buffer[buffer_index] = self.current_frame.data[self.current_frame_offset];
            self.current_frame_offset = self.current_frame_offset + 1;
            buffer_index = buffer_index + 1;
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
