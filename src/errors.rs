#[derive(Debug)]
pub enum RadioError {
    IoError(std::io::Error),
    JsonError(serde_json::Error),
    RadioError(String),
}
