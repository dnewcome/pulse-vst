#include "PluginProcessor.h"
#include "PluginEditor.h"

PulsePluginProcessor::PulsePluginProcessor()
    : AudioProcessor (BusesProperties()
                          .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "PARAMETERS", createParameterLayout())
{
    formatManager.registerBasicFormats();
    engine.initParameters (apvts);
}

PulsePluginProcessor::~PulsePluginProcessor() {}

void PulsePluginProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    engine.prepare (sampleRate, samplesPerBlock);
}

void PulsePluginProcessor::releaseResources() {}

bool PulsePluginProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;
    return true;
}

void PulsePluginProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();
    engine.processBlock (buffer, midiMessages);
}

juce::AudioProcessorEditor* PulsePluginProcessor::createEditor()
{
    return new PulsePluginEditor (*this);
}

void PulsePluginProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();

    // Save sample file paths
    for (int i = 0; i < Constants::NUM_PARTS; ++i)
    {
        auto& voice = engine.getVoice (i);
        state.setProperty ("p" + juce::String (i + 1) + "_s1_path",
                           voice.getSample1().getFilePath(), nullptr);
        state.setProperty ("p" + juce::String (i + 1) + "_s2_path",
                           voice.getSample2().getFilePath(), nullptr);
    }

    if (auto xml = state.createXml())
        copyXmlToBinary (*xml, destData);
}

void PulsePluginProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary (data, sizeInBytes))
    {
        auto state = juce::ValueTree::fromXml (*xml);
        if (state.isValid())
        {
            apvts.replaceState (state);

            // Restore sample paths
            for (int i = 0; i < Constants::NUM_PARTS; ++i)
            {
                auto& voice = engine.getVoice (i);
                auto s1Path = state.getProperty ("p" + juce::String (i + 1) + "_s1_path").toString();
                auto s2Path = state.getProperty ("p" + juce::String (i + 1) + "_s2_path").toString();

                voice.getSample1().loadFromPath (s1Path, formatManager);
                voice.getSample2().loadFromPath (s2Path, formatManager);
            }
        }
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PulsePluginProcessor();
}
