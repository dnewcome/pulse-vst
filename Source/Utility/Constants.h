#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

namespace Constants
{
    constexpr int NUM_PARTS = 4;
    constexpr int NUM_ENVELOPES_PER_PART = 4;
    constexpr int NUM_SOURCES_PER_PART = 4;
    constexpr float MAX_SAMPLE_LENGTH_SECS = 30.0f;
    constexpr int DEFAULT_BASE_NOTE = 60;

    // GM drum map notes for channel 10 mode
    constexpr int GM_KICK  = 36;
    constexpr int GM_SNARE = 38;
    constexpr int GM_HAT   = 42;
    constexpr int GM_PERC  = 46;

    inline const int GM_NOTE_MAP[NUM_PARTS] = { GM_KICK, GM_SNARE, GM_HAT, GM_PERC };
    inline const char* PART_NAMES[NUM_PARTS] = { "KICK", "SNARE", "HAT", "PERC" };
}

namespace Colours
{
    // Matching the JSX mockup
    inline const juce::Colour bg          { 0xff0a0a0c };
    inline const juce::Colour surface     { 0xff121216 };
    inline const juce::Colour surfaceAlt  { 0xff18181e };
    inline const juce::Colour strip       { 0xff14141a };
    inline const juce::Colour stripBorder { 0xff222230 };
    inline const juce::Colour text        { 0xffe8e8ec };
    inline const juce::Colour textDim     { 0xff7a7a88 };
    inline const juce::Colour textMuted   { 0xff44444e };
    inline const juce::Colour knobBg      { 0xff1e1e26 };
    inline const juce::Colour knobTrack   { 0xff2a2a36 };
    inline const juce::Colour padIdle     { 0xff1a1a22 };
    inline const juce::Colour padActive   { 0xff2a2a36 };
    inline const juce::Colour led         { 0xff22dd66 };

    inline const juce::Colour accent[Constants::NUM_PARTS] = {
        juce::Colour(0xffff4d4d),  // red - kick
        juce::Colour(0xffffaa22),  // orange - snare
        juce::Colour(0xff22ddaa),  // green - hat
        juce::Colour(0xff4488ff),  // blue - perc
    };
}
