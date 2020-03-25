extern crate libpulse_binding as pulse;
extern crate libpulse_simple_binding as psimple;
extern crate minimp3;
extern crate simple_error;

mod deck;
mod decoder;
mod next_file;
mod output;

use crate::deck::Deck;
use crate::next_file::NextFile;
use crate::output::OutputThread;
use std::env;
use std::error::Error;

fn main() -> Result<(), Box<dyn Error>> {
    let command_line_arguments: Vec<String> = env::args().collect();
    let next_file_command = match command_line_arguments.get(1) {
        Some(command) => command,
        None => {
            println!("No command given");
            std::process::exit(1);
        }
    };

    let next_file = NextFile::run_command(next_file_command.as_str())?;
    let mut deck = Deck::new(next_file.filename.as_str());

    let output_thread = OutputThread::new();
    loop {
        let buffer = deck.next();
        output_thread.write(buffer);

        if deck.has_stopped() {
            break;
        }
    }

    output_thread.close();
    Ok(())
}
