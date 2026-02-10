#include "VoiceStripComponent.h"
#include "../PluginProcessor.h"

VoiceStripComponent::VoiceStripComponent (int partIdx, juce::Colour accentColour,
                                           juce::AudioProcessorValueTreeState& apvtsRef,
                                           PulsePluginProcessor& proc)
    : partIndex (partIdx),
      accent (accentColour),
      apvts (apvtsRef),
      processor (proc),
      sample1Slot ("SAMPLE 1", accent),
      sample2Slot ("SAMPLE 2", accent),
      synthLevel ("Level", accent),
      synthTune ("Tune", accent, true),
      synthPW ("PW", accent),
      waveformSelector ({ "Sine", "Tri", "Saw", "Sqr" }, accent),
      noiseLevel ("Level", accent),
      noiseFilter ("Filter", accent),
      noiseTone ("Tone", accent),
      noiseTypeSelector ({ "White", "Pink", "Metal" }, accent),
      envelopeEditor (accent),
      envAssign (accent),
      atDampen ("Dampen", accent),
      atRetrigger ("Retrig", accent),
      volumeKnob ("Vol", accent),
      panKnob ("Pan", accent, true),
      triggerPad (Constants::PART_NAMES[partIdx], accent)
{
    auto pid = [&](const juce::String& suffix) {
        return ParamIDs::partParam (partIndex, suffix);
    };

    // Add all children
    addAndMakeVisible (sample1Slot);
    addAndMakeVisible (sample2Slot);
    addAndMakeVisible (synthLevel);
    addAndMakeVisible (synthTune);
    addAndMakeVisible (synthPW);
    addAndMakeVisible (waveformSelector);
    addAndMakeVisible (noiseLevel);
    addAndMakeVisible (noiseFilter);
    addAndMakeVisible (noiseTone);
    addAndMakeVisible (noiseTypeSelector);
    addAndMakeVisible (envelopeEditor);
    addAndMakeVisible (envAssign);
    addAndMakeVisible (atDampen);
    addAndMakeVisible (atRetrigger);
    addAndMakeVisible (volumeKnob);
    addAndMakeVisible (panKnob);
    addAndMakeVisible (muteButton);
    addAndMakeVisible (soloButton);
    addAndMakeVisible (triggerPad);

    // --- Attach parameters ---

    // Sample slots
    sample1Slot.onSampleLoaded = [this](const juce::File& file) {
        processor.getEngine().getVoice (partIndex).getSample1().loadSample (file, processor.getFormatManager());
    };
    sample2Slot.onSampleLoaded = [this](const juce::File& file) {
        processor.getEngine().getVoice (partIndex).getSample2().loadSample (file, processor.getFormatManager());
    };

    // Show existing sample names
    auto& voice = processor.getEngine().getVoice (partIndex);
    sample1Slot.setSampleName (voice.getSample1().getFileName());
    sample2Slot.setSampleName (voice.getSample2().getFileName());

    // Synth knobs
    synthLevel.setAttachment (std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        apvts, pid (ParamIDs::SYN_LEVEL), synthLevel.getSlider()));
    synthTune.setAttachment (std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        apvts, pid (ParamIDs::SYN_TUNE), synthTune.getSlider()));
    synthPW.setAttachment (std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        apvts, pid (ParamIDs::SYN_PW), synthPW.getSlider()));

    // Waveform selector
    waveformSelector.setAttachment (std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (
        apvts, pid (ParamIDs::SYN_WAVE), waveformSelector.getComboBox()));

    // Noise knobs
    noiseLevel.setAttachment (std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        apvts, pid (ParamIDs::NSE_LEVEL), noiseLevel.getSlider()));
    noiseFilter.setAttachment (std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        apvts, pid (ParamIDs::NSE_FILT), noiseFilter.getSlider()));
    noiseTone.setAttachment (std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        apvts, pid (ParamIDs::NSE_TONE), noiseTone.getSlider()));

    // Noise type selector
    noiseTypeSelector.setAttachment (std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (
        apvts, pid (ParamIDs::NSE_TYPE), noiseTypeSelector.getComboBox()));

    // Envelope editor (connect to env 1 by default)
    updateEnvelopeConnection();

    // Env assign
    envAssign.connectToParameters (apvts,
                                    pid (ParamIDs::EA_S1),
                                    pid (ParamIDs::EA_S2),
                                    pid (ParamIDs::EA_SYN),
                                    pid (ParamIDs::EA_NSE));

    // Aftertouch
    atDampen.setAttachment (std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        apvts, pid (ParamIDs::AT_DAMP), atDampen.getSlider()));
    atRetrigger.setAttachment (std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        apvts, pid (ParamIDs::AT_RETRIG), atRetrigger.getSlider()));

    // Volume / Pan
    volumeKnob.setAttachment (std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        apvts, pid (ParamIDs::VOL), volumeKnob.getSlider()));
    panKnob.setAttachment (std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        apvts, pid (ParamIDs::PAN), panKnob.getSlider()));

    // Mute / Solo
    muteButton.setClickingTogglesState (true);
    soloButton.setClickingTogglesState (true);
    muteAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (
        apvts, pid (ParamIDs::MUTE), muteButton);
    soloAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (
        apvts, pid (ParamIDs::SOLO), soloButton);

    // Trigger pad
    triggerPad.onTrigger = [this](float velocity) {
        processor.getEngine().triggerVoiceFromGUI (partIndex, velocity);
    };
}

