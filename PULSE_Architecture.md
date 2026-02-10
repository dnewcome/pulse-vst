# PULSE Drum Synth — JUCE Architecture Guide

## Overview

PULSE is a 4-part multitimbral hybrid drum synthesizer VST3/AU plugin built with JUCE. Each part combines **2 sample slots**, a **synth oscillator**, and a **noise generator**, shaped by **4 assignable envelopes** and responsive to **MIDI aftertouch** for expressive dampening and partial retrigger.

---

## Project Structure

```
PulseDrumSynth/
├── CMakeLists.txt
├── Source/
│   ├── PluginProcessor.h / .cpp          # Main audio processor
│   ├── PluginEditor.h / .cpp             # Main UI editor
│   │
│   ├── DSP/
│   │   ├── DrumVoice.h / .cpp            # Single drum part (all sources + mixing)
│   │   ├── SamplePlayer.h / .cpp         # Sample playback engine
│   │   ├── SynthOscillator.h / .cpp      # Wavetable synth osc
│   │   ├── NoiseGenerator.h / .cpp       # White/pink/metallic noise
│   │   ├── Envelope.h / .cpp             # ADSR envelope with curve control
│   │   ├── EnvelopeBank.h / .cpp         # 4-envelope manager per voice
│   │   ├── AftertouchProcessor.h / .cpp  # Aftertouch → dampen/retrigger
│   │   └── DrumEngine.h / .cpp           # Top-level: manages 4 DrumVoices
│   │
│   ├── GUI/
│   │   ├── VoiceStripComponent.h / .cpp  # Vertical strip for one drum part
│   │   ├── TriggerPadComponent.h / .cpp  # Clickable trigger pad
│   │   ├── KnobComponent.h / .cpp        # Custom rotary knob
│   │   ├── EnvelopeEditorComponent.h / .cpp  # Draggable ADSR editor
│   │   ├── SampleSlotComponent.h / .cpp  # Drag-and-drop sample slot
│   │   ├── WaveformSelector.h / .cpp     # Waveform type buttons
│   │   ├── EnvAssignComponent.h / .cpp   # Envelope routing matrix
│   │   └── HeaderComponent.h / .cpp      # Plugin header bar
│   │
│   └── Utility/
│       ├── Parameters.h / .cpp           # All parameter IDs and creation
│       └── Constants.h                   # Shared constants
│
└── Resources/
    └── (factory samples optional)
```

---

## Core DSP Architecture

### DrumEngine (top-level)

```cpp
class DrumEngine {
public:
    static constexpr int NUM_PARTS = 4;

    void prepare(double sampleRate, int blockSize);
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi);

private:
    std::array<DrumVoice, NUM_PARTS> voices;

    // MIDI routing: each part listens on its own MIDI channel (1-4)
    // or all parts respond to channel 10 with note-number mapping
    void handleMidiEvent(const juce::MidiMessage& msg);
    void handleNoteOn(int channel, int note, float velocity);
    void handleNoteOff(int channel, int note);
    void handleAftertouch(int channel, int pressure);
};
```

### DrumVoice (per-part)

```cpp
class DrumVoice {
public:
    void prepare(double sampleRate, int blockSize);
    void trigger(float velocity);
    void release();
    void applyAftertouch(float pressure); // 0.0–1.0

    void processBlock(juce::AudioBuffer<float>& output, int numSamples);

private:
    // Sound sources
    SamplePlayer sample1, sample2;
    SynthOscillator synth;
    NoiseGenerator noise;

    // 4 envelopes, freely assignable to any source
    EnvelopeBank envelopes; // contains 4 Envelope instances

    // Envelope routing: which envelope controls which source's amplitude
    // e.g. envAssign[0] = 0 means sample1 uses envelope 0
    //      envAssign[1] = 0 means sample2 also uses envelope 0
    //      envAssign[2] = 1 means synth uses envelope 1
    //      envAssign[3] = 2 means noise uses envelope 2
    std::array<int, 4> envAssign = {0, 0, 1, 2};

    // Per-source levels
    float sample1Level, sample2Level, synthLevel, noiseLevel;

    // Aftertouch state
    float currentAftertouch = 0.0f;
    float dampenAmount = 0.7f;    // how much AT dampens (parameter)
    float retriggerAmount = 0.3f;  // how much AT retriggers (parameter)

    // Mix
    float volume = 1.0f;
    float pan = 0.0f;

    // Internal mixing buffer
    juce::AudioBuffer<float> tempBuffer;
};
```

