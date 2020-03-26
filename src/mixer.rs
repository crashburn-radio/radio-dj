use crate::deck::DeckStatus;
use crate::decoder::{AudioBuffer, AudioSegment};
use crate::output::BUFFER_SIZE;
use std::collections::VecDeque;
use std::sync::mpsc::{sync_channel, Receiver, SyncSender};
use std::sync::Arc;
use std::thread;

const MESSAGE_CHANNEL_SIZE: usize = 100;

cfg_if::cfg_if! {
if #[cfg(test)] {
const MIX_STEP_PARAMETER: f64 = 1.0 / 4.0;
} else {
use crate::output::SAMPLE_RATE;
const MIX_TIME_IN_SECONDS: f64 = 4.0;
const MIX_STEP_PARAMETER: f64 = 1.0 / (MIX_TIME_IN_SECONDS * SAMPLE_RATE as f64);
}
}

cfg_if::cfg_if! {
if #[cfg(test)] {
use crate::output::MockOutputThread as OutputThread;
use crate::next_file::MockNextDeckFunction as NextDeckFunction;
} else {
use crate::output::OutputThread;
use crate::next_file::NextDeckFunction;
}
}

#[derive(Debug, PartialEq)]
enum MixerMessage {
    /// audio buffer to play
    AudioBuffer(AudioBuffer),

    /// starting to stop
    Stopping,

    /// track stopped, next message should be AudioBuffer of the next track loaded in a new deck
    Stopped,
}

enum MixerStatus {
    /// Playing Deck A
    Play,
    /// Mixing Deck A and B
    Mixing,

    /// Mixing Deck
    MixingWithSilentDeckB,
}

pub struct Mixer {
    output_thread: Arc<OutputThread>,
    status: MixerStatus,
    next_file_function: Arc<NextDeckFunction>,
}

impl Mixer {
    pub fn new(
        output_thread: Arc<OutputThread>,
        next_file_function: Arc<NextDeckFunction>,
    ) -> Self {
        Mixer {
            output_thread,
            status: MixerStatus::Play,
            next_file_function,
        }
    }

    fn start_deck_thread(&self, string: String, deck_sender: SyncSender<MixerMessage>) {
        let next_file_closure = self.next_file_function.clone();
        thread::Builder::new()
            .name(string)
            .spawn(move || {
                // todo handle exceptions
                let mut deck = next_file_closure
                    .next_deck()
                    .expect("couldn't load initial deck");
                let mut status = DeckStatus::Playing;

                loop {
                    let buffer = deck.next();
                    deck_sender.send(MixerMessage::AudioBuffer(buffer)).unwrap();
                    if deck.status != status {
                        status = deck.status;
                        assert_ne!(status, DeckStatus::Waiting);
                        match deck.status {
                            DeckStatus::Playing | DeckStatus::Waiting => {}
                            DeckStatus::Stopping => {
                                deck_sender.send(MixerMessage::Stopping).unwrap();
                            }
                            DeckStatus::Stopped => {
                                deck_sender.send(MixerMessage::Stopped).unwrap();
                                deck = next_file_closure
                                    .next_deck()
                                    .expect("couldn't load next deck");
                            }
                        }
                    }
                }
            })
            .unwrap();
    }

    pub fn start(&mut self) {
        let (deck_a_sender, deck_a_receiver): (SyncSender<MixerMessage>, Receiver<MixerMessage>) =
            sync_channel(MESSAGE_CHANNEL_SIZE);
        self.start_deck_thread("deck_a".to_string(), deck_a_sender);

        let (deck_b_sender, deck_b_receiver): (SyncSender<MixerMessage>, Receiver<MixerMessage>) =
            sync_channel(MESSAGE_CHANNEL_SIZE);
        self.start_deck_thread("deck_b".to_string(), deck_b_sender);

        self.main_loop(deck_a_receiver, deck_b_receiver);
    }

