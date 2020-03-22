mod pulseaudio_output;

use crate::decoder::AudioBuffer;
pub use crate::output::pulseaudio_output::PulseAudioOutput;
use std::sync::mpsc::{sync_channel, Receiver, SyncSender};
use std::thread;

pub const BUFFER_SIZE: usize = 512;
// how many messages are stored till writing will block
const MESSAGE_CHANNEL_SIZE: usize = 50;

trait OutputBackend {
    // fn write(&self, buffer: &[i16; BUFFER_SIZE]);
    fn write(&self, buffer: AudioBuffer);
    fn close(&self);
}

/// output thread that writes to an output.
/// throttled.
pub struct OutputThread {
    thread_sender: SyncSender<AudioBuffer>,
}

impl OutputThread {
    pub fn new() -> Self {
        let (thread_sender, thread_receiver): (SyncSender<AudioBuffer>, Receiver<AudioBuffer>) =
            sync_channel(MESSAGE_CHANNEL_SIZE);
        thread::Builder::new()
            .name("output_thread".to_string())
            .spawn(move || {
                let backend = PulseAudioOutput::new();
                loop {
                    let buffer = thread_receiver.recv().unwrap();
                    backend.write(buffer);
                }
            })
            .unwrap();
        OutputThread { thread_sender }
    }

    pub fn write(&self, buffer: AudioBuffer) {
        self.thread_sender.send(buffer).unwrap();
    }
    pub fn close(&self) {
        // todo : send via prio channel
    }
}
