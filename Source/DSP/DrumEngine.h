#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "DrumVoice.h"
#include "../Utility/Constants.h"

class DrumEngine
{
public:
    void prepare (double sampleRate, int maxBlockSize);
    void processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi);

    void initParameters (juce::AudioProcessorValueTreeState& apvts);

    DrumVoice& getVoice (int index) { return voices[(size_t) index]; }

    // For GUI trigger pads — injects a note into the next process block
    void triggerVoiceFromGUI (int partIndex, float velocity);

private:
    void handleMidiEvent (const juce::MidiMessage& msg);
    int noteToPartIndex (int note) const;

    std::array<DrumVoice, Constants::NUM_PARTS> voices;

    // GUI trigger FIFO
    struct GUITrigger { int partIndex; float velocity; };
    std::array<GUITrigger, 16> guiTriggerBuffer;
    std::atomic<int> guiTriggerCount { 0 };
};
