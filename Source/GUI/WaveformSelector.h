#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "../Utility/Constants.h"

class WaveformSelector : public juce::Component
{
public:
    WaveformSelector (const juce::StringArray& options, juce::Colour accentColour);

    void paint (juce::Graphics& g) override;
    void mouseDown (const juce::MouseEvent& e) override;

    void setSelectedIndex (int index);
    int getSelectedIndex() const { return selectedIndex; }

    void setAttachment (std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> att);

    // Internal combo box for APVTS attachment
    juce::ComboBox& getComboBox() { return hiddenCombo; }

private:
    juce::StringArray items;
    juce::Colour accent;
    int selectedIndex = 0;
    juce::ComboBox hiddenCombo;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> attachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveformSelector)
};
