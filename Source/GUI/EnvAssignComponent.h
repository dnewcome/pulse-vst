#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "../Utility/Constants.h"

class EnvAssignComponent : public juce::Component
{
public:
    EnvAssignComponent (juce::Colour accentColour);

    void paint (juce::Graphics& g) override;
    void mouseDown (const juce::MouseEvent& e) override;

    void setSelectedEnvelope (int envIndex) { selectedEnv = envIndex; repaint(); }
    int getSelectedEnvelope() const { return selectedEnv; }

    // Connect the 4 envelope assignment choice params
    void connectToParameters (juce::AudioProcessorValueTreeState& apvts,
                              const juce::String& s1ID,
                              const juce::String& s2ID,
                              const juce::String& synID,
                              const juce::String& nseID);

private:
    juce::Colour accent;
    int selectedEnv = 0;
    static constexpr int NUM_SOURCES = 4;
    const juce::String sourceLabels[NUM_SOURCES] = { "S1", "S2", "SYN", "NSE" };

    juce::RangedAudioParameter* assignParams[NUM_SOURCES] = {};
};
