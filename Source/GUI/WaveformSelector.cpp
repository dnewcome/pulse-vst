#include "WaveformSelector.h"

WaveformSelector::WaveformSelector (const juce::StringArray& options, juce::Colour accentColour)
    : items (options), accent (accentColour)
{
    for (int i = 0; i < items.size(); ++i)
        hiddenCombo.addItem (items[i], i + 1);
    hiddenCombo.setSelectedId (1, juce::dontSendNotification);
    hiddenCombo.setVisible (false);
    addChildComponent (hiddenCombo);

    hiddenCombo.onChange = [this]
    {
        selectedIndex = hiddenCombo.getSelectedItemIndex();
        repaint();
    };
}

void WaveformSelector::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    int numItems = items.size();
    if (numItems == 0) return;

    float itemWidth = (float) bounds.getWidth() / (float) numItems;
    float gap = 4.0f;

    auto monoFont = juce::Font (juce::FontOptions (juce::Font::getDefaultMonospacedFontName(), 8.0f, juce::Font::plain));
    g.setFont (monoFont);

    for (int i = 0; i < numItems; ++i)
    {
        auto itemBounds = juce::Rectangle<float> (i * itemWidth + gap / 2, 0,
                                                   itemWidth - gap, (float) bounds.getHeight());
        bool selected = (i == selectedIndex);

        if (selected)
        {
            g.setColour (accent.withAlpha (0.2f));
            g.fillRoundedRectangle (itemBounds, 3.0f);
            g.setColour (accent.withAlpha (0.4f));
            g.drawRoundedRectangle (itemBounds, 3.0f, 1.0f);
            g.setColour (accent);
        }
        else
        {
            g.setColour (Colours::stripBorder);
            g.drawRoundedRectangle (itemBounds, 3.0f, 1.0f);
            g.setColour (Colours::textMuted);
        }

        g.drawText (items[i].toUpperCase(), itemBounds, juce::Justification::centred);
    }
}

void WaveformSelector::mouseDown (const juce::MouseEvent& e)
{
    int numItems = items.size();
    if (numItems == 0) return;

    float itemWidth = (float) getWidth() / (float) numItems;
    int clicked = (int) (e.position.x / itemWidth);
    clicked = juce::jlimit (0, numItems - 1, clicked);

    hiddenCombo.setSelectedItemIndex (clicked, juce::sendNotificationSync);
}

void WaveformSelector::setSelectedIndex (int index)
{
    selectedIndex = index;
    hiddenCombo.setSelectedItemIndex (index, juce::dontSendNotification);
    repaint();
}

void WaveformSelector::setAttachment (std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> att)
{
    attachment = std::move (att);
    selectedIndex = hiddenCombo.getSelectedItemIndex();
    repaint();
}
