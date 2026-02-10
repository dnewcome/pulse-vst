#pragma once

class AftertouchProcessor
{
public:
    struct Config
    {
        float dampenSensitivity = 0.7f;
        float retriggerThreshold = 0.3f;
    };

    void setAftertouch (float pressure);
    float getAftertouch() const { return currentPressure; }
    float getDampenMultiplier() const;
    bool shouldRetrigger();

    Config config;

private:
    float currentPressure = 0.0f;
    float previousPressure = 0.0f;
    bool retriggerArmed = true;
};
