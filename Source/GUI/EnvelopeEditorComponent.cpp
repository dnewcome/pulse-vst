#include "EnvelopeEditorComponent.h"

EnvelopeEditorComponent::EnvelopeEditorComponent (juce::Colour accentColour)
    : accent (accentColour)
{
}

EnvelopeEditorComponent::~EnvelopeEditorComponent()
{
    if (apvtsPtr != nullptr)
    {
        if (attackID.isNotEmpty()) apvtsPtr->removeParameterListener (attackID, this);
        if (decayID.isNotEmpty()) apvtsPtr->removeParameterListener (decayID, this);
        if (sustainID.isNotEmpty()) apvtsPtr->removeParameterListener (sustainID, this);
        if (releaseID.isNotEmpty()) apvtsPtr->removeParameterListener (releaseID, this);
    }
}

void EnvelopeEditorComponent::connectToParameters (juce::AudioProcessorValueTreeState& apvts,
                                                     const juce::String& aID,
                                                     const juce::String& dID,
                                                     const juce::String& sID,
                                                     const juce::String& rID)
{
    // Remove old listeners if reconnecting
    if (apvtsPtr != nullptr)
    {
        if (attackID.isNotEmpty()) apvtsPtr->removeParameterListener (attackID, this);
        if (decayID.isNotEmpty()) apvtsPtr->removeParameterListener (decayID, this);
        if (sustainID.isNotEmpty()) apvtsPtr->removeParameterListener (sustainID, this);
        if (releaseID.isNotEmpty()) apvtsPtr->removeParameterListener (releaseID, this);
    }

    apvtsPtr = &apvts;
    attackID = aID;
    decayID = dID;
    sustainID = sID;
    releaseID = rID;

    attackParam = apvts.getParameter (aID);
    decayParam = apvts.getParameter (dID);
    sustainParam = apvts.getParameter (sID);
    releaseParam = apvts.getParameter (rID);

    apvts.addParameterListener (aID, this);
    apvts.addParameterListener (dID, this);
    apvts.addParameterListener (sID, this);
    apvts.addParameterListener (rID, this);

    repaint();
}

void EnvelopeEditorComponent::parameterChanged (const juce::String&, float)
{
    repaint();
}

std::array<EnvelopeEditorComponent::EnvPoint, 5> EnvelopeEditorComponent::calculatePoints() const
{
    float w = (float) getWidth();
    float h = (float) getHeight();
    float iw = w - padding * 2;
    float ih = h - padding * 2 - 12; // leave space for text

    float attack = attackParam != nullptr ? attackParam->getValue() * 2.0f : 0.01f;
    float decay = decayParam != nullptr ? decayParam->getValue() * 2.0f : 0.3f;
    float sustain = sustainParam != nullptr ? sustainParam->getValue() : 0.5f;
    float release = releaseParam != nullptr ? releaseParam->getValue() * 2.0f : 0.4f;

    // Use denormalized values
    if (attackParam) attack = attackParam->convertFrom0to1 (attackParam->getValue());
    if (decayParam) decay = decayParam->convertFrom0to1 (decayParam->getValue());
    if (sustainParam) sustain = sustainParam->convertFrom0to1 (sustainParam->getValue());
    if (releaseParam) release = releaseParam->convertFrom0to1 (releaseParam->getValue());

    float totalTime = attack + decay + holdTime + release;
    float scaleX = iw / totalTime;

    return {{
        { padding, padding + ih },
        { padding + attack * scaleX, padding },
        { padding + (attack + decay) * scaleX, padding + ih * (1.0f - sustain) },
        { padding + (attack + decay + holdTime) * scaleX, padding + ih * (1.0f - sustain) },
        { padding + (attack + decay + holdTime + release) * scaleX, padding + ih },
    }};
}

int EnvelopeEditorComponent::findDragPoint (juce::Point<float> pos) const
{
    auto pts = calculatePoints();
    const float hitRadius = 8.0f;

    for (int idx : { 1, 2, 4 })
    {
        float dx = pos.x - pts[(size_t) idx].x;
        float dy = pos.y - pts[(size_t) idx].y;
        if (dx * dx + dy * dy < hitRadius * hitRadius)
            return idx;
    }
    return -1;
}

