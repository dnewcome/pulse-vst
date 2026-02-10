#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "../Utility/Constants.h"

class SampleSlotComponent : public juce::Component,
                             public juce::FileDragAndDropTarget
{
public:
    SampleSlotComponent (const juce::String& label, juce::Colour accentColour);

    void paint (juce::Graphics& g) override;
    void mouseDown (const juce::MouseEvent& e) override;

    bool isInterestedInFileDrag (const juce::StringArray& files) override;
    void fileDragEnter (const juce::StringArray&, int, int) override;
    void fileDragExit (const juce::StringArray&) override;
    void filesDropped (const juce::StringArray& files, int, int) override;

    void setSampleName (const juce::String& name);

    std::function<void (const juce::File&)> onSampleLoaded;

private:
    juce::String labelText;
    juce::String sampleName;
    juce::Colour accent;
    bool isLoaded = false;
    bool isDragOver = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SampleSlotComponent)
};
