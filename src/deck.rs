use crate::decoder::{AudioBuffer, AudioSegment, Decoder, DecoderStatus, Mp3Decoder};
use crate::output::SAMPLE_RATE;

/// A Deck that holds a Track and plays it from a Cue point to a Cue Point
pub struct Deck {
    decoder: Box<dyn Decoder>,
    /// cue sample where to start
    cue: usize,
    cue_stopping: usize,
    pub status: DeckStatus,

    current_play_position: usize,
    current_audio_buffer: Option<AudioBuffer>,
}

/// in which state is the deck right now
#[derive(Debug, PartialEq, Copy, Clone)]
pub enum DeckStatus {
    /// Deck is playing
    Playing,
    /// Deck is finished playing
    Stopped,
    /// Deck reached stop cue point
    Stopping,
    /// Deck is waiting to play
    Waiting,
}

impl Deck {
    pub fn new(path: &str) -> Self {
        let decoder = Mp3Decoder::new(path).unwrap();
        Deck::setup(Box::new(decoder))
    }

    fn setup(decoder: Box<dyn Decoder>) -> Deck {
        Deck {
            decoder,
            cue: 0,
            cue_stopping: (10 * SAMPLE_RATE) as usize,
            status: DeckStatus::Waiting,
            current_play_position: 0,
            current_audio_buffer: None,
        }
    }

    pub fn next(&mut self) -> AudioBuffer {
        match &self.status {
            DeckStatus::Waiting => {
                self.seek_to_cue_point();
                self.status = DeckStatus::Playing;
                self.get_next()
            }
            DeckStatus::Playing | DeckStatus::Stopping => self.get_next(),
            DeckStatus::Stopped => vec![],
        }
    }

    fn seek_to_cue_point(&mut self) {
        loop {
            let (decoder_status, audio_buffer) = self.decoder.next();
            if decoder_status == DecoderStatus::Empty {
                return;
            }
            let offset = self.cue - self.current_play_position;
            if offset <= audio_buffer.len() {
                // todo : this part is shit, why do I have to copy everything?
                let mut buffer = Vec::with_capacity(audio_buffer.len());
                let mut count = 0;
                for segment in audio_buffer.iter() {
                    if count >= offset {
                        buffer.push(AudioSegment {
                            left: segment.left,
                            right: segment.right,
                        });
                    } else {
                        count = count + 1;
                    }
                }
                self.current_audio_buffer = Some(buffer);
                self.current_play_position = self.current_play_position + offset;
                return;
            }

            self.current_play_position = self.current_play_position + self.cue;
        }
    }

    /// get next audio buffer, also checks if there was seeking done
    /// and self.current_audio_buffer is set.
    fn get_next(&mut self) -> AudioBuffer {
        if self.current_audio_buffer.is_some() {
            let audio_buffer = self.current_audio_buffer.as_ref().unwrap();

            // todo : this part is shit, why do I have to copy everything?
            let mut buffer = Vec::with_capacity(audio_buffer.len());
            for audio_segment in audio_buffer.iter() {
                buffer.push(AudioSegment {
                    left: audio_segment.left,
                    right: audio_segment.right,
                });
            }
            self.current_audio_buffer = None;
            return buffer;
        }

        // todo : has to handle different sample_rate
        let (decoder_status, audio_buffer) = self.decoder.next();
        match decoder_status {
            DecoderStatus::Empty => self.status = DeckStatus::Stopped,
            DecoderStatus::HasContent => self.status = DeckStatus::Playing,
        }
        self.current_play_position = self.current_play_position + audio_buffer.len();
        if self.status == DeckStatus::Playing
            && self.current_play_position >= self.cue_stopping as usize
        {
            self.status = DeckStatus::Stopping
        }
        audio_buffer
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::decoder::MockDecoder;
    use crate::decoder::AudioSegment;

    #[test]
    fn test_seek() {
        let mut decoder = MockDecoder::new();
        decoder
            .expect_next()
            .returning(|| -> (DecoderStatus, AudioBuffer) {
                (
                    DecoderStatus::HasContent,
                    vec![
                        AudioSegment { left: 1, right: 2 },
                        AudioSegment { left: 3, right: 4 },
                        AudioSegment { left: 5, right: 6 },
                        AudioSegment { left: 7, right: 8 },
                        AudioSegment { left: 9, right: 10 },
                        AudioSegment {
                            left: 11,
                            right: 12,
                        },
                        AudioSegment {
                            left: 13,
                            right: 14,
                        },
                        AudioSegment {
                            left: 15,
                            right: 16,
                        },
                        AudioSegment {
                            left: 17,
                            right: 18,
                        },
                        AudioSegment {
                            left: 19,
                            right: 20,
                        },
                    ],
                )
            });
        let mut deck = Deck::setup(Box::new(decoder));
        assert_eq!(deck.current_play_position, 0);
        assert_eq!(deck.cue, 0);
        deck.cue = 6;

        let mut audio_buffer = deck.next();
        assert_eq!(deck.current_play_position, 6);
        assert_eq!(deck.cue, 6);
        assert_eq!(deck.status, DeckStatus::Playing);
        audio_buffer.truncate(4);
        assert_eq!(
            audio_buffer,
            vec![
                AudioSegment {
                    left: 13,
                    right: 14
                },
                AudioSegment {
                    left: 15,
                    right: 16
                },
                AudioSegment {
                    left: 17,
                    right: 18
                },
                AudioSegment {
                    left: 19,
                    right: 20
                },
            ]
        )
    }
}
