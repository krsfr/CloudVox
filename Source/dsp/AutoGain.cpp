#include "AutoGain.h"

namespace cloudvox::dsp
{
void AutoGain::prepare(const juce::dsp::ProcessSpec& spec)
{
    juce::ignoreUnused(spec);
    reset();
}

void AutoGain::reset()
{
    currentGain = 1.0f;
}

void AutoGain::setTargetRms(float dbfs)
{
    targetRms = dbfs;
}

void AutoGain::setEnabled(bool shouldBeEnabled)
{
    enabled = shouldBeEnabled;
}

float AutoGain::getGainForBuffer(const juce::AudioBuffer<float>& buffer)
{
    if (! enabled)
        return 1.0f;

    double sumSquares = 0.0;
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();

    for (int channel = 0; channel < numChannels; ++channel)
    {
        const auto* data = buffer.getReadPointer(channel);
        for (int sample = 0; sample < numSamples; ++sample)
            sumSquares += data[sample] * data[sample];
    }

    const double rms = std::sqrt(sumSquares / juce::jmax(1, numChannels * numSamples));
    const float rmsDb = juce::Decibels::gainToDecibels(static_cast<float>(rms), -120.0f);
    const float gainDb = targetRms - rmsDb;
    const float targetGain = juce::Decibels::decibelsToGain(gainDb);
    currentGain = juce::jlimit(0.25f, 4.0f, smoothingCoefficient * currentGain + (1.0f - smoothingCoefficient) * targetGain);
    return currentGain;
}
} // namespace cloudvox::dsp
