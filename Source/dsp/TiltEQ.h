#pragma once

#include <juce_dsp/juce_dsp.h>

namespace cloudvox::dsp
{
class TiltEQ
{
public:
    void prepare(const juce::dsp::ProcessSpec& spec);
    void reset();

    void setTilt(float centreFrequencyHz, float slopeDb);

    void process(juce::AudioBuffer<float>& buffer);

private:
    void updateFilters();

    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> lowShelf;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> highShelf;

    double sampleRate = 44100.0;
    float centreFrequency = 900.0f;
    float slope = 0.0f;
    bool isPrepared = false;
};
} // namespace cloudvox::dsp
