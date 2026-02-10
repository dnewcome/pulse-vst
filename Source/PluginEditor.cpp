#include "PluginEditor.h"

PulsePluginEditor::PulsePluginEditor (PulsePluginProcessor& p)
    : AudioProcessorEditor (p), processor (p)
{
    addAndMakeVisible (header);

    for (int i = 0; i < Constants::NUM_PARTS; ++i)
    {
        auto* strip = new VoiceStripComponent (i, Colours::accent[i], processor.apvts, processor);
        addAndMakeVisible (strip);
        voiceStrips.add (strip);
    }

    setSize (960, 820);
    setResizable (true, true);
    setResizeLimits (800, 700, 1600, 1200);
}

PulsePluginEditor::~PulsePluginEditor() {}

void PulsePluginEditor::paint (juce::Graphics& g)
{
    g.fillAll (Colours::bg);

    // Footer
    auto footerArea = getLocalBounds().removeFromBottom (32).toFloat();
    g.setColour (Colours::surface);
    g.fillRect (footerArea);
    g.setColour (Colours::stripBorder);
    g.drawLine (0, footerArea.getY(), footerArea.getWidth(), footerArea.getY(), 1.0f);

    auto monoFont = juce::Font::getDefaultMonospacedFontName();
    g.setColour (Colours::textMuted);
    g.setFont (juce::Font (juce::FontOptions (monoFont, 8.0f, juce::Font::plain)));
    g.drawText ("MIDI: MULTITIMBRAL CH 1-4    AFTERTOUCH: CHANNEL PRESSURE    POLYPHONY: 4-VOICE",
                footerArea.reduced (20, 0).toNearestInt(), juce::Justification::centredLeft);
    g.drawText ("v0.1.0", footerArea.reduced (20, 0).toNearestInt(), juce::Justification::centredRight);
}

void PulsePluginEditor::resized()
{
    auto area = getLocalBounds();
    header.setBounds (area.removeFromTop (44));
    area.removeFromBottom (32); // footer

    auto stripArea = area.reduced (4, 4);
    int stripWidth = stripArea.getWidth() / Constants::NUM_PARTS;

    for (int i = 0; i < voiceStrips.size(); ++i)
        voiceStrips[i]->setBounds (stripArea.removeFromLeft (stripWidth).reduced (2));
}
