#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "../Utility/Constants.h"

class HeaderComponent : public juce::Component
{
public:
    HeaderComponent();
    void paint (juce::Graphics& g) override;
};
