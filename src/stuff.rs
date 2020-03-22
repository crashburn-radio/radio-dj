fn f32_to_i16(f: f32) -> i16 {
    // prefer to clip the input rather than be excessively loud.
    (f.max(-1.0).min(1.0) * i16::max_value() as f32) as i16
}

/// Returns a 24 bit WAV int as an i16. Note that this is a 24 bit integer, not a
/// 32 bit one. 24 bit ints are in the range [−8,388,608, 8,388,607] while i16s
/// are in the range [-32768, 32767]. Note that this function definitely causes
/// precision loss but hopefully this isn't too audiable when actually playing?
fn i24_to_i16(i: i32) -> i16 {
    (i >> 8) as i16
}
