#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "../Utility/Constants.h"

class KnobComponent : public juce::Component
{
public:
    KnobComponent (const juce::String& label, juce::Colour accentColour, bool bipolar = false);

    void paint (juce::Graphics& g) override;
    void resized() override;

    juce::Slider& getSlider() { return slider; }

    void setAttachment (std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> att)
    {
        attachment = std::move (att);
    }

private:
    juce::Slider slider;
    juce::String labelText;
    juce::Colour accent;
    bool isBipolar;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (KnobComponent)
};
