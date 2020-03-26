extern crate cfg_if;
extern crate libpulse_binding as pulse;
extern crate libpulse_simple_binding as psimple;
extern crate minimp3;
extern crate simple_error;

mod deck;
mod decoder;
mod mixer;
mod next_file;
mod output;

use crate::mixer::Mixer;
use crate::next_file::NextDeckFunction;
use crate::output::OutputThread;
use std::env;
use std::error::Error;
use std::sync::Arc;

#[cfg(not(test))]
fn main() -> Result<(), Box<dyn Error>> {
    let command_line_arguments: Vec<String> = env::args().collect();
    let next_file_command = match command_line_arguments.get(1) {
        Some(command) => command,
        None => {
            println!("No command given");
            std::process::exit(1);
        }
    };

    let next_file_function = NextDeckFunction::new(next_file_command.clone());

    let output_thread = OutputThread::new();
    let mut mixer = Mixer::new(Arc::new(output_thread), Arc::new(next_file_function));
    mixer.start();

    Ok(())
}
