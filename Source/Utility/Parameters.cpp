#include "Parameters.h"
#include "Constants.h"

juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    for (int i = 0; i < Constants::NUM_PARTS; ++i)
    {
        auto partGroup = std::make_unique<juce::AudioProcessorParameterGroup>(
            "part" + juce::String (i + 1), "Part " + juce::String (i + 1), "|");

        auto pid = [&](const juce::String& suffix) {
            return juce::ParameterID { ParamIDs::partParam (i, suffix), 1 };
        };
        auto eid = [&](int envIdx, const juce::String& suffix) {
            return juce::ParameterID { ParamIDs::envParam (i, envIdx, suffix), 1 };
        };
        auto pname = [&](const juce::String& name) {
            return juce::String (Constants::PART_NAMES[i]) + " " + name;
        };

        // --- Sample 1 ---
        partGroup->addChild (std::make_unique<juce::AudioParameterFloat>(
            pid (ParamIDs::S1_LEVEL), pname ("S1 Level"),
            juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.8f));
        partGroup->addChild (std::make_unique<juce::AudioParameterFloat>(
            pid (ParamIDs::S1_TUNE), pname ("S1 Tune"),
            juce::NormalisableRange<float> (-24.0f, 24.0f, 0.01f), 0.0f));
        partGroup->addChild (std::make_unique<juce::AudioParameterFloat>(
            pid (ParamIDs::S1_START), pname ("S1 Start"),
            juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.0f));

        // --- Sample 2 ---
        partGroup->addChild (std::make_unique<juce::AudioParameterFloat>(
            pid (ParamIDs::S2_LEVEL), pname ("S2 Level"),
            juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.5f));
        partGroup->addChild (std::make_unique<juce::AudioParameterFloat>(
            pid (ParamIDs::S2_TUNE), pname ("S2 Tune"),
            juce::NormalisableRange<float> (-24.0f, 24.0f, 0.01f), 0.0f));
        partGroup->addChild (std::make_unique<juce::AudioParameterFloat>(
            pid (ParamIDs::S2_START), pname ("S2 Start"),
            juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.0f));

        // --- Synth Osc ---
        partGroup->addChild (std::make_unique<juce::AudioParameterChoice>(
            pid (ParamIDs::SYN_WAVE), pname ("Synth Wave"),
            juce::StringArray { "Sine", "Triangle", "Saw", "Square" }, 0));
        partGroup->addChild (std::make_unique<juce::AudioParameterFloat>(
            pid (ParamIDs::SYN_LEVEL), pname ("Synth Level"),
            juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), i == 0 ? 0.6f : 0.3f));
        partGroup->addChild (std::make_unique<juce::AudioParameterFloat>(
            pid (ParamIDs::SYN_TUNE), pname ("Synth Tune"),
            juce::NormalisableRange<float> (-24.0f, 24.0f, 0.01f), 0.0f));
        partGroup->addChild (std::make_unique<juce::AudioParameterFloat>(
            pid (ParamIDs::SYN_PW), pname ("Synth PW"),
            juce::NormalisableRange<float> (0.05f, 0.95f, 0.01f), 0.5f));
        partGroup->addChild (std::make_unique<juce::AudioParameterFloat>(
            pid (ParamIDs::SYN_FREQ), pname ("Synth Freq"),
            juce::NormalisableRange<float> (20.0f, 2000.0f, 0.1f, 0.3f), i == 0 ? 60.0f : 200.0f));

        // --- Noise ---
        partGroup->addChild (std::make_unique<juce::AudioParameterChoice>(
            pid (ParamIDs::NSE_TYPE), pname ("Noise Type"),
            juce::StringArray { "White", "Pink", "Metallic" }, 0));
        partGroup->addChild (std::make_unique<juce::AudioParameterFloat>(
            pid (ParamIDs::NSE_LEVEL), pname ("Noise Level"),
            juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), i == 2 ? 0.7f : 0.15f));
        partGroup->addChild (std::make_unique<juce::AudioParameterFloat>(
            pid (ParamIDs::NSE_FILT), pname ("Noise Filter"),
            juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.5f));
        partGroup->addChild (std::make_unique<juce::AudioParameterFloat>(
            pid (ParamIDs::NSE_TONE), pname ("Noise Tone"),
            juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.5f));

        // --- 4 Envelopes ---
        for (int e = 0; e < Constants::NUM_ENVELOPES_PER_PART; ++e)
        {
            float defA = 0.01f, defD = 0.3f, defS = 0.5f, defR = 0.4f;
            if (e == 1) { defD = 0.15f; defS = 0.2f; }
            if (e == 2) { defA = 0.001f; defD = 0.5f; defS = 0.0f; defR = 0.1f; }

            partGroup->addChild (std::make_unique<juce::AudioParameterFloat>(
                eid (e, ParamIDs::ENV_A), pname ("Env" + juce::String (e + 1) + " A"),
                juce::NormalisableRange<float> (0.001f, 2.0f, 0.001f, 0.3f), defA));
            partGroup->addChild (std::make_unique<juce::AudioParameterFloat>(
                eid (e, ParamIDs::ENV_D), pname ("Env" + juce::String (e + 1) + " D"),
                juce::NormalisableRange<float> (0.01f, 2.0f, 0.001f, 0.3f), defD));
            partGroup->addChild (std::make_unique<juce::AudioParameterFloat>(
                eid (e, ParamIDs::ENV_S), pname ("Env" + juce::String (e + 1) + " S"),
                juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), defS));
            partGroup->addChild (std::make_unique<juce::AudioParameterFloat>(
                eid (e, ParamIDs::ENV_R), pname ("Env" + juce::String (e + 1) + " R"),
                juce::NormalisableRange<float> (0.01f, 2.0f, 0.001f, 0.3f), defR));
        }

        // --- Envelope Assignments (which env index 0-3 controls each source) ---
        partGroup->addChild (std::make_unique<juce::AudioParameterChoice>(
            pid (ParamIDs::EA_S1), pname ("Env Assign S1"),
            juce::StringArray { "Env 1", "Env 2", "Env 3", "Env 4" }, 0));
        partGroup->addChild (std::make_unique<juce::AudioParameterChoice>(
            pid (ParamIDs::EA_S2), pname ("Env Assign S2"),
            juce::StringArray { "Env 1", "Env 2", "Env 3", "Env 4" }, 0));
        partGroup->addChild (std::make_unique<juce::AudioParameterChoice>(
            pid (ParamIDs::EA_SYN), pname ("Env Assign Synth"),
            juce::StringArray { "Env 1", "Env 2", "Env 3", "Env 4" }, 1));
        partGroup->addChild (std::make_unique<juce::AudioParameterChoice>(
            pid (ParamIDs::EA_NSE), pname ("Env Assign Noise"),
            juce::StringArray { "Env 1", "Env 2", "Env 3", "Env 4" }, 2));

        // --- Aftertouch ---
        partGroup->addChild (std::make_unique<juce::AudioParameterFloat>(
            pid (ParamIDs::AT_DAMP), pname ("AT Dampen"),
            juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.7f));
        partGroup->addChild (std::make_unique<juce::AudioParameterFloat>(
            pid (ParamIDs::AT_RETRIG), pname ("AT Retrigger"),
            juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.3f));

        // --- Mix ---
        partGroup->addChild (std::make_unique<juce::AudioParameterFloat>(
            pid (ParamIDs::VOL), pname ("Volume"),
            juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.8f));
        partGroup->addChild (std::make_unique<juce::AudioParameterFloat>(
            pid (ParamIDs::PAN), pname ("Pan"),
            juce::NormalisableRange<float> (-1.0f, 1.0f, 0.01f), 0.0f));
        partGroup->addChild (std::make_unique<juce::AudioParameterBool>(
            pid (ParamIDs::MUTE), pname ("Mute"), false));
        partGroup->addChild (std::make_unique<juce::AudioParameterBool>(
            pid (ParamIDs::SOLO), pname ("Solo"), false));

        layout.add (std::move (partGroup));
    }

    return layout;
}
