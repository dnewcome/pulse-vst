#include "SynthOscillator.h"

void SynthOscillator::prepare (double sampleRate)
{
    currentSampleRate = sampleRate;
    phase = 0.0;
    playing = false;
}

void SynthOscillator::trigger (float velocity, float frequency)
{
    currentVelocity = velocity;
    double tunedFreq = frequency * std::pow (2.0, tune / 12.0);
    phaseIncrement = tunedFreq / currentSampleRate;
    phase = 0.0;
    playing = true;
}

void SynthOscillator::stop()
{
    playing = false;
}

float SynthOscillator::polyBLEP (double t, double dt) const
{
    if (t < dt)
    {
        t /= dt;
        return (float) (t + t - t * t - 1.0);
    }
    if (t > 1.0 - dt)
    {
        t = (t - 1.0) / dt;
        return (float) (t * t + t + t + 1.0);
    }
    return 0.0f;
}

void SynthOscillator::processBlock (float* output, int numSamples)
{
    if (! playing)
    {
        juce::FloatVectorOperations::clear (output, numSamples);
        return;
    }

    double dt = phaseIncrement;

    for (int i = 0; i < numSamples; ++i)
    {
        float sample = 0.0f;

        switch (waveform)
        {
            case Waveform::Sine:
                sample = (float) std::sin (phase * juce::MathConstants<double>::twoPi);
                break;

            case Waveform::Triangle:
            {
                sample = (float) (2.0 * std::abs (2.0 * phase - 1.0) - 1.0);
                break;
            }

            case Waveform::Saw:
            {
                sample = (float) (2.0 * phase - 1.0);
                sample -= polyBLEP (phase, dt);
                break;
            }

            case Waveform::Square:
            {
                sample = (phase < (double) pulseWidth) ? 1.0f : -1.0f;
                sample += polyBLEP (phase, dt);
                double shiftedPhase = phase + (1.0 - (double) pulseWidth);
                if (shiftedPhase >= 1.0)
                    shiftedPhase -= 1.0;
                sample -= polyBLEP (shiftedPhase, dt);
                break;
            }
        }

        output[i] = sample * level * currentVelocity;

        phase += dt;
        if (phase >= 1.0)
            phase -= 1.0;
    }
}
