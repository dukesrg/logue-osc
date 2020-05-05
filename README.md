# logue-osc
Custom oscillators for Korg logue-sdk compatible synths

You can get the lastest precompiled versions of the oscillators in [releases](../../releases/) section.

### Project structure

* [lodue-sdk/](logue-sdk/) : My own logue-sdk fork with optimized makefiles and reduced project footprint.
* [inc/fixed_mathq.h](inc/fixed_mathq.h) : Q31 fixed point math functions.
* [inc/osc_apiq.h](inc/osc_apiq.h) : Q31 fixed point oscillator API functions.
* [src/](src/) : Oscillator source files.
* ...osc/ : Oscillator project files.

### Oscillator description
* Supersaw - Saw with unison.
* FastSaw - The same as Supersaw rewritten with Q31 fixed point. Less CPU resource consuming i.e. more unison/polyphony/FX avaiable without sound degradation.

### Oscillator Parameters
|Oscillator|Shape / A knob|Shift-Shape / Alt / B knob|Param 1|Param 2|Param 3|Param 4|Param 5|Param 6|
|-|-|-|-|-|-|-|-|-|
|Supersaw|Unison level|Detune level|Unison range 1...12 pairs|Detune range 1...100 cents|Band limit 0...100%|Attenuate 0...30dB|Route LFO<br>1 - Shape / Unison<br>2 - Shift-Shape / Detune<br>3 - both|Polyphony 1...12 voices|

### Notes
* Supersaw polyphony is only for NTS-1 firmware 1.2.0 with legato switched off. Setting polyphony more than 1 in any other hardware configuration may result to unpredicted behaviour.
* Supersaw polyphony is limited to use for chords or preemptive mode with last note priority due to NTS-1 firmware 1.2.0 non legato NOTE OFF implementation (i.e. only last released note event is passed to the runtime).
* Sound may be degraded when using high level of unison and/or high level of polyphony with another FX due to high CPU processing power requirement, so use parameters wisely for your current creative requiremet.
