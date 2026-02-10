#include "EnvelopeBank.h"

void EnvelopeBank::prepare (double sampleRate, int maxBlockSize)
{
    for (auto& env : envelopes)
        env.prepare (sampleRate);

    for (auto& buf : outputBuffers)
        buf.resize ((size_t) maxBlockSize, 0.0f);
}

void EnvelopeBank::triggerAll()
{
    for (auto& env : envelopes)
        env.trigger();
}

void EnvelopeBank::releaseAll()
{
    for (auto& env : envelopes)
        env.release();
}

void EnvelopeBank::processBlock (int numSamples)
{
    for (int i = 0; i < NUM_ENVELOPES; ++i)
        envelopes[(size_t) i].processBlock (outputBuffers[(size_t) i].data(), numSamples);
}

bool EnvelopeBank::isAnyActive() const
{
    for (auto& env : envelopes)
        if (env.isActive())
            return true;
    return false;
}
