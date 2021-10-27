# logue-osc
Custom oscillators for Korg logue-sdk compatible synths

You can get the latest pre-compiled versions of the oscillators in the [releases](../../releases/) section.
For user-customizable oscillators, an online constructor is available at https://dukesrg.github.io/logue-osc/.

### Project structure

* [.vscode/](.vscode/) : Visual Studio Code workspace configuration files. Optional, for additional info refer to [logue-sdk-vscode](https://github.com/dukesrg/logue-sdk-vscode) template documentation.
* [lodue-sdk/](logue-sdk/) : My own logue-sdk fork with optimized Makefiles, and reduced project footprint.
* [inc/arm.h](inc/arm.h) : Additional ARM opcodes.
* [inc/custom_param.h](inc/custom_param.h) : Customizable parameters feature.
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
|Morpheus|Morph X<br>LFO X rate 0.0&hellip;10.0Hz<br>or wave X position|Morph Y<br>LFO Y rate 0.0&hellip;10.0Hz<br>or wave Y position|LFO X mode|LFO Y mode|LFO X wave|LFO Y wave|LFO X depth -100%&hellip;100%|LFO Y depth -100%&hellip;100%|
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
* Since there is no way to pass velocity to the oscillator, FMxx utilizes assignable controller for velocity control with 10-bit precision.
* In case both Shape & Alt are bound to the same parameter, changing Alt value is ignored to avoid ambiguity for saving/restoring assignable parameter.
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
* FMxx algoritm parameter value is numerically the offset to the selected voice algorithm (with saturation). Just to enable both the ability to keep the default voice algorithm on oscillator init and to save/recall altered algorith with the program.
* Morpheus LFO rate control is in a non-linear scale, with more precise control in lower frequencies.

|#|Morpheus LFO X&Y modes|
|-|-|
|1|One shot|
|2|Key trigger|
|3|Random|
|4|Free run|
|5|One shot + Shape LFO|
|6|Key trigger + Shape LFO|
|7|Random + Shape LFO|
|8|Free run + Shape LFO|
|9|Off (LFO Y only, deactivates grid mode)|

|#|Morpheus LFO X&Y waves|
|-|-|
|-75&hellip;-90|Wave Bank F|
|-60&hellip;-74|Wave Bank E|
|-47&hellip;-59|Wave Bank D|
|-33&hellip;-46|Wave Bank C|
|-17&hellip;-32|Wave Bank B|
|-1&hellip;-16|Wave Bank A|
|0|Sawtooth|
|1|Triangle|
|2|Square|
|3|Sine|
|4|White noise S&H|
|5&hellip;68|Custom waves|

|FMxx features|FM48|FM64|FM65|FM66|FM67|FM68|FM69|
|-|-|-|-|-|-|-|-|
|Preset algorithm count|84|84|84|84|84|84|84|
|User algorithm count|-|-|16|-|-|-|-|
|Operators count|4|6|6|6|6|6|6|
|Voice bank type|DX21 / DX11|DX7|DX7|DX7|DX7|DX7|DX7|
|Voice bank count|4|5|4|4|2|2|5|
|Feedback count|1|1|2|1|2|1|1|
|Waveform count|8|1|1|8|16|1|1|
|Waveform customization||||+|+|||
|Waveform pinch|||+|||||
|AMP LUT depth x witch, bits|11x16|11x16|11x16|11x16|11x16|13x16|11x16|
|Mixing quality, bits|32|32|16|32|32|32|32|
|Shape LFO target supported|+|+|+|+|+|+|+|
|Pitch EG supported|+|+|+|+|+|+|+|
|Split zones|3|3|3|3|3|3|3|
|Chromatic mode|+|+|+|+|+|+||
|Kit mode|+|+|+|+|+|+|+|

|FMxx patch extensions|DX7 voices|DX21 / DX11 voices|
|-|-|-|
|Algorithm 1&hellip;128|upper bits at standard offset 0x6E|4 upper bits at offset 0x2F|
|Waveform 1&hellip;16|4 upper bits at offsets 0x0B, 0x1C, 0x2D, 0x3E, 0x4F, 0x60|1 higher bit at standard offsets 0x4A, 0x4C, 0x4E, 0x50|

|#|FMxx<br>Custom params|Range|Description|
|-|-|-|-|
|0|Velocity|0&hellip;127 ⁽¹⁾|Velocity control (maximum is 100/127 when using param and 127/127 with fractional 10-bit resolution when assigned Shape/Alt)|
|1|Voice 1|-96&hellip;96|Set zone 1 voice (single or right), 0 activates kit mode, negative values will wrap to the end of the maximum allowed voices (Assigning to Shape/Alt allows to select up to 128 voices, not existing voices will be initialized with random values and can't be saved/restored as a preset)|
|2|Voice 2|-96&hellip;96|Set zone 2 voice (left or middle), 0 activates kit mode, negative values will wrap to the end of the maximum allowed voices (Assigning to Shape/Alt allows to select up to 128 voices, not existing voices will be initialized with random values and can't be saved/restored as a preset)|
|3|Voice 3|-96&hellip;96|Set zone 3 voice (left), may be out of bounds, 0 activates kit mode, negative values will wrap to the end of the maximum allowed voices (Assigning to Shape/Alt allows to select up to 128 voices, not existing voices will be initialized with random values and can't be saved/restored as a preset)|
|4|Split Point1|1&hellip;101 ⁽²⁾|Split point between zone 1 and 2|
|5|Split Point2|1&hellip;101 ⁽²⁾|Split point between zone 2 and 3|
|6|Transpose 1|-99&hellip;100 ⁽²⁾|Transpose for zone 1|
|7|Transpose 2|-99&hellip;100 ⁽²⁾|Transpose for zone 2|
|8|Transpose 3|-99&hellip;100 ⁽²⁾|Transpose for zone 3|
|9|Voice Shift1|-99&hellip;100|Voice shift for zone 1|
|10|Voice Shift2|-99&hellip;100|Voice shift for zone 2|
|11|Voice Shift3|-99&hellip;100|Voice shift for zone 3|
|12|Shape Assign|-99&hellip;99|Assign param to Shape, default is Velocity (sign controls bipolar parameter value directon, higer 7 bits of the Shape value used as param value)|
|13|Alt Assign|-99&hellip;99|Assign param to Alt, default is FB scale (sign controls bipolar parameter value directon, higer 7 bits of the Alt value used as param value)|
|14|FB offset|-99&hellip;100 ⁽³⁾|Feedback 1 offset|
|15|FB2 offset|-99&hellip;100 ⁽³⁾|Feedback 2 offset|
|16|FB scale|-99&hellip;100 ⁽⁴⁾|Feedback 1 multiplier|
|17|FB2 scale|-99&hellip;100 ⁽⁴⁾|Feedback 2 multiplier|
|18|FB route|0&hellip;66|Feedback 1 route|
|19|FB2 route|0&hellip;66|Feedback 2 route|
|20|Alg select|0&hellip;100|Algorithm select|
|21|Alg offset|-99&hellip;99|Algorithm offset|
|22|Lvl offs All|-99&hellip;99|Level offset for all operators|
|23|Lvl offs Car|-99&hellip;99|Level offset for carriers|
|24|Lvl offs Mod|-99&hellip;99|Level offset for modulators|
|25|Lvl offs Op1|-99&hellip;99|Level offset for operator 1|
|26|Lvl offs Op2|-99&hellip;99|Level offset for operator 2|
|27|Lvl offs Op3|-99&hellip;99|Level offset for operator 3|
|28|Lvl offs Op4|-99&hellip;99|Level offset for operator 4|
|29|Lvl offs Op5|-99&hellip;99|Level offset for operator 5|
|30|Lvl offs Op6|-99&hellip;99|Level offset for operator 6|
|31|Lvl scal All|-99&hellip;100 ⁽⁴⁾|Level multiplier for all operators|
|32|Lvl scal Car|-99&hellip;100 ⁽⁴⁾|Level multiplier for carriers|
|33|Lvl scal Mod|-99&hellip;100 ⁽⁴⁾|Level multiplier for modulators|
|34|Lvl scal Op1|-99&hellip;100 ⁽⁴⁾|Level multiplier for operator 1|
|35|Lvl scal Op2|-99&hellip;100 ⁽⁴⁾|Level multiplier for operator 2|
|36|Lvl scal Op3|-99&hellip;100 ⁽⁴⁾|Level multiplier for operator 3|
|37|Lvl scal Op4|-99&hellip;100 ⁽⁴⁾|Level multiplier for operator 4|
|38|Lvl scal Op5|-99&hellip;100 ⁽⁴⁾|Level multiplier for operator 5|
|39|Lvl scal Op6|-99&hellip;100 ⁽⁴⁾|Level multiplier for operator 6|
|40|KLS offs All|-99&hellip;99|Keyboard level scaling offset for all operators|
|41|KLS offset Car|-99&hellip;99|Keyboard level scaling offset for carriers|
|42|KLS offset Mod|-99&hellip;99|Keyboard level scaling offset for modulators|
|43|KLS offset Op1|-99&hellip;99|Keyboard level scaling offset for operators 1|
|44|KLS offset Op2|-99&hellip;99|Keyboard level scaling offset for operators 2|
|45|KLS offset Op3|-99&hellip;99|Keyboard level scaling offset for operators 3|
|46|KLS offset Op4|-99&hellip;99|Keyboard level scaling offset for operators 4|
|47|KLS offset Op5|-99&hellip;99|Keyboard level scaling offset for operators 5|
|48|KLS offset Op6|-99&hellip;99|Keyboard level scaling offset for operators 6|
|49|KLS scal All|-99&hellip;100 ⁽⁴⁾|Keyboard level scaling multiplier for all operators|
|50|KLS scal Car|-99&hellip;100 ⁽⁴⁾|Keyboard level scaling multiplier carriers|
|51|KLS scal Mod|-99&hellip;100 ⁽⁴⁾|Keyboard level scaling multiplier modulators|
|52|KLS scal Op1|-99&hellip;100 ⁽⁴⁾|Keyboard level scaling multiplierfor operator 1|
|53|KLS scal Op2|-99&hellip;100 ⁽⁴⁾|Keyboard level scaling multiplierfor operator 2|
|54|KLS scal Op3|-99&hellip;100 ⁽⁴⁾|Keyboard level scaling multiplierfor operator 3|
|55|KLS scal Op4|-99&hellip;100 ⁽⁴⁾|Keyboard level scaling multiplierfor operator 4|
|56|KLS scal Op5|-99&hellip;100 ⁽⁴⁾|Keyboard level scaling multiplierfor operator 5|
|57|KLS scal Op6|-99&hellip;100 ⁽⁴⁾|Keyboard level scaling multiplierfor operator 6|
|58|KVS offs All|-99&hellip;100 ⁽³⁾|Key velocity sensitivity offset for all operators|
|59|KVS offs Car|-99&hellip;100 ⁽³⁾|Key velocity sensitivity offset for carriers|
|60|KVS offs Mod|-99&hellip;100 ⁽³⁾|Key velocity sensitivity offset for operators|
|61|KVS offs Op1|-99&hellip;100 ⁽³⁾|Key velocity sensitivity offset operator 1|
|62|KVS offs Op2|-99&hellip;100 ⁽³⁾|Key velocity sensitivity offset operator 2|
|63|KVS offs Op3|-99&hellip;100 ⁽³⁾|Key velocity sensitivity offset operator 3|
|64|KVS offs Op4|-99&hellip;100 ⁽³⁾|Key velocity sensitivity offset operator 4|
|65|KVS offs Op5|-99&hellip;100 ⁽³⁾|Key velocity sensitivity offset operator 5|
|66|KVS offs Op6|-99&hellip;100 ⁽³⁾|Key velocity sensitivity offset operator 6|
|67|KVS scal All|-99&hellip;100 ⁽⁴⁾|Key velocity sensitivity multiplier for all operators|
|68|KVS scal Car|-99&hellip;100 ⁽⁴⁾|Key velocity sensitivity multiplier for carriers|
|69|KVS scal Mod|-99&hellip;100 ⁽⁴⁾|Key velocity sensitivity multiplier for modulators|
|70|KVS scal Op1|-99&hellip;100 ⁽⁴⁾|Key velocity sensitivity multiplier for operator 1|
|71|KVS scal Op2|-99&hellip;100 ⁽⁴⁾|Key velocity sensitivity multiplier for operator 2|
|72|KVS scal Op3|-99&hellip;100 ⁽⁴⁾|Key velocity sensitivity multiplier for operator 3|
|73|KVS scal Op4|-99&hellip;100 ⁽⁴⁾|Key velocity sensitivity multiplier for operator 4|
|74|KVS scal Op5|-99&hellip;100 ⁽⁴⁾|Key velocity sensitivity multiplier for operator 5|
|75|KVS scal Op6|-99&hellip;100 ⁽⁴⁾|Key velocity sensitivity multiplier for operator 6|
|76|Rat offs All|-99&hellip;99|EG rate offset for all operators|
|77|Rat offs Car|-99&hellip;99|EG rate offset for carriers|
|78|Rat offs Mod|-99&hellip;99|EG rate offset for modulators|
|79|Rat offs Op1|-99&hellip;99|EG rate offset for operator 1|
|80|Rat offs Op2|-99&hellip;99|EG rate offset for operator 2|
|81|Rat offs Op3|-99&hellip;99|EG rate offset for operator 3|
|82|Rat offs Op4|-99&hellip;99|EG rate offset for operator 4|
|83|Rat offs Op5|-99&hellip;99|EG rate offset for operator 5|
|84|Rat offs Op6|-99&hellip;99|EG rate offset for operator 6|
|85|Rat scal All|-99&hellip;100 ⁽⁴⁾|EG rate multiplier for all operators|
|86|Rat scal Car|-99&hellip;100 ⁽⁴⁾|EG rate multiplier for carriers|
|87|Rat scal Mod|-99&hellip;100 ⁽⁴⁾|EG rate multiplier for modulators|
|88|Rat scal Op1|-99&hellip;100 ⁽⁴⁾|EG rate multiplier for operator 1|
|89|Rat scal Op2|-99&hellip;100 ⁽⁴⁾|EG rate multiplier for operator 2|
|90|Rat scal Op3|-99&hellip;100 ⁽⁴⁾|EG rate multiplier for operator 3|
|91|Rat scal Op4|-99&hellip;100 ⁽⁴⁾|EG rate multiplier for operator 4|
|92|Rat scal Op5|-99&hellip;100 ⁽⁴⁾|EG rate multiplier for operator 5|
|93|Rat scal Op6|-99&hellip;100 ⁽⁴⁾|EG rate multiplier for operator 6|
|94|KRS offs All|-99&hellip;100 ⁽³⁾|Keyboard EG rate scaling offset for all operators|
|95|KRS offs Car|-99&hellip;100 ⁽³⁾|Keyboard EG rate scaling offset for carriers|
|96|KRS offs Mod|-99&hellip;100 ⁽³⁾|Keyboard EG rate scaling offset for modulators|
|97|KRS offs Op1|-99&hellip;100 ⁽³⁾|Keyboard EG rate scaling offset for operator 1|
|98|KRS offs Op2|-99&hellip;100 ⁽³⁾|Keyboard EG rate scaling offset for operator 2|
|99|KRS offs Op3|-99&hellip;100 ⁽³⁾|Keyboard EG rate scaling offset for operator 3|
|100|KRS offs Op4|-99&hellip;100 ⁽³⁾|Keyboard EG rate scaling offset for operator 4|
|101|KRS offs Op5|-99&hellip;100 ⁽³⁾|Keyboard EG rate scaling offset for operator 5|
|102|KRS offs Op6|-99&hellip;100 ⁽³⁾|Keyboard EG rate scaling offset for operator 6|
|103|KRS scal All|-99&hellip;100 ⁽⁴⁾|Keyboard EG rate multiplier for all operators|
|104|KRS scal Car|-99&hellip;100 ⁽⁴⁾|Keyboard EG rate multiplier for carriers|
|105|KRS scal Mod|-99&hellip;100 ⁽⁴⁾|Keyboard EG rate multiplier for modulators|
|106|KRS scal Op1|-99&hellip;100 ⁽⁴⁾|Keyboard EG rate multiplier for operator 1|
|107|KRS scal Op2|-99&hellip;100 ⁽⁴⁾|Keyboard EG rate multiplier for operator 2|
|108|KRS scal Op3|-99&hellip;100 ⁽⁴⁾|Keyboard EG rate multiplier for operator 3|
|109|KRS scal Op4|-99&hellip;100 ⁽⁴⁾|Keyboard EG rate multiplier for operator 4|
|110|KRS scal Op5|-99&hellip;100 ⁽⁴⁾|Keyboard EG rate multiplier for operator 5|
|111|KRS scal Op6|-99&hellip;100 ⁽⁴⁾|Keyboard EG rate multiplier for operator 6|
|112|Det offs All|-99&hellip;100 ⁽⁷⁾|Detune offset in cents for all operators|
|113|Det offs Car|-99&hellip;100 ⁽⁷⁾|Detune offset in cents for carriers|
|114|Det offs Mod|-99&hellip;100 ⁽⁷⁾|Detune offset in cents for modulators|
|115|Det offs Op1|-99&hellip;100 ⁽⁷⁾|Detune offset in cents for operator 1|
|116|Det offs Op2|-99&hellip;100 ⁽⁷⁾|Detune offset in cents for operator 2|
|117|Det offs Op3|-99&hellip;100 ⁽⁷⁾|Detune offset in cents for operator 3|
|118|Det offs Op4|-99&hellip;100 ⁽⁷⁾|Detune offset in cents for operator 4|
|119|Det offs Op5|-99&hellip;100 ⁽⁷⁾|Detune offset in cents for operator 5|
|120|Det offs Op6|-99&hellip;100 ⁽⁷⁾|Detune offset in cents for operator 6|
|121|Det scal All|-99&hellip;100 ⁽⁴⁾|Detune multiplier for all operators|
|122|Det scal Car|-99&hellip;100 ⁽⁴⁾|Detune multiplier for carriers|
|123|Det scal Mod|-99&hellip;100 ⁽⁴⁾|Detune multiplier for modulators|
|124|Det scal Op1|-99&hellip;100 ⁽⁴⁾|Detune multiplier for operator 1|
|125|Det scal Op2|-99&hellip;100 ⁽⁴⁾|Detune multiplier for operator 2|
|126|Det scal Op3|-99&hellip;100 ⁽⁴⁾|Detune multiplier for operator 3|
|127|Det scal Op4|-99&hellip;100 ⁽⁴⁾|Detune multiplier for operator 4|
|128|Det scal Op5|-99&hellip;100 ⁽⁴⁾|Detune multiplier for operator 5|
|129|Det scal Op6|-99&hellip;100 ⁽⁴⁾|Detune multiplier for operator 6|
|130|Waveform C+M|-77&hellip;77 ⁽⁸⁾|Waveform offset for carriers and modulators|
|131|Waveform 1+2|-77&hellip;77 ⁽⁸⁾|Waveform offset for operators 1 and 2|
|132|Waveform 3+4|-77&hellip;77 ⁽⁸⁾|Waveform offset for operators 3 and 4|
|133|Waveform 5+6|-77&hellip;77 ⁽⁸⁾|Waveform offset for operators 5 and 6|
|134|Waveform All|-90&hellip;35|Waveform select for all operators|
|135|Waveform Car|-90&hellip;35|Waveform select for carriers|
|136|Waveform Mod|-90&hellip;35|Waveform select for modulators|
|137|Waveform Op1|-90&hellip;35|Waveform select for operator 1|
|138|Waveform Op2|-90&hellip;35|Waveform select for operator 2|
|139|Waveform Op3|-90&hellip;35|Waveform select for operator 3|
|140|Waveform Op4|-90&hellip;35|Waveform select for operator 4|
|141|Waveform Op5|-90&hellip;35|Waveform select for operator 5|
|142|Waveform Op6|-90&hellip;35|Waveform select for operator 6|
|143|WF pinch All|0&hellip;99|Waveform pinch for all operators|
|144|WF pinch Car|0&hellip;99|Waveform pinch for carriers|
|145|WF pinch Mod|0&hellip;99|Waveform pinch for modulatorss|
|146|WF pinch Op1|0&hellip;99|Waveform pinch for operator 1|
|147|WF pinch Op2|0&hellip;99|Waveform pinch for operator 2|
|148|WF pinch Op3|0&hellip;99|Waveform pinch for operator 3|
|149|WF pinch Op4|0&hellip;99|Waveform pinch for operator 4|
|150|WF pinch Op5|0&hellip;99|Waveform pinch for operator 5|
|151|WF pinch Op6|0&hellip;99|Waveform pinch for operator 6|

⁽¹⁾ : 0...100 with the step of 1 when assigned to the oscillator parameter, <br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;0..127 with the step of 0.125 when assigned to the Shape or Alt (Shift + Shape) <br/>
⁽²⁾ : semitones / notes <br/>
⁽³⁾ : -6.93…+7 with the step of 0.07 <br/>
⁽⁴⁾ : x0.01…x2 multiplier with the step of 0.01<br/>
⁽⁵⁾ : 0 - keep voice feedback route <br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;higher digit - feedback source operator, 1…6 (0→1, 7...9→6) <br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;lower digit - feedback destination operator, 1…6 (0→1, 7...9→6) <br/>
⁽⁶⁾ : 0 - keep voice algorithm <br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;1...85 - set algorithm explicitly <br/>
⁽⁷⁾ : cents <br/>
⁽⁸⁾ : higher digit - carriers and odd operators, <br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;lower digit - modulators and even operators <br/>


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
            ["Wave file", 64, 256, 64, 0],
            ["custom_params", 16448, 2, 8, 256]
        ],
        "custom_params" : [
            ["Wave", 0, 63, ""],
            ["LFO", 0, 99, ""]
        ]
    }
}
```

* custom_data (array) : custom data descriptors

Custom data descriptors are themselves arrays, and should contain 3-5 values:

0. description (string) : brief custom data description, may contain format description ("custom_params" value is a special case with the customizable parameter numbers lookup table).
1. offset (int) : custom data offset in oscillator binary file.
2. size (int) : custom data element size.
3. count (int) : optional maximum number of custom data elements (if more than one).
4. param (int) : optional index of parameter that controls custom element selection (to reflect actual number of elements injected), for the "custom_params" case this is an offset for the customizable parameter numbers.
5. param multiplier (int) : optional scaler for parameter value.

* custom_params (array) : contains ordered list of all supported parameter descriptors

### Credits
* [Sebo1971](https://github.com/Sebo1971) for comprehensive oscillators testing with prologue and in general
* [aminixduser](https://github.com/aminixduser) for optimization ideas
* [Gearspace - The Korg Logue User Oscillator Programming Thread](https://gearspace.com/board/electronic-music-instruments-and-electronic-music-production/1306032-korg-logue-user-oscillator-programming-thread.html) participants for inspirations
