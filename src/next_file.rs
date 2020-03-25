use crate::errors::RadioError;
use serde::Deserialize;
use std::process::Command;

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

impl NextFile {
    /// run command to determine next track to play
    pub fn run_command(program_path: &str) -> Result<Self, RadioError> {
        // hand over currently playing track
        // hand over current bpm
        let mut command = Command::new(program_path);

        let status = command
            .status()
            .expect("command for next track did not generate a status");

        if status.success() {
            command
                .output()
                .map_err(|error| RadioError::IoError(error))
                .and_then(|output| {
                    serde_json::from_slice(output.stdout.as_slice())
                        .map_err(|error| RadioError::JsonError(error))
                })
        } else {
            Err(RadioError::RadioError(
                "Command was not successful".to_string(),
            ))
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_next_file() {
        let result = NextFile::run_command("tests/nextTrack.sh");
        assert!(result.is_ok());
        let next_file = result.unwrap();
        assert_eq!(next_file.filename, "./assets/sample1.mp3");
    }
}
