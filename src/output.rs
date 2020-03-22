mod pulseaudio_output;

pub use crate::output::pulseaudio_output::PulseAudioOutput;
use std::sync::mpsc::{sync_channel, Receiver, SyncSender};
use std::thread;

pub const BUFFER_SIZE: usize = 512;
// how many messages are stored till writing will block
const MESSAGE_CHANNEL_SIZE: usize = 50;

trait OutputBackend {
    fn write(&self, buffer: &[i16; BUFFER_SIZE]);
    fn close(&self);
}

/// output thread that writes to an output.
/// throttled.
pub struct OutputThread {
    // todo : use vec instead of box<array>
    thread_sender: SyncSender<Box<[i16; BUFFER_SIZE]>>,
}

impl OutputThread {
    pub fn new() -> Self {
        let (thread_sender, thread_receiver): (
            SyncSender<Box<[i16; BUFFER_SIZE]>>,
            Receiver<Box<[i16; BUFFER_SIZE]>>,
        ) = sync_channel(MESSAGE_CHANNEL_SIZE);
        thread::Builder::new()
            .name("output_thread".to_string())
            .spawn(move || {
                let backend = PulseAudioOutput::new();
                loop {
                    let buffer = thread_receiver.recv().unwrap();
                    backend.write(buffer.as_ref());
                }
            })
            .unwrap();
        OutputThread { thread_sender }
    }
    pub fn write(&self, buffer: &[i16; BUFFER_SIZE]) {
        // todo : throttle
        self.thread_sender.send(Box::new(*buffer)).unwrap();
    }
    pub fn close(&self) {
        // todo : not necessary really
    }
}
