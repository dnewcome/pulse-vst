#pragma once
#include <juce_audio_basics/juce_audio_basics.h>

class SynthOscillator
{
public:
    enum class Waveform { Sine, Triangle, Saw, Square };

    void prepare (double sampleRate);
    void trigger (float velocity, float frequency);
    void stop();
    void processBlock (float* output, int numSamples);

    Waveform waveform = Waveform::Sine;
    float tune = 0.0f;       // semitones
    float pulseWidth = 0.5f;
    float level = 1.0f;

    bool isPlaying() const { return playing; }

private:
    float polyBLEP (double t, double dt) const;

    double phase = 0.0;
    double phaseIncrement = 0.0;
    double currentSampleRate = 44100.0;
    float currentVelocity = 0.0f;
    bool playing = false;
};
