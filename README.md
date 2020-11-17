# logue-osc
Custom oscillators for Korg logue-sdk compatible synths

You can get the latest pre-compiled versions of the oscillators in the [releases](../../releases/) section.
For user-customizable oscillators, an online constructor is available at https://dukesrg.github.io/logue-osc/.

### Project structure

* [lodue-sdk/](logue-sdk/) : My own logue-sdk fork with optimized Makefiles, and reduced project footprint.
* [inc/fixed_mathq.h](inc/fixed_mathq.h) : Additional fixed point math functions.
* [inc/g711_decode.h](inc/g711_decode.h) : μ-law/A-law decoding functions.
* [inc/osc_apiq.h](inc/osc_apiq.h) : Q31 fixed point oscillator API functions.
* [inc/wavebank.h](inc/wavebank.h) : Customizable [WaveEdit](https://synthtech.com/waveedit) compatible wavetable functions.
* [Anthologue.sh](Anthologue.sh) : KORG logue-series program data injector for Anthologue oscillator. You can get sample programs at Korg downloads for [minilogue](https://www.korg.com/us/support/download/product/0/544/) and [monologue](https://www.korg.com/us/support/download/product/0/733/).
* [FM64.sh](FM64.sh) : Yamaha DX7/DX21/DX11-series voice bank SysEx injector for FMxx oscillators. You can find banks at [Synth Zone](http://www.synthzone.com/yamaha.htm).
* [Morpheus.sh](Morpheus.sh) : Wavetable oscillator wave data injector to use with any custom oscillator built with [inc/wavebank.h](inc/wavebank.h) file.
* [PCM2ALaw.sh](PCM2ALaw.sh) : Dumbest ever audio transcoder for 16-bit PCM to A-law convertion.
* [PCM2uLaw.sh](PCM2uLaw.sh) : Same as the above to μ-law convertion.
* [WaveEdit.sh](WaveEdit.sh) : [WaveEdit Online](https://waveeditonline.com/) library batch converter, very slow and CPU consuming.
* [src/](src/) : Oscillator source files.
* &hellip;osc/ : Oscillator project files.

### Oscillator description
* Supersaw - Saw with unison.
* FastSaw - The same as Supersaw, rewritten with Q31 fixed point. Less CPU resource consumption; i.e. more unison/polyphony/FX avaiable without sound degradation.
* Morpheus - Example implementation of custom wavetable inspired by [WaveEdit](https://synthtech.com/waveedit).
* FMxx - 6/4-operator FM oscillator series with up to 4 Yamaha DX7/DX21/DX11-series voice banks suport. Refer to table below for features comparison. Current progress is in issue [FM64 features implementation](../../issues/2).
* Anthologue - 6 VCO oscillator with Korg logue-series program suport. Current progress is in issue [Anthologue features implementation](../../issues/1).

### Oscillator Parameters
|Oscillator|Shape / A knob|Shift-Shape / Alt / B knob|Param 1|Param 2|Param 3|Param 4|Param 5|Param 6|
|-|-|-|-|-|-|-|-|-|
|Supersaw<br>FastSaw|Unison level|Detune level|Unison range 1&hellip;12 pairs|Detune range 1&hellip;100 cents|Band limit 0&hellip;100%|Attenuate 0&hellip;30dB|Route LFO<br>1 - Shape / Unison<br>2 - Shift-Shape / Detune<br>3 - both|Polyphony 1&hellip;12 voices|
|Morpheus|Morph X<br>LFO X rate 0.0&hellip;10.0Hz<br>or wave select|Morph Y<br>LFO Y rate 0.0&hellip;10.0Hz<br>or wave select|Mode<br>1 - Linear X<br>2 - Grid XY|LFO X type|LFO Y type|LFO trigger<br>1 - none<br>2 - LFO X<br>3 - LFO Y<br>4 - both|Morph Interpolate<br>1 - off<br>2 - on|-|
|FMxx|Assignable controller 1|Assignable controller 2|Voice select 1&hellip;32|Bank select 1&hellip;4|Assignable controller 1 select 1&hellip;69/49|Assignable controller 2 select 1&hellip;69/49|Shape LFO target select 1&hellip;2|[Algorithm](https://cdn.korg.com/us/products/upload/5cc6b8eb8a815777c811bbb3fc293c34_pc.jpg) 1&hellip;40|
|Anthologue|Assignable controller 1|Assignable controller 2|Program select 1&hellip;25|Sub timbre select 1&hellip;25|Play mode select<br>1 - note<br>2 - sequence trigger<br>3 - sequence trigger with native BMP|Assignable controller 1 select 1&hellip;79|Assignable controller 2 select 1&hellip;79|-|

### Oscillator notes
* Oscillators are developed and tested on the NTS-1, which can utilize about twice the CPU performance compared to the Prologue and Monologue XD. The latter may experience oscillator sound degradation with some of the FX enabled, or even without the FX. Please don't hesitate to report any issues related to this.
* Supersaw polyphony is only for NTS-1 firmware 1.2.0, with legato switched off. Setting polyphony to more than 1 in any other hardware configuration may result to unpredictable behaviour.
* Supersaw polyphony is limited to use for chords or preemptive mode with last note priority, due to NTS-1 firmware 1.2.0 non legato NOTE OFF implementation (i.e. only last released note event is passed to the runtime).
* With Supersaw, the sound may be degraded when using a high level of unison and/or a high level of polyphony with another FX, due to high CPU processing requirements. Use parameters wisely for your creative requirements.
* Morpheus LFO rate control is in a non-linear scale, with more precise control in lower frequencies.
* FM64 is very rough and only a limited number of features are supported. Currently most voices sound quite different to the originals.
* Using more than 2 FX on NTS-1 with FM64 may produce sound degradation due to high CPU processing power requirement for 6-op FM calculations.
* Using LFO with pitch or shape as a target may produce sound degradation on NTS-1 or multi-engine voice hang on -logues.
* DX21/DX11 voices utilize only operators 6 to 3. Operators 1 and 2 levels are set to silent, but may be altered manually.
* DX21/DX11 voices with algorithm 3 are initialized with different operator order to match DX7 algorithm 8.
* Since there is no way to pass velocity to the oscillator FM64 utilizes assignable controller #01 as a velocity. Velocity will reset on a voice change to the default value. The default value is equivalent to 100 from 0&hellip;127 range though internally 10-bit precision is used.
* Anthologue patch select sets the VCO's parameters according to the selected patch. Further manual parameter edit may available for all supported features, which can exceed the original synth capabilities (e.x. Cross Mod can be activated for Monologue program).
* Any types and combinations of logue-series can be injected in Anthologue.
* Maximum number of Anthologue programs depends on their types and combinations, and can vary from 25 to 76.
* Due to logue-sdk parameter initialization implementation, FM64 and Anthologue oscillators may alter program parameters on selection. Change the program after oscillator selection to make sure all parameters are loaded from the program to their default values.
* With Anthologue, only the NTS-1 can utilize system BPM with play mode 3. All other -logue synths work the same way for both sequence modes: internal oscillator BPM initialized from the program, and can only be changed with assignable controllers.
* All 6 VCOs of Anthologue are identical, and sequentially chained with sync/ring mod/cross mod.
* VCOs 4-6 of Anthologue are considered as a sub timbre. To utilize them, either select a Prologue program with sub timbre, or force sub timbre and set with Sub On AC, Main/Sub Balance AC and Sub parameter.
* Split sub timbre type is available for all models since it utilizes 3 VCOs at a time.
* 6 VCO is only stable on the NTS-1 with up to 2 FX.
* Sub timbre is reset on program change. For Prologue programs - according to program settings. For other -logues programs - Sub On: switched off, Main/Sub balance: center, VCO 4-6 are reset.
* On -logues, Prologue programs with timbre mode other than Split are loaded with sub timbre forcefully disabled, in order to avoid oscillator hang.
* FM64 does not suport waveform select due to performance limitations.
* Both FM64 and FM48 support manual selection from 40 algoritms introduced in Korg opsix, though several algorithm won't produce any sound with FM48.

|#|Morpheus LFO X&Y types|
|-|-|
|0|LFO off (wave select mode)|
|1|Sawtooth (ascending)|
|2|Sawtooth (descending)|
|3|Triangle (ping-pong)|
|4|Sine|
|5&hellip;19|Wave Bank E|
|20&hellip;35|Wave Bank F|
|36&hellip;99|Custom waves|
|100|White noise S&H|

|FMxx features|FM48|FM64|FM66|FM67|FM68|
|-|-|-|-|-|-|
|Operators count|4|6|6|6|6|
|Voice bank type|DX21 / DX11|DX7 / DX21 / DX11|DX7 / DX21 / DX11|DX7 / DX21 / DX11|DX7 / DX21 / DX11|
|Voice bank count|4|4|3|2|2|
|Waveform count|8|1|8|16|8|
|Waveform bit depth|32|32|16|16|32|
|Waveform customization|||+|+|+|
|Shape LFO target supported|+|+|+|+|+|
|Feedback supported|+|+|+|+|+|

|FMxx patch extensions|DX7 voices|DX21 / DX11 voices|
|-|-|-|
|Algorithm 1&hellip;128|upper bits at standard offset 0x6E|4 upper bits at offset 0x2F|
|Waveform 1&hellip;16|4 upper bits at offsets 0x0B, 0x1C, 0x2D, 0x3E, 0x4F, 0x60|1 higher bit at standard offsets 0x4A, 0x4C, 0x4E, 0x50|

|#|FMxx<br>Assignable controllers 1&2, LFO target|1x (Op.6/4)|2x (Op.5/3)|3x (Op.4/2)|4x (Op.3/1)|5x (Op.2/-)|6x (Op.1/-)|
|-|-|-|-|-|-|-|-|
|x0|N/A|Op. level|Op. level|Op. level|Op. level|Op. level|Op. level / -|Op. level / -|
|x1|Velocity|Op. rate scale|Op. rate scale|Op. rate scale|Op. rate scale|Op. rate scale / -|Op. rate scale / -|
|x2|Feedback|- / Waveform|- / Waveform|- / Waveform|- / Waveform|-|-|
|x3|-|-|-|-|-|-|-|
|x4|-|-|-|-|-|-|-|
|x5|-|-|-|-|-|-|-|
|x6|-|-|-|-|-|-|-|
|x7|-|-|-|-|-|-|-|
|x8|-|-|-|-|-|-|-|
|x9|-|-|-|-|-|-|-|

|#|FMxx Shape LFO target|
|-|-|
|1|Amp|
|2|Feedback level|

|#|Anthologue<br>Assignable controllers 1&2|1x<br>(Main VCO 1)|2x<br>(Main VCO 2)|3x<br>(Main VCO 3)|4x<br>(Sub VCO 1)|5x<br>(Sub VCO 2)|6x<br>(Sub VCO 3)|7x<br>(Sub settings)|
|-|-|-|-|-|-|-|-|-|
|x0|N/A|Wave|Wave|Wave|Wave|Wave|Wave|Sub On|
|x1|Slider<br>Mod.Wheel/E.Pedal<br>Joy Y+/Joy Y-|Shape|Shape|Shape|Shape|Shape|Shape|Timbre Type<br>(Layer/XFade/Split)|
|x2|Keyboard Octave|Octave|Octave|Octave|Octave|Octave|Octave|Main/Sub Balance|
|x3|Pitch Bend|Pitch|Pitch|Pitch|Pitch|Pitch|Pitch|Main/Sub Position|
|x4|Program Level|Level|Level|Level|Level|Level|Level|Split Point|
|x5|Bend Range +|-|Sync|Sync|Sync|Sync|Sync|-|
|x6|Bend Range -|-|Ring Mod|Ring Mod|Ring Mod|Ring Mod|Ring Mod|-|
|x7|BPM|-|Cross Mod Depth|Cross Mod Depth|Cross Mod Depth|Cross Mod Depth|Cross Mod Depth|-|
|x8|-|-|-|-|-|-|-|-|
|x9|-|-|-|-|-|-|-|-|

|#|Anthologue<br>Waves|
|-|-|
|1|Square|
|2|Triangle|
|3|Saw|
|4|Noise|

### manifest.json custom data extension

Oscillators with customizable data may contain additional metadata about customization. This data is ignored by synthesizers and Librarian apps, but is useful for external utilities to manage custom data. Here is an example:

```
{
    "header" : 
    {
        ...
        "params" : [
            ["Wave", 0, 63, ""]
          ],
        "custom_data" : [
            ["Wave file", 64, 256, 64, 0]
        ]
    }
}
```

* custom_data (array) : custom data descriptors

Custom data descriptors are themselves arrays, and should contain 3-5 values:

0. description (string) : brief custom data description, may contain format description.
1. offset (int) : custom data offset in oscillator binary file.
2. size (int) : custom data element size.
3. count (int) : optional maximum number of custom data elements (if more than one).
4. param (int) : optional index of parameter that controls custom element selection (to reflect actual number of elements injected).

### Credits
* [Sebo1971](https://github.com/Sebo1971) for comprehensive oscillators testing with prologue and in general
* [Gearslutz - The Korg Logue User Oscillator Programming Thread](https://www.gearslutz.com/board/electronic-music-instruments-and-electronic-music-production/1306032-korg-logue-user-oscillator-programming-thread.html) participants for inspirations
