cmake_minimum_required(VERSION 3.22)
project(PulseDrumSynth VERSION 0.1.0)

add_subdirectory(JUCE)

juce_add_plugin(PulseDrumSynth
    COMPANY_NAME "Pulse"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE Plse
    PLUGIN_CODE Plds
    FORMATS VST3 AU Standalone
    PRODUCT_NAME "PULSE Drum Synth"
)

target_sources(PulseDrumSynth PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/Utility/Parameters.cpp
    Source/DSP/Envelope.cpp
    Source/DSP/EnvelopeBank.cpp
    Source/DSP/SynthOscillator.cpp
    Source/DSP/NoiseGenerator.cpp
    Source/DSP/SamplePlayer.cpp
    Source/DSP/AftertouchProcessor.cpp
    Source/DSP/DrumVoice.cpp
    Source/DSP/DrumEngine.cpp
    Source/GUI/KnobComponent.cpp
    Source/GUI/EnvelopeEditorComponent.cpp
    Source/GUI/SampleSlotComponent.cpp
    Source/GUI/WaveformSelector.cpp
    Source/GUI/TriggerPadComponent.cpp
    Source/GUI/EnvAssignComponent.cpp
    Source/GUI/VoiceStripComponent.cpp
    Source/GUI/HeaderComponent.cpp
)

target_compile_definitions(PulseDrumSynth
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_DISPLAY_SPLASH_SCREEN=0
        JUCE_MODAL_LOOPS_PERMITTED=1
)

target_compile_features(PulseDrumSynth PUBLIC cxx_std_17)

target_link_libraries(PulseDrumSynth
    PRIVATE
        juce::juce_audio_utils
        juce::juce_audio_processors
        juce::juce_audio_formats
        juce::juce_dsp
        juce::juce_gui_extra
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags
)
