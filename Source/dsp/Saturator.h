#pragma once

#include <juce_dsp/juce_dsp.h>

namespace cloudvox::dsp
{
class Saturator
{
public:
    void prepare(const juce::dsp::ProcessSpec& spec);
    void reset();

    void setDrive(float driveAmount);
    void process(juce::AudioBuffer<float>& buffer);

private:
    float drive = 0.0f;
    juce::dsp::IIR::Filter<float> preHighPass;
    juce::dsp::IIR::Filter<float> postHighShelf;
};
} // namespace cloudvox::dsp
