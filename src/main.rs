extern crate libpulse_binding as pulse;
extern crate libpulse_simple_binding as psimple;
extern crate minimp3;

mod decoder;
mod output;

use crate::decoder::DecoderStatus;
use crate::decoder::RadioDecoder;
use crate::output::OutputThread;

fn main() {
    let output_thread = OutputThread::new();
    let mut radio_decoder = RadioDecoder::new("assets/sample1.mp3").unwrap();

    loop {
        let (decoder_statue, buffer) = radio_decoder.next();
        output_thread.write(buffer);

        match decoder_statue {
            DecoderStatus::Empty => break,
            DecoderStatus::HasContent => continue,
        }
    }

    output_thread.close();
}
