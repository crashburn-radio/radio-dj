extern crate libpulse_binding as pulse;
extern crate libpulse_simple_binding as psimple;

use psimple::Simple;
use pulse::sample;
use pulse::stream::Direction;

fn main() {
    let spec = sample::Spec {
        format: sample::Format::F32le,
        channels: 1,
        rate: 44100,
    };
    assert!(spec.is_valid());

    let pulse_simple = Simple::new(
        None,                // Use the default server
        "FooApp",            // Our application’s name
        Direction::Playback, // We want a playback stream
        None,                // Use the default device
        "Music",             // Description of our stream
        &spec,               // Our sample format
        None,                // Use default channel map
        None,                // Use default buffering attributes
    )
    .unwrap();

    const BUFFER_SIZE: usize = 512;
    const FREQUENCY: f32 = 440.0;
    let mut t: f32 = 0.0;
    let t_inc = 2.0 * std::f32::consts::PI * FREQUENCY / spec.rate as f32;

    for _x in 0..300 {
        let mut buffer: [f32; 2 * BUFFER_SIZE] = [0.0; 2 * BUFFER_SIZE];
        for i in 0..BUFFER_SIZE {
            //buffer[i] =  f32::sin(t ) ;
            //buffer[i + BUFFER_SIZE] = buffer[i];
            buffer[i * 2 + 0] = f32::sin(t);
            buffer[i * 2 + 1] = f32::sin(t);
            t = t + t_inc;
        }
        let data: [u8; 2 * 4 * BUFFER_SIZE] = unsafe { std::mem::transmute(buffer) };
        pulse_simple.write(data.as_ref()).unwrap();
    }

    pulse_simple.drain().unwrap();
    pulse_simple.flush().unwrap();
}
