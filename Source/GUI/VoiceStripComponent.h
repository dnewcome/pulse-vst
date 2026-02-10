#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "../Utility/Constants.h"
#include "../Utility/Parameters.h"
#include "KnobComponent.h"
#include "EnvelopeEditorComponent.h"
#include "SampleSlotComponent.h"
#include "WaveformSelector.h"
#include "TriggerPadComponent.h"
#include "EnvAssignComponent.h"

class PulsePluginProcessor;

class VoiceStripComponent : public juce::Component
{
public:
    VoiceStripComponent (int partIndex, juce::Colour accentColour,
                         juce::AudioProcessorValueTreeState& apvts,
                         PulsePluginProcessor& processor);

    void paint (juce::Graphics& g) override;
    void resized() override;
    void mouseDown (const juce::MouseEvent& e) override;

    void setEnvelopeTab (int index);

private:
    juce::Rectangle<float> envTabArea; // stored during resized for hit testing
    int partIndex;
    juce::Colour accent;
    juce::AudioProcessorValueTreeState& apvts;
    PulsePluginProcessor& processor;

    // Sample slots
    SampleSlotComponent sample1Slot, sample2Slot;

    // Synth section
    KnobComponent synthLevel, synthTune, synthPW;
    WaveformSelector waveformSelector;

    // Noise section
    KnobComponent noiseLevel, noiseFilter, noiseTone;
    WaveformSelector noiseTypeSelector;

    // Envelope section
    int selectedEnvTab = 0;
    EnvelopeEditorComponent envelopeEditor;
    EnvAssignComponent envAssign;

    // Aftertouch
    KnobComponent atDampen, atRetrigger;

    // Mixer
    KnobComponent volumeKnob, panKnob;
    juce::TextButton muteButton { "M" }, soloButton { "S" };
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> muteAttachment, soloAttachment;

    // Trigger pad
    TriggerPadComponent triggerPad;

    // Helper to connect the envelope editor to the currently selected envelope tab
    void updateEnvelopeConnection();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VoiceStripComponent)
};
