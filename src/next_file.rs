use crate::deck::Deck;
use serde::Deserialize;
use std::error::Error;
use std::process::Command;
use std::sync::Arc;

#[derive(Deserialize)]
pub struct NextFile {
    /// full path to the file to play next
    pub filename: String,

    /// cue point to start. Using the sample rate of the track
    #[serde(default, rename = "start")]
    pub cue_start: Option<usize>,

    /// cue point to start fading out and start the next track.
    /// Using the sample rate of the track
    #[serde(default, rename = "stop")]
    pub cue_stop: Option<usize>,
}

#[derive(Clone)]
pub struct NextDeckFunction {
    program_path: Arc<String>,
}

use mockall::automock;

#[automock]
impl NextDeckFunction {
    pub fn new(program_path: String) -> Self {
        NextDeckFunction {
            program_path: Arc::new(program_path),
        }
    }

    /// run command to determine next track to play
    fn run_command(&self) -> Result<NextFile, Box<dyn Error>> {
        // hand over currently playing track
        // hand over current bpm
        let mut command = Command::new(self.program_path.as_str());

        let output = command.output()?;
        let next_file = serde_json::from_slice(output.stdout.as_slice())?;
        Ok(next_file)
    }

    pub fn next_deck(&self) -> Result<Deck, Box<dyn Error>> {
        let next_file = self.run_command()?;
        let deck = Deck::new(next_file.filename.as_str());
        Ok(deck)
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_next_file() {
        let next_deck_function = NextDeckFunction::new("tests/nextTrack.sh".to_string());
        let result = next_deck_function.run_command();
        assert!(result.is_ok());
        let next_file = result.unwrap();
        assert_eq!(next_file.filename, "./assets/sample1.mp3");
    }
}
