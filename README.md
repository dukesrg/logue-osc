# logue-osc
Custom oscillators for Korg logue-sdk compatible synths

You can get the lastest precompiled versions of the oscillators in [releases](../../releases/) section.

### Project structure

* [lodue-sdk/](logue-sdk/) : My own logue-sdk fork with optimized makefiles and reduced project footprint.
* [src/](src/) : Oscillator source files.
* ...osc/ : Oscillator project files.

### Oscillator description
* Supersaw - Saw with unison.

### Oscillator Parameters
|Oscillator|Shape / A knob|Alt / B knob|Param 1|Param 2|Param 3|Param 4|Param 5|Param 6|
|-|-|-|-|-|-|-|-|-|
|Supersaw|Unison level|Detune level|Unison range 1...12 pairs|Detune range 1...100 cents|Attenuate 0...30dB|Polyphony 1...8|-|-|

### Notes
* Supersaw polyphony is only for NTS-1 firmware 1.2.0 with legato switched off. Setting polyphony more than 1 in any other hardware configuration may result to unpredicted behaviour.
* Supersaw polyphony is limited to use for chords or preemptive mode with last note priority due to NTS-1 firmware 1.2.0 non legato NOTE OFF implementation (i.e. only last released note event is passed to the runtime).
* Sound may be degraded when using high level of unison and/or high level of polyphony with another FX due to high CPU processing power requirement, so use parameters wisely for your current creative requiremet.
