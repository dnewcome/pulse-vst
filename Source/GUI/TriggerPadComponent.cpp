#include "TriggerPadComponent.h"

TriggerPadComponent::TriggerPadComponent (const juce::String& name, juce::Colour accentColour)
    : partName (name), accent (accentColour)
{
}

void TriggerPadComponent::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    if (isPressed)
    {
        g.setGradientFill (juce::ColourGradient (accent.withAlpha (0.27f), bounds.getCentre(),
                                                  accent.withAlpha (0.07f), bounds.getTopLeft(), true));
        g.fillRoundedRectangle (bounds, 8.0f);
        g.setColour (accent);
        g.drawRoundedRectangle (bounds.reduced (1), 8.0f, 2.0f);
    }
    else
    {
        g.setGradientFill (juce::ColourGradient (Colours::padIdle, bounds.getCentre(),
                                                  Colours::bg, bounds.getTopLeft(), true));
        g.fillRoundedRectangle (bounds, 8.0f);
        g.setColour (Colours::stripBorder);
        g.drawRoundedRectangle (bounds.reduced (1), 8.0f, 2.0f);
    }

    g.setColour (isPressed ? accent : Colours::text);
    g.setFont (juce::Font (juce::Font::getDefaultMonospacedFontName(), 11.0f, juce::Font::bold));
    g.drawText (partName, bounds, juce::Justification::centred);
}

void TriggerPadComponent::mouseDown (const juce::MouseEvent&)
{
    isPressed = true;
    repaint();

    if (onTrigger)
        onTrigger (1.0f);

    startTimer (150);
}

void TriggerPadComponent::timerCallback()
{
    stopTimer();
    isPressed = false;
    repaint();
}