    fn main_loop(
        &mut self,
        deck_a_receiver: Receiver<MixerMessage>,
        deck_b_receiver: Receiver<MixerMessage>,
    ) {
        let mut deck_a = deck_a_receiver;
        let mut deck_b = deck_b_receiver;
        let mut deck_a_volume = 1.0;
        let mut deck_b_volume = 0.0;
        let mut buffer_b_cache: VecDeque<AudioSegment> = VecDeque::with_capacity(BUFFER_SIZE);
        loop {
            match deck_a.recv().unwrap() {
                MixerMessage::Stopping => self.status = MixerStatus::Mixing,
                MixerMessage::Stopped => {
                    std::mem::swap(&mut deck_a, &mut deck_b);
                    std::mem::swap(&mut deck_a_volume, &mut deck_b_volume);
                    self.status = MixerStatus::Play;
                }
                MixerMessage::AudioBuffer(buffer) => {
                    match self.status {
                        MixerStatus::Play => {
                            // todo: handle situation when Stopping is taking longer thant the actual track
                            self.output_thread.write(buffer);
                        }
                        MixerStatus::Mixing | MixerStatus::MixingWithSilentDeckB => {
                            let mut mix_buffer = Vec::with_capacity(buffer.len());
                            for segment in buffer.iter() {
                                deck_a_volume = f64::max(deck_a_volume - MIX_STEP_PARAMETER, 0.0);
                                deck_b_volume = f64::min(deck_b_volume + MIX_STEP_PARAMETER, 1.0);

                                let left_a = (segment.left as f64 * deck_a_volume) as i16;
                                let right_a = (segment.right as f64 * deck_a_volume) as i16;

                                let segment_b =
                                    self.get_segment_b(&mut deck_b, &mut buffer_b_cache);

                                let left_b = (segment_b.left as f64 * deck_b_volume) as i16;
                                let right_b = (segment_b.right as f64 * deck_b_volume) as i16;
                                mix_buffer.push(AudioSegment {
                                    left: left_a + left_b,
                                    right: right_a + right_b,
                                });
                            }
                            self.output_thread.write(mix_buffer);
                            if deck_a_volume == 0.0 || deck_b_volume == 1.0 {
                                loop {
                                    match deck_a.recv().unwrap() {
                                        MixerMessage::Stopped => {
                                            std::mem::swap(&mut deck_a, &mut deck_b);
                                            std::mem::swap(&mut deck_a_volume, &mut deck_b_volume);
                                            self.status = MixerStatus::Play;
                                            break;
                                        }
                                        _ => {}
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    fn get_segment_b(
        &mut self,
        deck_b: &mut Receiver<MixerMessage>,
        buffer_b_cache: &mut VecDeque<AudioSegment>,
    ) -> AudioSegment {
        match self.status {
            MixerStatus::MixingWithSilentDeckB => AudioSegment { left: 0, right: 0 },
            _ => {
                match buffer_b_cache.pop_front() {
                    Some(segment_b) => segment_b,
                    None => {
                        match deck_b.recv().unwrap() {
                            MixerMessage::AudioBuffer(buffer_b) => {
                                for segment in buffer_b.iter() {
                                    buffer_b_cache.push_back(AudioSegment {
                                        left: segment.left,
                                        right: segment.right,
                                    });
                                }
                                // todo : error prawn
                                buffer_b_cache.pop_front().unwrap()
                            }
                            MixerMessage::Stopped => {
                                self.status = MixerStatus::MixingWithSilentDeckB;
                                AudioSegment { left: 0, right: 0 }
                            }
                            _ => unimplemented!(),
                        }
                    }
                }
            }
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    use crate::next_file::MockNextDeckFunction;
    use crate::output::MockOutputThread;

    #[test]
    fn test_main_loop() {
        let output_thread: MockOutputThread = MockOutputThread::default();
        let next_file_function: MockNextDeckFunction = MockNextDeckFunction::default();
        Mixer::new(Arc::new(output_thread), Arc::new(next_file_function));
        assert!(false);
    }
}
