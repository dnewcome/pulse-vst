#pragma once
#include "Envelope.h"
#include "../Utility/Constants.h"

class EnvelopeBank
{
public:
    static constexpr int NUM_ENVELOPES = Constants::NUM_ENVELOPES_PER_PART;

    void prepare (double sampleRate, int maxBlockSize);
    void triggerAll();
    void releaseAll();
    void processBlock (int numSamples);

    Envelope& getEnvelope (int index) { return envelopes[(size_t) index]; }
    const float* getEnvelopeOutput (int index) const { return outputBuffers[(size_t) index].data(); }

    bool isAnyActive() const;

private:
    std::array<Envelope, NUM_ENVELOPES> envelopes;
    std::array<std::vector<float>, NUM_ENVELOPES> outputBuffers;
};
