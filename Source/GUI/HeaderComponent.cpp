#include "HeaderComponent.h"

HeaderComponent::HeaderComponent() {}

void HeaderComponent::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    g.setColour (Colours::surface);
    g.fillRect (bounds);
    g.setColour (Colours::stripBorder);
    g.drawLine (0, bounds.getBottom(), bounds.getWidth(), bounds.getBottom(), 1.0f);

    auto monoFont = juce::Font::getDefaultMonospacedFontName();

    // Title
    g.setColour (Colours::text);
    g.setFont (juce::Font (monoFont, 16.0f, juce::Font::bold));
    g.drawText ("PULSE", 20, 0, 100, (int) bounds.getHeight(), juce::Justification::centredLeft);

    // Subtitle
    g.setColour (Colours::textDim);
    g.setFont (juce::Font (monoFont, 9.0f, juce::Font::plain));
    g.drawText ("DRUM SYNTH", 110, 0, 100, (int) bounds.getHeight(), juce::Justification::centredLeft);

    // Right side info
    g.setColour (Colours::textMuted);
    g.setFont (juce::Font (monoFont, 8.0f, juce::Font::plain));
    juce::String info = juce::String::fromUTF8("4-VOICE HYBRID \xe2\x80\xa2 SAMPLE + SYNTH");
    g.drawText (info, bounds.reduced (20, 0).toNearestInt(), juce::Justification::centredRight);

    // LED
    float ledX = bounds.getWidth() - 24;
    float ledY = bounds.getCentreY() - 4;
    g.setColour (Colours::led);
    g.fillEllipse (ledX, ledY, 8, 8);
    g.setColour (Colours::led.withAlpha (0.4f));
    g.fillEllipse (ledX - 2, ledY - 2, 12, 12);
}
