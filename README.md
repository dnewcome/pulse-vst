# PULSE Drum Synth

A 4-part multitimbral hybrid drum synthesizer plugin built with JUCE. Each part combines two sample slots, a synth oscillator, and a noise generator, all shaped by four assignable ADSR envelopes and responsive to MIDI aftertouch.

## Features

- **4 independent drum parts** (Kick, Snare, Hat, Perc) with per-part accent colors
- **Hybrid sound sources per part:**
  - 2 sample slots with drag-and-drop or click-to-browse loading, pitch shifting, and start offset
  - Synth oscillator with sine, triangle, saw, and square waveforms (polyBLEP anti-aliased)
  - Noise generator with white, pink (Paul Kellet), and metallic (Karplus-Strong) modes
- **4 assignable ADSR envelopes per part** — freely route any envelope to any source
- **Interactive envelope editor** with draggable control points
- **MIDI aftertouch** — channel pressure controls dampening and partial retrigger
- **MIDI routing:**
  - Multi-channel mode: parts on channels 1-4
  - GM drum map: channel 10 (note 36=kick, 38=snare, 42=hat, 46=perc)
- **~120 automatable parameters** (30 per part)
- **GUI trigger pads** for mouse-based triggering
- **State save/restore** including sample file paths
- **Formats:** VST3, Standalone

## Building

Requires CMake 3.22+ and a C++17 compiler.

```bash
# Clone JUCE into the project directory if not already present
# git clone https://github.com/juce-framework/JUCE.git

cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
```

Artifacts are placed in `build/PulseDrumSynth_artefacts/`:
- `Release/Standalone/PULSE Drum Synth` — standalone application
- `Release/VST3/PULSE Drum Synth.vst3` — VST3 plugin (also copied to `~/.vst3/`)

## Project Structure

```
Source/
  PluginProcessor.h/.cpp     Main audio processor, owns DrumEngine + APVTS
  PluginEditor.h/.cpp         Top-level UI: header, 4 voice strips, footer

  DSP/
    DrumEngine.h/.cpp          Top-level: 4 DrumVoices, MIDI routing, solo/mute
    DrumVoice.h/.cpp           Single drum part: wires sources + envelopes + mix
    SamplePlayer.h/.cpp        Thread-safe sample loading, pitched playback
    SynthOscillator.h/.cpp     4-waveform oscillator with polyBLEP
    NoiseGenerator.h/.cpp      White/pink/metallic noise with filtering
    Envelope.h/.cpp            Linear ADSR with retrigger support
    EnvelopeBank.h/.cpp        Manages 4 envelopes + output buffers per voice
    AftertouchProcessor.h/.cpp Dampening + threshold-crossing retrigger

  GUI/
    VoiceStripComponent.h/.cpp Vertical strip composing all controls for one part
    KnobComponent.h/.cpp       Custom rotary knob with arc painting
    EnvelopeEditorComponent.h/.cpp  Draggable ADSR editor with gradient fill
    SampleSlotComponent.h/.cpp Click-to-browse + drag-and-drop sample loading
    WaveformSelector.h/.cpp    Toggle button row for waveform/noise type
    EnvAssignComponent.h/.cpp  Envelope-to-source routing buttons
    TriggerPadComponent.h/.cpp Clickable pad with glow animation
    HeaderComponent.h/.cpp     Title bar with status LED

  Utility/
    Parameters.h/.cpp          ~120 parameter definitions via APVTS
    Constants.h                Shared constants + UI color palette
```

## Signal Flow

```
MIDI In -> DrumEngine (routes by channel or note number)
             |
             v
         DrumVoice (per part)
           |-- SamplePlayer 1 --\
           |-- SamplePlayer 2 ---|-- each multiplied by assigned envelope
           |-- SynthOscillator --|
           |-- NoiseGenerator  --/
           |
           v
         Mix sources -> Aftertouch dampening -> Volume + Pan -> Stereo out
```

## Parameter IDs

Parameters follow the pattern `p{part}_{suffix}` (e.g., `p1_syn_lvl` for part 1 synth level). Envelope parameters use `p{part}_env{env}_{adsr}` (e.g., `p2_env3_d` for part 2 envelope 3 decay). See `Source/Utility/Parameters.h` for the full list.
