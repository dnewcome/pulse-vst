#include "EnvAssignComponent.h"

EnvAssignComponent::EnvAssignComponent (juce::Colour accentColour)
    : accent (accentColour)
{
}

void EnvAssignComponent::connectToParameters (juce::AudioProcessorValueTreeState& apvts,
                                               const juce::String& s1ID,
                                               const juce::String& s2ID,
                                               const juce::String& synID,
                                               const juce::String& nseID)
{
    assignParams[0] = apvts.getParameter (s1ID);
    assignParams[1] = apvts.getParameter (s2ID);
    assignParams[2] = apvts.getParameter (synID);
    assignParams[3] = apvts.getParameter (nseID);
}

void EnvAssignComponent::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    g.setColour (Colours::surfaceAlt);
    g.fillRoundedRectangle (bounds, 4.0f);
    g.setColour (Colours::stripBorder);
    g.drawRoundedRectangle (bounds, 4.0f, 1.0f);

    auto inner = bounds.reduced (6, 4);

    // Label
    auto monoFont = juce::Font (juce::FontOptions (juce::Font::getDefaultMonospacedFontName(), 8.0f, juce::Font::plain));
    g.setColour (Colours::textMuted);
    g.setFont (monoFont);
    g.drawText (juce::String::fromUTF8("ASSIGN \xe2\x86\x92 ENV ") + juce::String (selectedEnv + 1),
                inner.removeFromTop (12), juce::Justification::centredLeft);

    // Source buttons
    auto buttonArea = inner;
    float bw = buttonArea.getWidth() / NUM_SOURCES;
    float gap = 3.0f;

    for (int i = 0; i < NUM_SOURCES; ++i)
    {
        auto btnBounds = juce::Rectangle<float> (buttonArea.getX() + i * bw + gap / 2,
                                                  buttonArea.getY(),
                                                  bw - gap,
                                                  buttonArea.getHeight());

        // Check if this source is assigned to the selected envelope
        bool assigned = false;
        if (assignParams[i] != nullptr)
        {
            int assignedEnv = (int) assignParams[i]->convertFrom0to1 (assignParams[i]->getValue());
            assigned = (assignedEnv == selectedEnv);
        }

        if (assigned)
        {
            g.setColour (accent.withAlpha (0.13f));
            g.fillRoundedRectangle (btnBounds, 3.0f);
            g.setColour (accent.withAlpha (0.27f));
            g.drawRoundedRectangle (btnBounds, 3.0f, 1.0f);
            g.setColour (accent);
        }
        else
        {
            g.setColour (Colours::stripBorder);
            g.drawRoundedRectangle (btnBounds, 3.0f, 1.0f);
            g.setColour (Colours::textMuted);
        }

        g.setFont (monoFont);
        g.drawText (sourceLabels[i], btnBounds, juce::Justification::centred);
    }
}

void EnvAssignComponent::mouseDown (const juce::MouseEvent& e)
{
    auto buttonArea = getLocalBounds().toFloat().reduced (6, 4);
    buttonArea.removeFromTop (12);

    float bw = buttonArea.getWidth() / NUM_SOURCES;
    int clicked = (int) ((e.position.x - buttonArea.getX()) / bw);
    clicked = juce::jlimit (0, NUM_SOURCES - 1, clicked);

    if (e.position.y < buttonArea.getY())
        return;

    // Toggle: if this source is assigned to selectedEnv, it stays (or cycles)
    // For simplicity: clicking assigns this source to the current selected envelope
    if (assignParams[clicked] != nullptr)
    {
        assignParams[clicked]->setValueNotifyingHost (
            assignParams[clicked]->convertTo0to1 ((float) selectedEnv));
    }

    repaint();
}
