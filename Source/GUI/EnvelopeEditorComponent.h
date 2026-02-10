#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "../Utility/Constants.h"

class EnvelopeEditorComponent : public juce::Component,
                                 public juce::AudioProcessorValueTreeState::Listener
{
public:
    EnvelopeEditorComponent (juce::Colour accentColour);
    ~EnvelopeEditorComponent() override;

    void connectToParameters (juce::AudioProcessorValueTreeState& apvts,
                              const juce::String& attackID,
                              const juce::String& decayID,
                              const juce::String& sustainID,
                              const juce::String& releaseID);

    void paint (juce::Graphics& g) override;
    void mouseDown (const juce::MouseEvent& e) override;
    void mouseDrag (const juce::MouseEvent& e) override;
    void mouseUp (const juce::MouseEvent& e) override;

    void parameterChanged (const juce::String& parameterID, float newValue) override;

private:
    struct EnvPoint { float x, y; };
    std::array<EnvPoint, 5> calculatePoints() const;
    int findDragPoint (juce::Point<float> pos) const;

    juce::Colour accent;
    juce::AudioProcessorValueTreeState* apvtsPtr = nullptr;
    juce::RangedAudioParameter* attackParam = nullptr;
    juce::RangedAudioParameter* decayParam = nullptr;
    juce::RangedAudioParameter* sustainParam = nullptr;
    juce::RangedAudioParameter* releaseParam = nullptr;

    juce::String attackID, decayID, sustainID, releaseID;

    int draggingPoint = -1;
    static constexpr float padding = 8.0f;
    static constexpr float holdTime = 0.2f;
};