void VoiceStripComponent::updateEnvelopeConnection()
{
    envelopeEditor.connectToParameters (
        apvts,
        ParamIDs::envParam (partIndex, selectedEnvTab, ParamIDs::ENV_A),
        ParamIDs::envParam (partIndex, selectedEnvTab, ParamIDs::ENV_D),
        ParamIDs::envParam (partIndex, selectedEnvTab, ParamIDs::ENV_S),
        ParamIDs::envParam (partIndex, selectedEnvTab, ParamIDs::ENV_R));

    envAssign.setSelectedEnvelope (selectedEnvTab);
}

void VoiceStripComponent::setEnvelopeTab (int index)
{
    if (index == selectedEnvTab) return;
    selectedEnvTab = juce::jlimit (0, 3, index);
    updateEnvelopeConnection();
    repaint();
}

void VoiceStripComponent::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    // Strip background
    g.setColour (Colours::strip);
    g.fillRoundedRectangle (bounds, 8.0f);
    g.setColour (Colours::stripBorder);
    g.drawRoundedRectangle (bounds, 8.0f, 1.0f);

    auto inner = bounds.reduced (10);
    auto monoFont = juce::Font::getDefaultMonospacedFontName();

    // Header
    auto headerArea = inner.removeFromTop (24);
    g.setColour (accent);
    g.setFont (juce::Font (monoFont, 12.0f, juce::Font::bold));
    g.drawText (Constants::PART_NAMES[partIndex], headerArea, juce::Justification::centredLeft);
    g.setColour (Colours::textMuted);
    g.setFont (juce::Font (monoFont, 8.0f, juce::Font::plain));
    g.drawText ("CH " + juce::String (partIndex + 1), headerArea, juce::Justification::centredRight);
    g.setColour (Colours::stripBorder);
    g.drawLine (inner.getX(), headerArea.getBottom(), inner.getRight(), headerArea.getBottom(), 1.0f);

    // Section labels
    auto paintSectionLabel = [&](float y, const juce::String& text)
    {
        g.setColour (accent);
        g.setFont (juce::Font (monoFont, 9.0f, juce::Font::bold));
        g.drawText (text, inner.getX(), (int) y, (int) inner.getWidth(), 14, juce::Justification::centredLeft);
    };

    // Sources label
    paintSectionLabel (inner.getY() + 28, "SOURCES");

    // Envelopes label
    float envLabelY = noiseTypeSelector.getBottom() + 4.0f;
    paintSectionLabel (envLabelY, "ENVELOPES");

    // Envelope tabs
    float tabY = envLabelY + 16;
    float tabW = (inner.getWidth()) / 4.0f;
    for (int i = 0; i < 4; ++i)
    {
        auto tabBounds = juce::Rectangle<float> (inner.getX() + i * tabW + 1, tabY, tabW - 3, 16);
        bool selected = (i == selectedEnvTab);

        if (selected)
        {
            g.setColour (accent.withAlpha (0.2f));
            g.fillRoundedRectangle (tabBounds, 3.0f);
            g.setColour (accent.withAlpha (0.4f));
            g.drawRoundedRectangle (tabBounds, 3.0f, 1.0f);
            g.setColour (accent);
        }
        else
        {
            g.setColour (Colours::stripBorder);
            g.drawRoundedRectangle (tabBounds, 3.0f, 1.0f);
            g.setColour (Colours::textMuted);
        }

        g.setFont (juce::Font (monoFont, 8.0f, juce::Font::plain));
        g.drawText ("ENV " + juce::String (i + 1), tabBounds, juce::Justification::centred);
    }

    // Aftertouch label
    g.setColour (Colours::textMuted);
    g.setFont (juce::Font (monoFont, 8.0f, juce::Font::plain));

    // Mute/Solo styling
    auto styleMuteSolo = [this](juce::TextButton& btn, const juce::String& /*label*/, bool active, juce::Colour colour)
    {
        btn.setColour (juce::TextButton::buttonColourId,
                       active ? colour.withAlpha (0.2f) : Colours::strip);
        btn.setColour (juce::TextButton::textColourOnId, active ? colour : Colours::textMuted);
        btn.setColour (juce::TextButton::textColourOffId, Colours::textMuted);
    };

    styleMuteSolo (muteButton, "M", muteButton.getToggleState(), juce::Colour (0xffff4444));
    styleMuteSolo (soloButton, "S", soloButton.getToggleState(), juce::Colour (0xffffaa22));
}

