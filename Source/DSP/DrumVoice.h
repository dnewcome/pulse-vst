#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "SamplePlayer.h"
#include "SynthOscillator.h"
#include "NoiseGenerator.h"
#include "EnvelopeBank.h"
#include "AftertouchProcessor.h"
#include "../Utility/Constants.h"
#include "../Utility/Parameters.h"

class DrumVoice
{
public:
    struct ParameterRefs
    {
        std::atomic<float>* s1Level = nullptr;
        std::atomic<float>* s1Tune = nullptr;
        std::atomic<float>* s1Start = nullptr;
        std::atomic<float>* s2Level = nullptr;
        std::atomic<float>* s2Tune = nullptr;
        std::atomic<float>* s2Start = nullptr;
        std::atomic<float>* synWave = nullptr;
        std::atomic<float>* synLevel = nullptr;
        std::atomic<float>* synTune = nullptr;
        std::atomic<float>* synPW = nullptr;
        std::atomic<float>* synFreq = nullptr;
        std::atomic<float>* nseType = nullptr;
        std::atomic<float>* nseLevel = nullptr;
        std::atomic<float>* nseFilt = nullptr;
        std::atomic<float>* nseTone = nullptr;
        std::atomic<float>* atDamp = nullptr;
        std::atomic<float>* atRetrig = nullptr;
        std::atomic<float>* volume = nullptr;
        std::atomic<float>* pan = nullptr;
        std::atomic<float>* mute = nullptr;
        std::atomic<float>* solo = nullptr;

        // Envelope ADSR params [env_index]
        std::atomic<float>* envA[Constants::NUM_ENVELOPES_PER_PART] = {};
        std::atomic<float>* envD[Constants::NUM_ENVELOPES_PER_PART] = {};
        std::atomic<float>* envS[Constants::NUM_ENVELOPES_PER_PART] = {};
        std::atomic<float>* envR[Constants::NUM_ENVELOPES_PER_PART] = {};

        // Envelope assignments (choice params, stored as float 0-3)
        std::atomic<float>* eaS1 = nullptr;
        std::atomic<float>* eaS2 = nullptr;
        std::atomic<float>* eaSyn = nullptr;
        std::atomic<float>* eaNse = nullptr;
    };

    void prepare (double sampleRate, int maxBlockSize);
    void setParameterRefs (ParameterRefs refs);
    void trigger (float velocity);
    void release();
    void applyAftertouch (float pressure);
    void processBlock (juce::AudioBuffer<float>& output, int numSamples);

    SamplePlayer& getSample1() { return sample1; }
    SamplePlayer& getSample2() { return sample2; }

    bool isMuted() const { return params.mute != nullptr && params.mute->load() > 0.5f; }
    bool isSoloed() const { return params.solo != nullptr && params.solo->load() > 0.5f; }

private:
    void readParameters();

    ParameterRefs params;

    SamplePlayer sample1, sample2;
    SynthOscillator synth;
    NoiseGenerator noise;
    EnvelopeBank envelopes;
    AftertouchProcessor atProcessor;

    std::array<int, 4> envAssign = { 0, 0, 1, 2 };

    float volume = 1.0f;
    float pan = 0.0f;

    // Temp buffers for per-source rendering
    std::vector<float> tempS1, tempS2, tempSyn, tempNse;
    std::vector<float> tempMix;
};