### SamplePlayer

```cpp
class SamplePlayer {
public:
    void loadSample(const juce::File& file);
    void loadSample(const void* data, size_t size); // from memory/resource

    void trigger(float velocity);
    void stop();

    // Renders into buffer, returns true if still playing
    bool processBlock(float* output, int numSamples);

    // Parameters
    float level = 1.0f;
    float tune = 0.0f;     // semitones, -24 to +24
    float startPos = 0.0f; // 0.0–1.0 normalized start position

private:
    juce::AudioBuffer<float> sampleData;
    double sampleRate = 44100.0;
    double playbackRate = 1.0;
    double position = 0.0;
    bool playing = false;

    // Interpolation (linear or cubic)
    float interpolate(double pos) const;
};
```

### SynthOscillator

```cpp
class SynthOscillator {
public:
    enum class Waveform { Sine, Triangle, Saw, Square };

    void prepare(double sampleRate);
    void trigger(float velocity, float frequency);
    void processBlock(float* output, int numSamples);

    Waveform waveform = Waveform::Sine;
    float tune = 0.0f;       // semitones offset
    float pulseWidth = 0.5f; // for square wave
    float level = 1.0f;

private:
    double phase = 0.0;
    double phaseIncrement = 0.0;
    double currentSampleRate = 44100.0;

    // Band-limited using polyBLEP for saw/square
    float polyBLEP(double t, double dt) const;
};
```

### NoiseGenerator

```cpp
class NoiseGenerator {
public:
    enum class Type { White, Pink, Metallic };

    void prepare(double sampleRate);
    void processBlock(float* output, int numSamples);

    Type type = Type::White;
    float level = 1.0f;
    float filterCutoff = 1.0f; // normalized 0–1
    float tone = 0.5f;         // spectral shaping

private:
    juce::Random random;

    // Pink noise state (Voss-McCartney or Paul Kellet's)
    float pinkState[7] = {};

    // Metallic noise (short delay + feedback for inharmonic content)
    std::array<float, 256> metalBuffer = {};
    int metalWritePos = 0;
    float metalFeedback = 0.95f;

    // Simple one-pole filter for tone
    float filterState = 0.0f;
};
```

### Envelope

```cpp
class Envelope {
public:
    enum class Stage { Idle, Attack, Decay, Sustain, Release };

    struct Parameters {
        float attack = 0.01f;   // seconds
        float decay = 0.3f;     // seconds
        float sustain = 0.5f;   // level 0–1
        float release = 0.4f;   // seconds
        float attackCurve = 0.0f;  // -1 log, 0 linear, +1 exp
        float decayCurve = 0.0f;
        float releaseCurve = 0.0f;
    };

    void prepare(double sampleRate);
    void trigger();
    void release();
    void forceRelease(float time); // for aftertouch dampening

    // Process a block, writing envelope values into output
    void processBlock(float* output, int numSamples);

    float getCurrentLevel() const { return currentLevel; }
    Stage getStage() const { return stage; }
    bool isActive() const { return stage != Stage::Idle; }

    Parameters params;

private:
    Stage stage = Stage::Idle;
    float currentLevel = 0.0f;
    double sampleRate = 44100.0;
    int samplesInStage = 0;
    int stageLengthSamples = 0;
};
```

### EnvelopeBank

```cpp
class EnvelopeBank {
public:
    static constexpr int NUM_ENVELOPES = 4;

    void prepare(double sampleRate);
    void triggerAll();
    void releaseAll();

    Envelope& getEnvelope(int index);

    // Process all envelopes for a block, storing results
    void processBlock(int numSamples);

    // Get the envelope output buffer for a specific envelope
    const float* getEnvelopeOutput(int index) const;

private:
    std::array<Envelope, NUM_ENVELOPES> envelopes;
    std::array<std::vector<float>, NUM_ENVELOPES> outputBuffers;
};
```

### AftertouchProcessor

```cpp
class AftertouchProcessor {
public:
    struct Config {
        float dampenSensitivity = 0.7f;  // 0 = no dampen, 1 = full dampen
        float retriggerThreshold = 0.3f; // AT level that triggers partial retrigger
        float retriggerDecayScale = 0.5f;// how much to shorten decay on retrigger
    };

    void setAftertouch(float pressure); // 0.0–1.0
    float getAftertouch() const { return currentPressure; }

    // Returns amplitude multiplier for dampening (1.0 = no dampen)
    float getDampenMultiplier() const;

    // Returns true if a retrigger should occur this frame
    bool shouldRetrigger();

    Config config;

private:
    float currentPressure = 0.0f;
    float previousPressure = 0.0f;
    bool retriggerArmed = true;
};
```

