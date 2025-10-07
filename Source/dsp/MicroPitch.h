#pragma once

#include "ModDelay.h"
#include <juce_dsp/juce_dsp.h>
#include <array>
#include <vector>

namespace cloudvox::dsp
{
class MicroPitch
{
public:
    void prepare(const juce::dsp::ProcessSpec& spec, float maxDelayMs = 40.0f);
    void reset();

    void setDetuneCents(float cents);
    void setModulationRate(float rateHz);
    void setModulationDepth(float depthMs);

    void processBlock(juce::AudioBuffer<float>& buffer);

private:
    float computeLfoValue(int voice, size_t sampleIndex) const;

    std::vector<ModDelay> delaysA;
    std::vector<ModDelay> delaysB;
    std::array<float, 2> lfoPhaseOffsets { 0.0f, juce::MathConstants<float>::pi * 0.5f };

    double sampleRate = 44100.0;
    float detuneCents = 6.0f;
    float modulationRate = 0.25f;
    float modulationDepthMs = 1.0f;
    float baseDelayMs = 15.0f;
};
} // namespace cloudvox::dsp
