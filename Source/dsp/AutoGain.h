#pragma once

#include <juce_dsp/juce_dsp.h>

namespace cloudvox::dsp
{
class AutoGain
{
public:
    void prepare(const juce::dsp::ProcessSpec& spec);
    void reset();

    void setTargetRms(float dbfs);
    void setEnabled(bool shouldBeEnabled);

    float getGainForBuffer(const juce::AudioBuffer<float>& buffer);

private:
    float smoothingCoefficient = 0.99f;
    float targetRms = -18.0f;
    float currentGain = 1.0f;
    bool enabled = true;
};
} // namespace cloudvox::dsp