---

## MIDI Routing Strategy

Two modes supported (configurable):

### Mode 1: Multi-Channel
- Part 1 → MIDI Channel 1
- Part 2 → MIDI Channel 2
- Part 3 → MIDI Channel 3
- Part 4 → MIDI Channel 4
- Aftertouch is per-channel, naturally maps to each part

### Mode 2: Note-Number (GM Drum Map)
- All parts on Channel 10
- Note 36 (C1) → Part 1 (Kick)
- Note 38 (D1) → Part 2 (Snare)
- Note 42 (F#1) → Part 3 (Hat)
- Note 46 (A#1) → Part 4 (Perc)
- Polyphonic aftertouch maps per-note, channel aftertouch applies to last-triggered

---

## Aftertouch Implementation Detail

```cpp
void DrumVoice::applyAftertouch(float pressure) {
    atProcessor.setAftertouch(pressure);

    // 1. DAMPENING: Reduce amplitude based on pressure
    //    Higher pressure = more dampening (like palm-muting)
    float dampen = atProcessor.getDampenMultiplier();
    // Applied as a gain multiplier in processBlock

    // 2. PARTIAL RETRIGGER: When pressure crosses threshold upward,
    //    retrigger envelopes from current position with shortened decay
    if (atProcessor.shouldRetrigger()) {
        for (auto& env : envelopes) {
            env.retriggerPartial(pressure); // restart from current level
        }
    }
}

// In the Envelope class:
void Envelope::retriggerPartial(float intensity) {
    // Don't reset to zero — start attack from current level
    // Scale decay time by (1.0 - intensity) for tighter re-hit feel
    float scaledDecay = params.decay * (1.0f - intensity * 0.7f);
    stage = Stage::Attack;
    // currentLevel stays where it is — no click
    stageLengthSamples = (int)(params.attack * sampleRate);
    samplesInStage = 0;
}
```

---

## Parameter Layout

Use `juce::AudioProcessorValueTreeState` with a flat parameter scheme:

```cpp
// Parameters.h
namespace ParamIDs {
    // Per part (replace N with 1-4)
    // Sources
    constexpr auto SAMPLE1_LEVEL   = "partN_s1_level";
    constexpr auto SAMPLE1_TUNE    = "partN_s1_tune";
    constexpr auto SAMPLE1_START   = "partN_s1_start";
    constexpr auto SAMPLE2_LEVEL   = "partN_s2_level";
    constexpr auto SAMPLE2_TUNE    = "partN_s2_tune";
    constexpr auto SAMPLE2_START   = "partN_s2_start";
    constexpr auto SYNTH_WAVE      = "partN_syn_wave";
    constexpr auto SYNTH_LEVEL     = "partN_syn_level";
    constexpr auto SYNTH_TUNE      = "partN_syn_tune";
    constexpr auto SYNTH_PW        = "partN_syn_pw";
    constexpr auto NOISE_TYPE      = "partN_nse_type";
    constexpr auto NOISE_LEVEL     = "partN_nse_level";
    constexpr auto NOISE_FILTER    = "partN_nse_filter";
    constexpr auto NOISE_TONE      = "partN_nse_tone";

    // Envelopes (replace M with 1-4)
    constexpr auto ENV_ATTACK      = "partN_envM_a";
    constexpr auto ENV_DECAY       = "partN_envM_d";
    constexpr auto ENV_SUSTAIN     = "partN_envM_s";
    constexpr auto ENV_RELEASE     = "partN_envM_r";

    // Envelope assignment
    constexpr auto ENV_ASSIGN_S1   = "partN_ea_s1";  // which env (0-3)
    constexpr auto ENV_ASSIGN_S2   = "partN_ea_s2";
    constexpr auto ENV_ASSIGN_SYN  = "partN_ea_syn";
    constexpr auto ENV_ASSIGN_NSE  = "partN_ea_nse";

    // Aftertouch
    constexpr auto AT_DAMPEN       = "partN_at_damp";
    constexpr auto AT_RETRIGGER    = "partN_at_retrig";

    // Mix
    constexpr auto VOLUME          = "partN_vol";
    constexpr auto PAN             = "partN_pan";
    constexpr auto MUTE            = "partN_mute";
    constexpr auto SOLO            = "partN_solo";
}
```

This gives **~30 parameters × 4 parts = ~120 parameters** total, all automatable.

---

## processBlock Flow

```
PluginProcessor::processBlock()
│
├── Parse MIDI buffer → route to DrumEngine
│
└── DrumEngine::processBlock()
    │
    ├── for each MIDI event:
    │   ├── NoteOn  → voices[ch].trigger(velocity)
    │   ├── NoteOff → voices[ch].release()
    │   └── Aftertouch → voices[ch].applyAftertouch(pressure)
    │
    └── for each DrumVoice:
        │
        ├── EnvelopeBank::processBlock(numSamples)
        │   └── generates 4 envelope output buffers
        │
        ├── SamplePlayer1::processBlock(tempBuf)
        │   └── *= envelopeBank.output[envAssign[0]]
        │
        ├── SamplePlayer2::processBlock(tempBuf)
        │   └── *= envelopeBank.output[envAssign[1]]
        │
        ├── SynthOscillator::processBlock(tempBuf)
        │   └── *= envelopeBank.output[envAssign[2]]
        │
        ├── NoiseGenerator::processBlock(tempBuf)
        │   └── *= envelopeBank.output[envAssign[3]]
        │
        ├── Mix all 4 sources together
        ├── Apply aftertouch dampening multiplier
        ├── Apply volume + pan
        └── Sum into main output buffer
```

---

## CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.22)
project(PulseDrumSynth VERSION 0.1.0)

# Point to your JUCE installation
add_subdirectory(JUCE)

juce_add_plugin(PulseDrumSynth
    COMPANY_NAME "YourCompany"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    PLUGIN_MANUFACTURER_CODE Plse
    PLUGIN_CODE Plds
    FORMATS VST3 AU Standalone
    PRODUCT_NAME "PULSE Drum Synth"
)

target_sources(PulseDrumSynth PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/DSP/DrumEngine.cpp
    Source/DSP/DrumVoice.cpp
    Source/DSP/SamplePlayer.cpp
    Source/DSP/SynthOscillator.cpp
    Source/DSP/NoiseGenerator.cpp
    Source/DSP/Envelope.cpp
    Source/DSP/EnvelopeBank.cpp
    Source/DSP/AftertouchProcessor.cpp
    Source/GUI/VoiceStripComponent.cpp
    Source/GUI/TriggerPadComponent.cpp
    Source/GUI/KnobComponent.cpp
    Source/GUI/EnvelopeEditorComponent.cpp
    Source/GUI/SampleSlotComponent.cpp
    Source/GUI/WaveformSelector.cpp
    Source/GUI/EnvAssignComponent.cpp
    Source/GUI/HeaderComponent.cpp
    Source/Utility/Parameters.cpp
)

target_compile_features(PulseDrumSynth PUBLIC cxx_std_17)

target_link_libraries(PulseDrumSynth
    PRIVATE
        juce::juce_audio_utils
        juce::juce_audio_processors
        juce::juce_dsp
        juce::juce_gui_extra
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags
)
```

---

## Implementation Priority (Suggested Build Order)

1. **Scaffold**: PluginProcessor, PluginEditor, Parameters, CMakeLists
2. **Single Envelope + SynthOscillator**: Get a sine wave triggering from MIDI with an ADSR
3. **NoiseGenerator**: Add noise source, verify mixing
4. **SamplePlayer**: File loading, playback with pitch, start offset
5. **DrumVoice**: Wire all 4 sources + EnvelopeBank + routing
6. **DrumEngine**: 4-voice multitimbral MIDI routing
7. **AftertouchProcessor**: Dampening + partial retrigger
8. **GUI**: VoiceStrip layout, knobs, envelope editor, trigger pads
9. **Polish**: Preset system, drag-and-drop samples, parameter smoothing

---

## Key Design Decisions

- **No polyphony per part**: Each drum part is monophonic with voice stealing (retrigger). This keeps CPU low and matches drum machine behavior.
- **Envelope flexibility**: 4 envelopes per part is overkill for simple drums but enables complex layered sounds (e.g., pitch envelope on synth, separate amp for noise).
- **Aftertouch as expression**: Channel pressure maps naturally to dampening. The partial retrigger uses a threshold-crossing detector so rapid pressure changes create ghost-note effects.
- **Sample-rate independent envelopes**: All time parameters stored in seconds, converted to sample counts in `prepare()`.
- **Thread safety**: GUI reads parameters via `AudioProcessorValueTreeState` attachments. Sample loading uses a lock-free FIFO or `std::atomic` flag to signal the audio thread.
