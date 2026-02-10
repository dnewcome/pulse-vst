#include "AftertouchProcessor.h"

void AftertouchProcessor::setAftertouch (float pressure)
{
    previousPressure = currentPressure;
    currentPressure = pressure;

    // Re-arm retrigger when pressure drops below threshold
    if (currentPressure < config.retriggerThreshold)
        retriggerArmed = true;
}

float AftertouchProcessor::getDampenMultiplier() const
{
    return 1.0f - (currentPressure * config.dampenSensitivity);
}

bool AftertouchProcessor::shouldRetrigger()
{
    if (retriggerArmed
        && currentPressure >= config.retriggerThreshold
        && previousPressure < config.retriggerThreshold)
    {
        retriggerArmed = false;
        return true;
    }
    return false;
}
