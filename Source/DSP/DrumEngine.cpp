#include "DrumEngine.h"

void DrumEngine::prepare (double sampleRate, int maxBlockSize)
{
    for (auto& voice : voices)
        voice.prepare (sampleRate, maxBlockSize);
}

void DrumEngine::initParameters (juce::AudioProcessorValueTreeState& apvts)
{
    for (int i = 0; i < Constants::NUM_PARTS; ++i)
    {
        DrumVoice::ParameterRefs refs;

        auto get = [&](const juce::String& suffix) {
            return apvts.getRawParameterValue (ParamIDs::partParam (i, suffix));
        };
        auto getEnv = [&](int e, const juce::String& suffix) {
            return apvts.getRawParameterValue (ParamIDs::envParam (i, e, suffix));
        };

        refs.s1Level = get (ParamIDs::S1_LEVEL);
        refs.s1Tune  = get (ParamIDs::S1_TUNE);
        refs.s1Start = get (ParamIDs::S1_START);
        refs.s2Level = get (ParamIDs::S2_LEVEL);
        refs.s2Tune  = get (ParamIDs::S2_TUNE);
        refs.s2Start = get (ParamIDs::S2_START);
        refs.synWave  = get (ParamIDs::SYN_WAVE);
        refs.synLevel = get (ParamIDs::SYN_LEVEL);
        refs.synTune  = get (ParamIDs::SYN_TUNE);
        refs.synPW    = get (ParamIDs::SYN_PW);
        refs.synFreq  = get (ParamIDs::SYN_FREQ);
        refs.nseType  = get (ParamIDs::NSE_TYPE);
        refs.nseLevel = get (ParamIDs::NSE_LEVEL);
        refs.nseFilt  = get (ParamIDs::NSE_FILT);
        refs.nseTone  = get (ParamIDs::NSE_TONE);
        refs.atDamp   = get (ParamIDs::AT_DAMP);
        refs.atRetrig = get (ParamIDs::AT_RETRIG);
        refs.volume   = get (ParamIDs::VOL);
        refs.pan      = get (ParamIDs::PAN);
        refs.mute     = get (ParamIDs::MUTE);
        refs.solo     = get (ParamIDs::SOLO);

        for (int e = 0; e < Constants::NUM_ENVELOPES_PER_PART; ++e)
        {
            refs.envA[e] = getEnv (e, ParamIDs::ENV_A);
            refs.envD[e] = getEnv (e, ParamIDs::ENV_D);
            refs.envS[e] = getEnv (e, ParamIDs::ENV_S);
            refs.envR[e] = getEnv (e, ParamIDs::ENV_R);
        }

        refs.eaS1  = get (ParamIDs::EA_S1);
        refs.eaS2  = get (ParamIDs::EA_S2);
        refs.eaSyn = get (ParamIDs::EA_SYN);
        refs.eaNse = get (ParamIDs::EA_NSE);

        voices[(size_t) i].setParameterRefs (refs);
    }
}

void DrumEngine::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    int numSamples = buffer.getNumSamples();

    // Process GUI triggers
    int numGuiTriggers = guiTriggerCount.exchange (0);
    for (int i = 0; i < numGuiTriggers; ++i)
    {
        auto& t = guiTriggerBuffer[(size_t) i];
        if (t.partIndex >= 0 && t.partIndex < Constants::NUM_PARTS)
            voices[(size_t) t.partIndex].trigger (t.velocity);
    }

    // Process MIDI
    for (const auto metadata : midi)
    {
        auto msg = metadata.getMessage();
        handleMidiEvent (msg);
    }

    // Determine solo state
    bool anySolo = false;
    for (auto& voice : voices)
        if (voice.isSoloed())
            anySolo = true;

    // Process all voices
    for (int i = 0; i < Constants::NUM_PARTS; ++i)
    {
        auto& voice = voices[(size_t) i];

        if (voice.isMuted())
            continue;
        if (anySolo && ! voice.isSoloed())
            continue;

        voice.processBlock (buffer, numSamples);
    }
}

void DrumEngine::handleMidiEvent (const juce::MidiMessage& msg)
{
    if (msg.isNoteOn())
    {
        int channel = msg.getChannel(); // 1-based
        float velocity = msg.getFloatVelocity();

        if (channel >= 1 && channel <= 4)
        {
            voices[(size_t) (channel - 1)].trigger (velocity);
        }
        else if (channel == 10)
        {
            int partIdx = noteToPartIndex (msg.getNoteNumber());
            if (partIdx >= 0)
                voices[(size_t) partIdx].trigger (velocity);
        }
    }
    else if (msg.isNoteOff())
    {
        int channel = msg.getChannel();

        if (channel >= 1 && channel <= 4)
        {
            voices[(size_t) (channel - 1)].release();
        }
        else if (channel == 10)
        {
            int partIdx = noteToPartIndex (msg.getNoteNumber());
            if (partIdx >= 0)
                voices[(size_t) partIdx].release();
        }
    }
    else if (msg.isChannelPressure())
    {
        int channel = msg.getChannel();
        float pressure = msg.getChannelPressureValue() / 127.0f;

        if (channel >= 1 && channel <= 4)
            voices[(size_t) (channel - 1)].applyAftertouch (pressure);
    }
    else if (msg.isAftertouch())
    {
        int partIdx = noteToPartIndex (msg.getNoteNumber());
        float pressure = msg.getAfterTouchValue() / 127.0f;

        if (partIdx >= 0)
            voices[(size_t) partIdx].applyAftertouch (pressure);
    }
}

int DrumEngine::noteToPartIndex (int note) const
{
    for (int i = 0; i < Constants::NUM_PARTS; ++i)
        if (Constants::GM_NOTE_MAP[i] == note)
            return i;
    return -1;
}

void DrumEngine::triggerVoiceFromGUI (int partIndex, float velocity)
{
    int idx = guiTriggerCount.load();
    if (idx < (int) guiTriggerBuffer.size())
    {
        guiTriggerBuffer[(size_t) idx] = { partIndex, velocity };
        guiTriggerCount.store (idx + 1);
    }
}
