#pragma once
#include <juce_audio_formats/juce_audio_formats.h>

class SamplePlayer
{
public:
    struct SampleData
    {
        juce::AudioBuffer<float> buffer;
        double sampleRate = 44100.0;
        juce::String fileName;
    };

    void prepare (double sampleRate);
    void trigger (float velocity);
    void stop();
    bool processBlock (float* output, int numSamples);

    // Called from GUI/message thread
    void loadSample (const juce::File& file, juce::AudioFormatManager& formatManager);

    float level = 1.0f;
    float tune = 0.0f;       // semitones -24 to +24
    float startPos = 0.0f;   // 0.0 to 1.0

    bool isLoaded() const { return std::atomic_load (&currentSample) != nullptr; }
    bool isPlaying() const { return playing; }
    juce::String getFileName() const;
    juce::String getFilePath() const { return loadedFilePath; }

    // Load from a saved path (for state restore)
    void loadFromPath (const juce::String& path, juce::AudioFormatManager& formatManager);

private:
    float interpolate (double pos, const float* data, int length) const;

    std::shared_ptr<SampleData> currentSample;
    double hostSampleRate = 44100.0;
    double playbackRate = 1.0;
    double position = 0.0;
    float currentVelocity = 0.0f;
    bool playing = false;
    juce::String loadedFilePath;
};
