#include "TiltEQ.h"

namespace cloudvox::dsp
{
namespace
{
constexpr float minSlopeDb = -12.0f;
constexpr float maxSlopeDb = 12.0f;
}

void TiltEQ::prepare(const juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate;
    lowShelf.prepare(spec);
    highShelf.prepare(spec);
    updateFilters();
    isPrepared = true;
}

void TiltEQ::reset()
{
    lowShelf.reset();
    highShelf.reset();
}

void TiltEQ::setTilt(float centreFrequencyHz, float slopeDb)
{
    centreFrequency = juce::jlimit(200.0f, 4000.0f, centreFrequencyHz);
    slope = juce::jlimit(minSlopeDb, maxSlopeDb, slopeDb);
    if (isPrepared)
        updateFilters();
}

void TiltEQ::process(juce::AudioBuffer<float>& buffer)
{
    jassert(isPrepared);
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    lowShelf.process(context);
    highShelf.process(context);
}

void TiltEQ::updateFilters()
{
    const float lowGain = juce::Decibels::decibelsToGain(slope * 0.5f);
    const float highGain = juce::Decibels::decibelsToGain(-slope * 0.5f);

    *lowShelf.state = *juce::dsp::IIR::Coefficients<float>::makeLowShelf(sampleRate, centreFrequency, 0.707f, lowGain);
    *highShelf.state = *juce::dsp::IIR::Coefficients<float>::makeHighShelf(sampleRate, centreFrequency, 0.707f, highGain);
}
} // namespace cloudvox::dsp
