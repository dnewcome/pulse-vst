#include "SamplePlayer.h"

void SamplePlayer::prepare (double sampleRate)
{
    hostSampleRate = sampleRate;
    playing = false;
    position = 0.0;
}

void SamplePlayer::trigger (float velocity)
{
    auto sample = std::atomic_load (&currentSample);
    if (sample == nullptr || sample->buffer.getNumSamples() == 0)
        return;

    currentVelocity = velocity;
    playbackRate = (sample->sampleRate / hostSampleRate) * std::pow (2.0, tune / 12.0);
    position = startPos * sample->buffer.getNumSamples();
    playing = true;
}

void SamplePlayer::stop()
{
    playing = false;
}

bool SamplePlayer::processBlock (float* output, int numSamples)
{
    if (! playing)
    {
        juce::FloatVectorOperations::clear (output, numSamples);
        return false;
    }

    auto sample = std::atomic_load (&currentSample);
    if (sample == nullptr)
    {
        playing = false;
        juce::FloatVectorOperations::clear (output, numSamples);
        return false;
    }

    const float* data = sample->buffer.getReadPointer (0);
    int sampleLength = sample->buffer.getNumSamples();

    // Recalculate playback rate in case tune changed
    playbackRate = (sample->sampleRate / hostSampleRate) * std::pow (2.0, tune / 12.0);

    for (int i = 0; i < numSamples; ++i)
    {
        if (position >= sampleLength - 1)
        {
            playing = false;
            for (int j = i; j < numSamples; ++j)
                output[j] = 0.0f;
            return i > 0; // was playing for part of the block
        }

        output[i] = interpolate (position, data, sampleLength) * level * currentVelocity;
        position += playbackRate;
    }

    return true;
}

float SamplePlayer::interpolate (double pos, const float* data, int length) const
{
    int idx = (int) pos;
    float frac = (float) (pos - idx);

    if (idx < 0)
        return 0.0f;
    if (idx + 1 >= length)
        return data[juce::jmin (idx, length - 1)];

    return data[idx] * (1.0f - frac) + data[idx + 1] * frac;
}

void SamplePlayer::loadSample (const juce::File& file, juce::AudioFormatManager& formatManager)
{
    auto reader = std::unique_ptr<juce::AudioFormatReader> (formatManager.createReaderFor (file));
    if (reader == nullptr)
        return;

    auto newSample = std::make_shared<SampleData>();
    int numChannels = (int) reader->numChannels;
    int numSamples = (int) reader->lengthInSamples;

    // Read as mono (mix down if stereo)
    newSample->buffer.setSize (1, numSamples);

    if (numChannels == 1)
    {
        reader->read (&newSample->buffer, 0, numSamples, 0, true, false);
    }
    else
    {
        juce::AudioBuffer<float> tempBuf (numChannels, numSamples);
        reader->read (&tempBuf, 0, numSamples, 0, true, true);

        auto* mono = newSample->buffer.getWritePointer (0);
        for (int i = 0; i < numSamples; ++i)
        {
            float sum = 0.0f;
            for (int ch = 0; ch < numChannels; ++ch)
                sum += tempBuf.getSample (ch, i);
            mono[i] = sum / (float) numChannels;
        }
    }

    newSample->sampleRate = reader->sampleRate;
    newSample->fileName = file.getFileName();
    loadedFilePath = file.getFullPathName();

    std::atomic_store (&currentSample, newSample);
}

juce::String SamplePlayer::getFileName() const
{
    auto sample = std::atomic_load (&currentSample);
    if (sample != nullptr)
        return sample->fileName;
    return {};
}

void SamplePlayer::loadFromPath (const juce::String& path, juce::AudioFormatManager& formatManager)
{
    if (path.isNotEmpty())
    {
        juce::File file (path);
        if (file.existsAsFile())
            loadSample (file, formatManager);
    }
}
