#pragma once
#include <juce_audio_processors/juce_audio_processors.h>

namespace ParamIDs
{
    inline juce::String partParam (int partIndex, const juce::String& suffix)
    {
        return "p" + juce::String (partIndex + 1) + "_" + suffix;
    }

    inline juce::String envParam (int partIndex, int envIndex, const juce::String& suffix)
    {
        return "p" + juce::String (partIndex + 1) + "_env" + juce::String (envIndex + 1) + "_" + suffix;
    }

    // Source suffixes
    const juce::String S1_LEVEL  = "s1_lvl";
    const juce::String S1_TUNE   = "s1_tune";
    const juce::String S1_START  = "s1_start";
    const juce::String S2_LEVEL  = "s2_lvl";
    const juce::String S2_TUNE   = "s2_tune";
    const juce::String S2_START  = "s2_start";
    const juce::String SYN_WAVE  = "syn_wave";
    const juce::String SYN_LEVEL = "syn_lvl";
    const juce::String SYN_TUNE  = "syn_tune";
    const juce::String SYN_PW    = "syn_pw";
    const juce::String SYN_FREQ  = "syn_freq";
    const juce::String NSE_TYPE  = "nse_type";
    const juce::String NSE_LEVEL = "nse_lvl";
    const juce::String NSE_FILT  = "nse_filt";
    const juce::String NSE_TONE  = "nse_tone";

    // Envelope ADSR suffixes (used with envParam)
    const juce::String ENV_A = "a";
    const juce::String ENV_D = "d";
    const juce::String ENV_S = "s";
    const juce::String ENV_R = "r";

    // Envelope assignment
    const juce::String EA_S1  = "ea_s1";
    const juce::String EA_S2  = "ea_s2";
    const juce::String EA_SYN = "ea_syn";
    const juce::String EA_NSE = "ea_nse";

    // Aftertouch
    const juce::String AT_DAMP   = "at_damp";
    const juce::String AT_RETRIG = "at_retrig";

    // Mix
    const juce::String VOL  = "vol";
    const juce::String PAN  = "pan";
    const juce::String MUTE = "mute";
    const juce::String SOLO = "solo";
}

juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
