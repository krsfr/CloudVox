#include "Saturator.h"

#include <cmath>

namespace cloudvox::dsp
{
void Saturator::prepare(const juce::dsp::ProcessSpec& spec)
{
    preHighPass.prepare(spec);
    postHighShelf.prepare(spec);

    auto hpCoeffs = juce::dsp::IIR::Coefficients<float>::makeHighPass(spec.sampleRate, 100.0f);
    preHighPass.coefficients = hpCoeffs;

    auto hsCoeffs = juce::dsp::IIR::Coefficients<float>::makeHighShelf(spec.sampleRate, 8000.0f, 0.707f, 1.5f);
    postHighShelf.coefficients = hsCoeffs;
}

void Saturator::reset()
{
    preHighPass.reset();
    postHighShelf.reset();
}

void Saturator::setDrive(float driveAmount)
{
    drive = juce::jlimit(0.0f, 1.0f, driveAmount);
}

void Saturator::process(juce::AudioBuffer<float>& buffer)
{
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    preHighPass.process(context);

    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();
    const float driveGain = juce::jmap(drive, 1.0f, 6.0f);

    for (int channel = 0; channel < numChannels; ++channel)
    {
        auto* samples = buffer.getWritePointer(channel);
        for (int sample = 0; sample < numSamples; ++sample)
        {
            const float x = samples[sample] * driveGain;
            samples[sample] = std::tanh(x);
        }
    }

    postHighShelf.process(context);
}
} // namespace cloudvox::dsp
