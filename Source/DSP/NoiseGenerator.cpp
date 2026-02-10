#include "NoiseGenerator.h"

void NoiseGenerator::prepare (double sampleRate)
{
    currentSampleRate = sampleRate;
    filterState = 0.0f;
    std::fill (std::begin (pinkState), std::end (pinkState), 0.0f);
    metalBuffer.fill (0.0f);
    metalWritePos = 0;
}

void NoiseGenerator::processBlock (float* output, int numSamples)
{
    if (level <= 0.0001f)
    {
        juce::FloatVectorOperations::clear (output, numSamples);
        return;
    }

    for (int i = 0; i < numSamples; ++i)
    {
        float white = random.nextFloat() * 2.0f - 1.0f;
        float sample = 0.0f;

        switch (type)
        {
            case Type::White:
                sample = white;
                break;

            case Type::Pink:
            {
                pinkState[0] = 0.99886f * pinkState[0] + white * 0.0555179f;
                pinkState[1] = 0.99332f * pinkState[1] + white * 0.0750759f;
                pinkState[2] = 0.96900f * pinkState[2] + white * 0.1538520f;
                pinkState[3] = 0.86650f * pinkState[3] + white * 0.3104856f;
                pinkState[4] = 0.55000f * pinkState[4] + white * 0.5329522f;
                pinkState[5] = -0.7616f * pinkState[5] - white * 0.0168980f;
                sample = pinkState[0] + pinkState[1] + pinkState[2] + pinkState[3]
                       + pinkState[4] + pinkState[5] + pinkState[6] + white * 0.5362f;
                pinkState[6] = white * 0.115926f;
                sample *= 0.11f;
                break;
            }

            case Type::Metallic:
            {
                // Karplus-Strong-esque short delay for inharmonic metallic tones
                int delayLen = std::max (4, (int) (tone * 250.0f + 6.0f));
                int readPos = metalWritePos - delayLen;
                if (readPos < 0)
                    readPos += (int) metalBuffer.size();

                float delayed = metalBuffer[(size_t) readPos];
                sample = white * 0.3f + delayed * metalFeedback;

                // Simple averaging filter for the feedback path
                int readPos2 = readPos - 1;
                if (readPos2 < 0)
                    readPos2 += (int) metalBuffer.size();
                sample = (sample + metalBuffer[(size_t) readPos2]) * 0.5f;

                metalBuffer[(size_t) metalWritePos] = sample;
                metalWritePos = (metalWritePos + 1) % (int) metalBuffer.size();
                break;
            }
        }

        // One-pole lowpass filter
        float cutoff = std::clamp (filterCutoff, 0.0f, 1.0f);
        // Map 0-1 to coefficient: 0 = very filtered, 1 = wide open
        float coeff = cutoff * cutoff; // quadratic mapping for better feel
        filterState = filterState + coeff * (sample - filterState);
        sample = filterState;

        output[i] = sample * level;
    }
}
