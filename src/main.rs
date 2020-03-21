extern crate libpulse_binding as pulse;
extern crate libpulse_simple_binding as psimple;

use psimple::Simple;
use pulse::stream::Direction;
use pulse::sample;

fn main() {
    let spec = sample::Spec {
        format: sample::SAMPLE_S16NE,
        channels: 2,
        rate: 44100,
    };
    assert!(spec.is_valid());

    let s = Simple::new(
        None,                // Use the default server
        "FooApp",            // Our application’s name
        Direction::Playback, // We want a playback stream
        None,                // Use the default device
        "Music",             // Description of our stream
        &spec,               // Our sample format
        None,                // Use default channel map
        None                 // Use default buffering attributes
    ).unwrap();

    println!("Hello, world!");
}
