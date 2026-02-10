#include "SampleSlotComponent.h"

SampleSlotComponent::SampleSlotComponent (const juce::String& label, juce::Colour accentColour)
    : labelText (label), accent (accentColour)
{
}

void SampleSlotComponent::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    g.setColour (isDragOver ? accent.withAlpha (0.1f) : Colours::surfaceAlt);
    g.fillRoundedRectangle (bounds, 4.0f);
    g.setColour (isDragOver ? accent.withAlpha (0.5f) : Colours::stripBorder);
    g.drawRoundedRectangle (bounds, 4.0f, 1.0f);

    auto monoFont = juce::Font (juce::FontOptions (juce::Font::getDefaultMonospacedFontName(), 8.0f, juce::Font::plain));

    // Label
    g.setColour (Colours::textMuted);
    g.setFont (monoFont);
    g.drawText (labelText, bounds.reduced (8, 4).removeFromTop (12), juce::Justification::centredLeft);

    // Sample name or "Drop sample..."
    auto nameArea = bounds.reduced (8, 4);
    nameArea.removeFromTop (14);

    if (isLoaded)
    {
        g.setColour (accent);
        g.setFont (juce::Font (juce::FontOptions (juce::Font::getDefaultMonospacedFontName(), 9.0f, juce::Font::plain)));
        g.drawText (sampleName, nameArea, juce::Justification::centredLeft);
    }
    else
    {
        g.setColour (Colours::textMuted);
        g.setFont (juce::Font (juce::FontOptions (juce::Font::getDefaultMonospacedFontName(), 9.0f, juce::Font::italic)));
        g.drawText ("Drop sample...", nameArea, juce::Justification::centredLeft);
    }
}

void SampleSlotComponent::mouseDown (const juce::MouseEvent&)
{
    auto chooser = std::make_shared<juce::FileChooser> (
        "Select a sample...",
        juce::File::getSpecialLocation (juce::File::userHomeDirectory),
        "*.wav;*.aif;*.aiff;*.flac;*.mp3;*.ogg");

    chooser->launchAsync (juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this, chooser] (const juce::FileChooser& fc)
        {
            auto result = fc.getResult();
            if (result.existsAsFile())
            {
                sampleName = result.getFileName();
                isLoaded = true;
                repaint();

                if (onSampleLoaded)
                    onSampleLoaded (result);
            }
        });
}

bool SampleSlotComponent::isInterestedInFileDrag (const juce::StringArray& files)
{
    for (auto& f : files)
    {
        auto ext = juce::File (f).getFileExtension().toLowerCase();
        if (ext == ".wav" || ext == ".aif" || ext == ".aiff" || ext == ".flac" || ext == ".mp3" || ext == ".ogg")
            return true;
    }
    return false;
}

void SampleSlotComponent::fileDragEnter (const juce::StringArray&, int, int)
{
    isDragOver = true;
    repaint();
}

void SampleSlotComponent::fileDragExit (const juce::StringArray&)
{
    isDragOver = false;
    repaint();
}

void SampleSlotComponent::filesDropped (const juce::StringArray& files, int, int)
{
    isDragOver = false;

    if (! files.isEmpty())
    {
        juce::File file (files[0]);
        if (file.existsAsFile())
        {
            sampleName = file.getFileName();
            isLoaded = true;
            repaint();

            if (onSampleLoaded)
                onSampleLoaded (file);
        }
    }
}

void SampleSlotComponent::setSampleName (const juce::String& name)
{
    sampleName = name;
    isLoaded = name.isNotEmpty();
    repaint();
}