void EnvelopeEditorComponent::paint (juce::Graphics& g)
{
    float w = (float) getWidth();
    float h = (float) getHeight();

    // Background
    g.setColour (Colours::surfaceAlt);
    g.fillRoundedRectangle (0, 0, w, h, 4.0f);
    g.setColour (Colours::stripBorder);
    g.drawRoundedRectangle (0, 0, w, h, 4.0f, 1.0f);

    auto pts = calculatePoints();
    float ih = h - padding * 2 - 12;

    // Fill under curve
    juce::Path fillPath;
    fillPath.startNewSubPath (pts[0].x, pts[0].y);
    for (int i = 1; i < 5; ++i)
        fillPath.lineTo (pts[(size_t) i].x, pts[(size_t) i].y);
    fillPath.lineTo (pts[4].x, padding + ih);
    fillPath.lineTo (padding, padding + ih);
    fillPath.closeSubPath();

    g.setGradientFill (juce::ColourGradient (accent.withAlpha (0.15f), 0, 0,
                                              accent.withAlpha (0.02f), 0, h, false));
    g.fillPath (fillPath);

    // Stroke
    juce::Path strokePath;
    strokePath.startNewSubPath (pts[0].x, pts[0].y);
    for (int i = 1; i < 5; ++i)
        strokePath.lineTo (pts[(size_t) i].x, pts[(size_t) i].y);

    g.setColour (accent);
    g.strokePath (strokePath, juce::PathStrokeType (1.5f, juce::PathStrokeType::curved,
                                                     juce::PathStrokeType::rounded));

    // Draggable dots
    for (int idx : { 1, 2, 4 })
    {
        g.setColour (draggingPoint == idx ? accent : Colours::bg);
        g.fillEllipse (pts[(size_t) idx].x - 4, pts[(size_t) idx].y - 4, 8, 8);
        g.setColour (accent);
        g.drawEllipse (pts[(size_t) idx].x - 4, pts[(size_t) idx].y - 4, 8, 8, 1.5f);
    }

    // ADSR text
    float a = attackParam ? attackParam->convertFrom0to1 (attackParam->getValue()) : 0.01f;
    float d = decayParam ? decayParam->convertFrom0to1 (decayParam->getValue()) : 0.3f;
    float s = sustainParam ? sustainParam->convertFrom0to1 (sustainParam->getValue()) : 0.5f;
    float r = releaseParam ? releaseParam->convertFrom0to1 (releaseParam->getValue()) : 0.4f;

    g.setColour (Colours::textMuted);
    g.setFont (juce::Font (juce::FontOptions (juce::Font::getDefaultMonospacedFontName(), 8.0f, juce::Font::plain)));
    juce::String text = "A:" + juce::String (a, 2) + " D:" + juce::String (d, 2)
                      + " S:" + juce::String (s, 2) + " R:" + juce::String (r, 2);
    g.drawText (text, juce::Rectangle<float> (padding, h - 12, w - padding * 2, 12),
                juce::Justification::centredLeft);
}

void EnvelopeEditorComponent::mouseDown (const juce::MouseEvent& e)
{
    draggingPoint = findDragPoint (e.position);

    if (draggingPoint >= 0)
    {
        if (draggingPoint == 1 && attackParam) attackParam->beginChangeGesture();
        if (draggingPoint == 2) { if (decayParam) decayParam->beginChangeGesture(); if (sustainParam) sustainParam->beginChangeGesture(); }
        if (draggingPoint == 4 && releaseParam) releaseParam->beginChangeGesture();
    }
}

void EnvelopeEditorComponent::mouseDrag (const juce::MouseEvent& e)
{
    if (draggingPoint < 0 || apvtsPtr == nullptr)
        return;

    float w = (float) getWidth();
    float h = (float) getHeight();
    float ih = h - padding * 2 - 12;
    float iw = w - padding * 2;

    float mx = e.position.x;
    float my = e.position.y;

    // Get current denormalized values
    float attack = attackParam ? attackParam->convertFrom0to1 (attackParam->getValue()) : 0.01f;
    float decay = decayParam ? decayParam->convertFrom0to1 (decayParam->getValue()) : 0.3f;
    float release = releaseParam ? releaseParam->convertFrom0to1 (releaseParam->getValue()) : 0.4f;
    float totalTime = attack + decay + holdTime + release;
    float scaleX = iw / totalTime;

    if (draggingPoint == 1 && attackParam)
    {
        float newAttack = std::clamp ((mx - padding) / scaleX, 0.001f, 2.0f);
        attackParam->setValueNotifyingHost (attackParam->convertTo0to1 (newAttack));
    }
    else if (draggingPoint == 2)
    {
        if (decayParam)
        {
            float decayX = mx - padding - attack * scaleX;
            float newDecay = std::clamp (decayX / scaleX, 0.01f, 2.0f);
            decayParam->setValueNotifyingHost (decayParam->convertTo0to1 (newDecay));
        }
        if (sustainParam)
        {
            float newSustain = std::clamp (1.0f - (my - padding) / ih, 0.0f, 1.0f);
            sustainParam->setValueNotifyingHost (sustainParam->convertTo0to1 (newSustain));
        }
    }
    else if (draggingPoint == 4 && releaseParam)
    {
        float relX = mx - padding - (attack + decay + holdTime) * scaleX;
        float newRelease = std::clamp (relX / scaleX, 0.01f, 2.0f);
        releaseParam->setValueNotifyingHost (releaseParam->convertTo0to1 (newRelease));
    }

    repaint();
}

void EnvelopeEditorComponent::mouseUp (const juce::MouseEvent&)
{
    if (draggingPoint == 1 && attackParam) attackParam->endChangeGesture();
    if (draggingPoint == 2) { if (decayParam) decayParam->endChangeGesture(); if (sustainParam) sustainParam->endChangeGesture(); }
    if (draggingPoint == 4 && releaseParam) releaseParam->endChangeGesture();

    draggingPoint = -1;
    repaint();
}
