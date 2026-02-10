#pragma once
#include <cmath>
#include <algorithm>

class Envelope
{
public:
    enum class Stage { Idle, Attack, Decay, Sustain, Release };

    struct Parameters
    {
        float attack  = 0.01f;
        float decay   = 0.3f;
        float sustain = 0.5f;
        float release = 0.4f;
    };

    void prepare (double sampleRate);
    void trigger();
    void release();
    void retriggerPartial (float intensity);
    void forceRelease (float time);

    void processBlock (float* output, int numSamples);

    float getCurrentLevel() const { return currentLevel; }
    Stage getStage() const { return stage; }
    bool isActive() const { return stage != Stage::Idle; }

    Parameters params;

private:
    void advanceStage();

    Stage stage = Stage::Idle;
    float currentLevel = 0.0f;
    float startLevel = 0.0f;
    float targetLevel = 0.0f;
    double sampleRate = 44100.0;
    int samplesInStage = 0;
    int stageLengthSamples = 0;
};
