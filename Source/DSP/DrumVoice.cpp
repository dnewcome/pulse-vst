#include "DrumVoice.h"

void DrumVoice::prepare (double sampleRate, int maxBlockSize)
{
    sample1.prepare (sampleRate);
    sample2.prepare (sampleRate);
    synth.prepare (sampleRate);
    noise.prepare (sampleRate);
    envelopes.prepare (sampleRate, maxBlockSize);

    tempS1.resize ((size_t) maxBlockSize, 0.0f);
    tempS2.resize ((size_t) maxBlockSize, 0.0f);
    tempSyn.resize ((size_t) maxBlockSize, 0.0f);
    tempNse.resize ((size_t) maxBlockSize, 0.0f);
    tempMix.resize ((size_t) maxBlockSize, 0.0f);
}

void DrumVoice::setParameterRefs (ParameterRefs refs)
{
    params = refs;
}

void DrumVoice::readParameters()
{
    if (params.s1Level == nullptr)
        return;

    sample1.level = params.s1Level->load();
    sample1.tune = params.s1Tune->load();
    sample1.startPos = params.s1Start->load();

    sample2.level = params.s2Level->load();
    sample2.tune = params.s2Tune->load();
    sample2.startPos = params.s2Start->load();

    synth.waveform = static_cast<SynthOscillator::Waveform> ((int) params.synWave->load());
    synth.level = params.synLevel->load();
    synth.tune = params.synTune->load();
    synth.pulseWidth = params.synPW->load();

    noise.type = static_cast<NoiseGenerator::Type> ((int) params.nseType->load());
    noise.level = params.nseLevel->load();
    noise.filterCutoff = params.nseFilt->load();
    noise.tone = params.nseTone->load();

    atProcessor.config.dampenSensitivity = params.atDamp->load();
    atProcessor.config.retriggerThreshold = params.atRetrig->load();

    volume = params.volume->load();
    pan = params.pan->load();

    // Read envelope parameters
    for (int e = 0; e < Constants::NUM_ENVELOPES_PER_PART; ++e)
    {
        auto& env = envelopes.getEnvelope (e);
        env.params.attack = params.envA[e]->load();
        env.params.decay = params.envD[e]->load();
        env.params.sustain = params.envS[e]->load();
        env.params.release = params.envR[e]->load();
    }

    // Read envelope assignments
    envAssign[0] = (int) params.eaS1->load();
    envAssign[1] = (int) params.eaS2->load();
    envAssign[2] = (int) params.eaSyn->load();
    envAssign[3] = (int) params.eaNse->load();
}

void DrumVoice::trigger (float velocity)
{
    readParameters();

    sample1.trigger (velocity);
    sample2.trigger (velocity);
    synth.trigger (velocity, params.synFreq != nullptr ? params.synFreq->load() : 60.0f);
    envelopes.triggerAll();
}

void DrumVoice::release()
{
    envelopes.releaseAll();
}

void DrumVoice::applyAftertouch (float pressure)
{
    atProcessor.setAftertouch (pressure);

    if (atProcessor.shouldRetrigger())
    {
        for (int e = 0; e < Constants::NUM_ENVELOPES_PER_PART; ++e)
            envelopes.getEnvelope (e).retriggerPartial (pressure);
    }
}

void DrumVoice::processBlock (juce::AudioBuffer<float>& output, int numSamples)
{
    if (! envelopes.isAnyActive())
        return;

    readParameters();

    // Process envelopes
    envelopes.processBlock (numSamples);

    // Process each source
    sample1.processBlock (tempS1.data(), numSamples);
    sample2.processBlock (tempS2.data(), numSamples);
    synth.processBlock (tempSyn.data(), numSamples);
    noise.processBlock (tempNse.data(), numSamples);

    // Mix sources with envelope modulation
    float dampen = atProcessor.getDampenMultiplier();
    float leftGain = volume * dampen * std::sqrt (0.5f * (1.0f - pan));
    float rightGain = volume * dampen * std::sqrt (0.5f * (1.0f + pan));

    auto* outL = output.getWritePointer (0);
    auto* outR = output.getNumChannels() > 1 ? output.getWritePointer (1) : nullptr;

    const float* envOut0 = envelopes.getEnvelopeOutput (envAssign[0]);
    const float* envOut1 = envelopes.getEnvelopeOutput (envAssign[1]);
    const float* envOut2 = envelopes.getEnvelopeOutput (envAssign[2]);
    const float* envOut3 = envelopes.getEnvelopeOutput (envAssign[3]);

    for (int i = 0; i < numSamples; ++i)
    {
        float mono = tempS1[(size_t) i] * envOut0[i]
                   + tempS2[(size_t) i] * envOut1[i]
                   + tempSyn[(size_t) i] * envOut2[i]
                   + tempNse[(size_t) i] * envOut3[i];

        outL[i] += mono * leftGain;
        if (outR != nullptr)
            outR[i] += mono * rightGain;
    }
}
