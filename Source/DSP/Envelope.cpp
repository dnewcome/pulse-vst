#include "Envelope.h"

void Envelope::prepare (double sr)
{
    sampleRate = sr;
    stage = Stage::Idle;
    currentLevel = 0.0f;
}

void Envelope::trigger()
{
    stage = Stage::Attack;
    startLevel = currentLevel; // no click — start from wherever we are
    targetLevel = 1.0f;
    samplesInStage = 0;
    stageLengthSamples = std::max (1, (int) (params.attack * sampleRate));
}

void Envelope::release()
{
    if (stage == Stage::Idle)
        return;

    stage = Stage::Release;
    startLevel = currentLevel;
    targetLevel = 0.0f;
    samplesInStage = 0;
    stageLengthSamples = std::max (1, (int) (params.release * sampleRate));
}

void Envelope::retriggerPartial (float /*intensity*/)
{
    // Restart attack from current level with optionally shortened decay
    stage = Stage::Attack;
    startLevel = currentLevel;
    targetLevel = 1.0f;
    samplesInStage = 0;
    stageLengthSamples = std::max (1, (int) (params.attack * sampleRate));
}

void Envelope::forceRelease (float time)
{
    stage = Stage::Release;
    startLevel = currentLevel;
    targetLevel = 0.0f;
    samplesInStage = 0;
    stageLengthSamples = std::max (1, (int) (time * sampleRate));
}

void Envelope::advanceStage()
{
    switch (stage)
    {
        case Stage::Attack:
            stage = Stage::Decay;
            startLevel = 1.0f;
            targetLevel = params.sustain;
            samplesInStage = 0;
            stageLengthSamples = std::max (1, (int) (params.decay * sampleRate));
            break;

        case Stage::Decay:
            stage = Stage::Sustain;
            currentLevel = params.sustain;
            startLevel = params.sustain;
            targetLevel = params.sustain;
            samplesInStage = 0;
            stageLengthSamples = 0;
            break;

        case Stage::Sustain:
            break;

        case Stage::Release:
            stage = Stage::Idle;
            currentLevel = 0.0f;
            break;

        default:
            break;
    }
}

void Envelope::processBlock (float* output, int numSamples)
{
    for (int i = 0; i < numSamples; ++i)
    {
        if (stage == Stage::Idle)
        {
            output[i] = 0.0f;
            continue;
        }

        if (stage == Stage::Sustain)
        {
            currentLevel = params.sustain;
            output[i] = currentLevel;
            continue;
        }

        if (stageLengthSamples > 0)
        {
            float progress = (float) samplesInStage / (float) stageLengthSamples;
            currentLevel = startLevel + (targetLevel - startLevel) * progress;
        }

        output[i] = currentLevel;
        ++samplesInStage;

        if (samplesInStage >= stageLengthSamples)
            advanceStage();
    }
}
