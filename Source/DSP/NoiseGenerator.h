#pragma once
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_core/juce_core.h>

class NoiseGenerator
{
public:
    enum class Type { White, Pink, Metallic };

    void prepare (double sampleRate);
    void processBlock (float* output, int numSamples);

    Type type = Type::White;
    float level = 1.0f;
    float filterCutoff = 1.0f;
    float tone = 0.5f;

private:
    juce::Random random;

    // Pink noise state (Paul Kellet's method)
    float pinkState[7] = {};

    // Metallic noise (short delay + feedback)
    std::array<float, 256> metalBuffer = {};
    int metalWritePos = 0;
    float metalFeedback = 0.95f;

    // One-pole filter
    float filterState = 0.0f;

    double currentSampleRate = 44100.0;
};
