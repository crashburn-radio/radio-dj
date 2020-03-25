extern crate libpulse_binding as pulse;
extern crate libpulse_simple_binding as psimple;
extern crate minimp3;

mod deck;
mod decoder;
mod errors;
mod next_file;
mod output;

use crate::deck::Deck;
use crate::next_file::NextFile;
use crate::output::OutputThread;
use std::env;

fn main() {
    // Prints each argument on a separate line
    //    for argument in env::args() {
    //        println!("{}", argument);
    //    }

    let mut x = env::args();
    let command = match x.next() {
        Some(command) => command,
        None => {
            println!("No command given");
            std::process::exit(1);
        }
    };

    let next_file =
    //    NextFile::run_command("/home/palo/dev/radio-dj2/examples/nextTrack.sh").unwrap();
    NextFile::run_command(command.as_str()).unwrap();
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
}
