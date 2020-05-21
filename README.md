# logue-osc
Custom oscillators for Korg logue-sdk compatible synths

You can get the lastest precompiled versions of the oscillators in [releases](../../releases/) section.

### Project structure

* [lodue-sdk/](logue-sdk/) : My own logue-sdk fork with optimized makefiles and reduced project footprint.
* [inc/fixed_mathq.h](inc/fixed_mathq.h) : Q31 fixed point math functions.
* [inc/osc_apiq.h](inc/osc_apiq.h) : Q31 fixed point oscillator API functions.
* [inc/wavebank.h](inc/wavebank.h) : Customizable [WaveEdit](https://synthtech.com/waveedit) compatible wavetable functions.
* [Morpheus.sh](Morpheus.sh) : Wavetable oscillator wave data injector to use with any custom oscillator built with [inc/wavebank.h](inc/wavebank.h) file.
* [PCM2uLaw.sh](PCM2uLaw.sh) : Dumbest ever audio transcoder for 16-bit PCM to μ-law convertion.
* [WaveEdit.sh](WaveEdit.sh) : [WaveEdit Online](https://waveeditonline.com/) library batch converter, very slow and CPU consuming.
* [src/](src/) : Oscillator source files.
* ...osc/ : Oscillator project files.

### Oscillator description
* Supersaw - Saw with unison.
* FastSaw - The same as Supersaw rewritten with Q31 fixed point. Less CPU resource consuming i.e. more unison/polyphony/FX avaiable without sound degradation.
* Morpheus - Example implementation of custom wavetable inspired by [WaveEdit](https://synthtech.com/waveedit).

### Oscillator Parameters
|Oscillator|Shape / A knob|Shift-Shape / Alt / B knob|Param 1|Param 2|Param 3|Param 4|Param 5|Param 6|
|-|-|-|-|-|-|-|-|-|
|Supersaw<br>FastSaw|Unison level|Detune level|Unison range 1...12 pairs|Detune range 1...100 cents|Band limit 0...100%|Attenuate 0...30dB|Route LFO<br>1 - Shape / Unison<br>2 - Shift-Shape / Detune<br>3 - both|Polyphony 1...12 voices|
|Morpheus|Morph X<br>LFO X rate 0.0...10.0Hz<br>or wave select|Morph Y<br>LFO Y rate 0.0...10.0Hz<br>or wave select|Mode<br>1 - Linear X<br>2 - Grid XY|LFO X type|LFO Y type|LFO trigger<br>0 - none<br>1 - LFO X<br>2 - LFO Y<br>3 - both|Morph Interpolate<br>0 - off<br>1 - on|-|

### Notes
* Supersaw polyphony is only for NTS-1 firmware 1.2.0 with legato switched off. Setting polyphony more than 1 in any other hardware configuration may result to unpredicted behaviour.
* Supersaw polyphony is limited to use for chords or preemptive mode with last note priority due to NTS-1 firmware 1.2.0 non legato NOTE OFF implementation (i.e. only last released note event is passed to the runtime).
* Sound may be degraded when using high level of unison and/or high level of polyphony with another FX due to high CPU processing power requirement, so use parameters wisely for your current creative requiremet.
* Morpheus LFO rate control is in non-linear scale with more precise control in lower frequencies.

|#|Morpheus LFO X&Y types|
|-|-|
|0|LFO off (wave select mode)|
|1|Sawtooth (ascending)|
|2|Sawtooth (descending)|
|3|Triangle (ping-pong)|
|4|Sine|
|5...19|Wave Bank E|
|20...35|Wave Bank F|
|36...99|Custom waves|
|100|White noise S&H|
