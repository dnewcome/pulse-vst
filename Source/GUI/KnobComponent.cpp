#include "KnobComponent.h"

KnobComponent::KnobComponent (const juce::String& label, juce::Colour accentColour, bool bipolar)
    : labelText (label), accent (accentColour), isBipolar (bipolar)
{
    slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    slider.setColour (juce::Slider::rotarySliderFillColourId, accent);
    slider.setColour (juce::Slider::rotarySliderOutlineColourId, Colours::knobTrack);
    addAndMakeVisible (slider);
}

void KnobComponent::resized()
{
    auto area = getLocalBounds();
    int labelH = 12;
    slider.setBounds (area.removeFromTop (area.getHeight() - labelH));
}

void KnobComponent::paint (juce::Graphics& g)
{
    auto area = getLocalBounds();
    auto knobArea = area.removeFromTop (area.getHeight() - 12).toFloat();
    auto labelArea = area.toFloat();

    float size = juce::jmin (knobArea.getWidth(), knobArea.getHeight());
    auto centre = knobArea.getCentre();
    float r = size / 2.0f - 4.0f;

    // Background circle
    g.setColour (Colours::knobBg);
    g.fillEllipse (centre.x - r, centre.y - r, r * 2, r * 2);
    g.setColour (Colours::knobTrack);
    g.drawEllipse (centre.x - r, centre.y - r, r * 2, r * 2, 2.0f);

    // Arc angles
    float startAngle = juce::MathConstants<float>::pi * 1.25f;   // -135 deg from top
    float endAngle = juce::MathConstants<float>::pi * 2.75f;     // +135 deg from top
    float arcRadius = r - 2.0f;

    // Track arc
    juce::Path trackArc;
    trackArc.addCentredArc (centre.x, centre.y, arcRadius, arcRadius, 0.0f,
                            startAngle, endAngle, true);
    g.setColour (Colours::knobTrack);
    g.strokePath (trackArc, juce::PathStrokeType (3.0f, juce::PathStrokeType::curved,
                                                   juce::PathStrokeType::rounded));

    // Value arc
    double norm = slider.valueToProportionOfLength (slider.getValue());
    float valueAngle = startAngle + (float) norm * (endAngle - startAngle);

    juce::Path valueArc;
    if (isBipolar)
    {
        float midAngle = (startAngle + endAngle) * 0.5f;
        if (valueAngle > midAngle)
            valueArc.addCentredArc (centre.x, centre.y, arcRadius, arcRadius, 0.0f,
                                    midAngle, valueAngle, true);
        else
            valueArc.addCentredArc (centre.x, centre.y, arcRadius, arcRadius, 0.0f,
                                    valueAngle, midAngle, true);
    }
    else
    {
        valueArc.addCentredArc (centre.x, centre.y, arcRadius, arcRadius, 0.0f,
                                startAngle, valueAngle, true);
    }

    g.setColour (accent);
    g.strokePath (valueArc, juce::PathStrokeType (3.0f, juce::PathStrokeType::curved,
                                                   juce::PathStrokeType::rounded));

    // Indicator line
    float lineR = r - 8.0f;
    float lx = centre.x + lineR * std::cos (valueAngle);
    float ly = centre.y + lineR * std::sin (valueAngle);
    g.setColour (accent);
    g.drawLine (centre.x, centre.y, lx, ly, 2.0f);

    // Label
    g.setColour (Colours::textDim);
    g.setFont (juce::Font (juce::FontOptions (juce::Font::getDefaultMonospacedFontName(), 9.0f, juce::Font::plain)));
    g.drawText (labelText.toUpperCase(), labelArea, juce::Justification::centredTop);
}
