#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "../Utility/Constants.h"

class TriggerPadComponent : public juce::Component,
                             public juce::Timer
{
public:
    TriggerPadComponent (const juce::String& name, juce::Colour accentColour);

    void paint (juce::Graphics& g) override;
    void mouseDown (const juce::MouseEvent& e) override;
    void timerCallback() override;

    std::function<void (float velocity)> onTrigger;

private:
    juce::String partName;
    juce::Colour accent;
    bool isPressed = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TriggerPadComponent)
};