void VoiceStripComponent::resized()
{
    auto bounds = getLocalBounds().reduced (10);
    bounds.removeFromTop (24); // header
    bounds.removeFromTop (4);  // gap

    // Sources section label
    bounds.removeFromTop (16);

    // Sample slots
    sample1Slot.setBounds (bounds.removeFromTop (32));
    bounds.removeFromTop (4);
    sample2Slot.setBounds (bounds.removeFromTop (32));
    bounds.removeFromTop (4);

    // Synth OSC section
    {
        auto synthArea = bounds.removeFromTop (80);
        // Label + background drawn in paint

        auto knobRow = synthArea.removeFromTop (48);
        int knobW = knobRow.getWidth() / 3;
        synthLevel.setBounds (knobRow.removeFromLeft (knobW));
        synthTune.setBounds (knobRow.removeFromLeft (knobW));
        synthPW.setBounds (knobRow);

        synthArea.removeFromTop (4);
        waveformSelector.setBounds (synthArea.removeFromTop (18));
    }
    bounds.removeFromTop (4);

    // Noise section
    {
        auto noiseArea = bounds.removeFromTop (80);
        auto knobRow = noiseArea.removeFromTop (48);
        int knobW = knobRow.getWidth() / 3;
        noiseLevel.setBounds (knobRow.removeFromLeft (knobW));
        noiseFilter.setBounds (knobRow.removeFromLeft (knobW));
        noiseTone.setBounds (knobRow);

        noiseArea.removeFromTop (4);
        noiseTypeSelector.setBounds (noiseArea.removeFromTop (18));
    }
    bounds.removeFromTop (4);

    // Envelope section
    bounds.removeFromTop (16); // "ENVELOPES" label
    envTabArea = bounds.removeFromTop (20).toFloat(); // store for hit testing
    bounds.removeFromTop (4);
    envelopeEditor.setBounds (bounds.removeFromTop (80));
    bounds.removeFromTop (4);
    envAssign.setBounds (bounds.removeFromTop (34));
    bounds.removeFromTop (4);

    // Aftertouch section
    {
        auto atArea = bounds.removeFromTop (52);
        int knobW = atArea.getWidth() / 2;
        atDampen.setBounds (atArea.removeFromLeft (knobW));
        atRetrigger.setBounds (atArea);
    }
    bounds.removeFromTop (4);

    // Mixer section
    {
        auto mixArea = bounds.removeFromTop (48);
        int knobW = 40;
        volumeKnob.setBounds (mixArea.removeFromLeft (knobW));
        panKnob.setBounds (mixArea.removeFromLeft (knobW));
        mixArea.removeFromLeft (8);
        muteButton.setBounds (mixArea.removeFromLeft (20).withHeight (20).withY (mixArea.getY() + 10));
        mixArea.removeFromLeft (3);
        soloButton.setBounds (mixArea.removeFromLeft (20).withHeight (20).withY (mixArea.getY() + 10));
    }
    bounds.removeFromTop (4);

    // Trigger pad (fill remaining)
    triggerPad.setBounds (bounds.withHeight (juce::jmin (bounds.getHeight(), 70)));
}

void VoiceStripComponent::mouseDown (const juce::MouseEvent& e)
{
    // Check if click is in the envelope tab area
    if (envTabArea.contains (e.position))
    {
        float tabW = envTabArea.getWidth() / 4.0f;
        int clicked = (int) ((e.position.x - envTabArea.getX()) / tabW);
        clicked = juce::jlimit (0, 3, clicked);
        setEnvelopeTab (clicked);
    }
}
