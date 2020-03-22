extern crate libpulse_binding as pulse;
extern crate libpulse_simple_binding as psimple;
extern crate minimp3;

mod deck;
mod decoder;
mod output;

use crate::deck::Deck;
use crate::output::OutputThread;

fn main() {
    let output_thread = OutputThread::new();
    let mut deck = Deck::new("assets/sample1.mp3");

    loop {
        let buffer = deck.next();
        output_thread.write(buffer);

        if deck.has_stopped() {
            break;
        }
    }

    output_thread.close();
}
