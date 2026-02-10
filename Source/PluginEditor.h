#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"
#include "GUI/HeaderComponent.h"
#include "GUI/VoiceStripComponent.h"

class PulsePluginEditor : public juce::AudioProcessorEditor
{
public:
    explicit PulsePluginEditor (PulsePluginProcessor& p);
    ~PulsePluginEditor() override;

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    PulsePluginProcessor& processor;
    HeaderComponent header;
    juce::OwnedArray<VoiceStripComponent> voiceStrips;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PulsePluginEditor)
};
