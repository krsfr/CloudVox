#include "MicroPitch.h"

namespace cloudvox::dsp
{
void MicroPitch::prepare(const juce::dsp::ProcessSpec& spec, float maxDelayMs)
{
    sampleRate = spec.sampleRate;
    const size_t numChannels = static_cast<size_t>(spec.numChannels);
    if (numChannels == 0)
        return;
    delaysA.resize(numChannels);
    delaysB.resize(numChannels);
    for (auto& d : delaysA)
        d.prepare(sampleRate, maxDelayMs);
    for (auto& d : delaysB)
        d.prepare(sampleRate, maxDelayMs);
}

void MicroPitch::reset()
{
    for (auto& d : delaysA)
        d.reset();
    for (auto& d : delaysB)
        d.reset();
}

void MicroPitch::setDetuneCents(float cents)
{
    detuneCents = juce::jlimit(0.0f, 12.0f, cents);
}

void MicroPitch::setModulationRate(float rateHz)
{
    modulationRate = juce::jlimit(0.05f, 2.0f, rateHz);
}

void MicroPitch::setModulationDepth(float depthMs)
{
    modulationDepthMs = juce::jlimit(0.0f, 5.0f, depthMs);
}

void MicroPitch::processBlock(juce::AudioBuffer<float>& buffer)
{
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();
    if (numChannels <= 0 || static_cast<size_t>(numChannels) > delaysA.size())
        return;

    for (int channel = 0; channel < numChannels; ++channel)
    {
        auto* samples = buffer.getWritePointer(channel);
        for (int sample = 0; sample < numSamples; ++sample)
        {
            const float lfoA = computeLfoValue(0, static_cast<size_t>(sample));
            const float lfoB = computeLfoValue(1, static_cast<size_t>(sample));
            const float depthScale = detuneCents / 12.0f;
            const float modDepth = modulationDepthMs * (0.5f + depthScale * 0.5f);
            const float base = baseDelayMs + detuneCents * 0.5f;

            const float delayedA = delaysA[static_cast<size_t>(channel)].process(samples[sample], base, modDepth, lfoA);
            const float delayedB = delaysB[static_cast<size_t>(channel)].process(samples[sample], base * 1.1f, modDepth, lfoB);

            samples[sample] = 0.5f * (delayedA + delayedB);
        }
    }
}

float MicroPitch::computeLfoValue(int voice, size_t sampleIndex) const
{
    const float phase = (static_cast<float>(sampleIndex) / static_cast<float>(sampleRate)) * modulationRate;
    const float wrapped = juce::MathConstants<float>::twoPi * phase + lfoPhaseOffsets[static_cast<size_t>(voice) % lfoPhaseOffsets.size()];
    return std::sin(wrapped);
}
} // namespace cloudvox::dsp
