#include "Shimmer.h"

namespace cloudvox::dsp
{
void Shimmer::prepare(const juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate;
    reverb.prepare(spec);
    preDelay.setMaximumDelayInSamples(static_cast<int>(sampleRate * 0.2));
    preDelay.reset();
    pitchShifter.prepare(spec);
    pitchShifter.setPitchRatio(2.0f); // +12 semitones
    pitchShifter.setWindowSize(1024);

    tempBuffer.setSize(static_cast<int>(spec.numChannels), static_cast<int>(spec.maximumBlockSize));

    setDecay(decayTime);
    setPreDelayMs(preDelayMs);
}

void Shimmer::reset()
{
    reverb.reset();
    preDelay.reset();
    pitchShifter.reset();
}

void Shimmer::setMix(float newMix)
{
    mix = juce::jlimit(0.0f, 1.0f, newMix);
}

void Shimmer::setFeedbackGain(float newFeedback)
{
    feedback = juce::jlimit(0.0f, 0.95f, newFeedback);
}

void Shimmer::setPreDelayMs(float newPreDelay)
{
    preDelayMs = juce::jlimit(0.0f, 100.0f, newPreDelay);
    const auto delaySamples = static_cast<int>(sampleRate * preDelayMs / 1000.0);
    preDelay.setDelay(static_cast<float>(delaySamples));
}

void Shimmer::setDecay(float decaySeconds)
{
    decayTime = juce::jmax(0.1f, decaySeconds);
    reverbParams.roomSize = juce::jlimit(0.1f, 0.99f, decayTime / 3.5f);
    reverbParams.wetLevel = 0.5f;
    reverbParams.dryLevel = 0.0f;
    reverbParams.width = 1.0f;
    reverbParams.damping = 0.4f;
    reverb.setParameters(reverbParams);
}

void Shimmer::process(juce::AudioBuffer<float>& buffer)
{
    if (buffer.getNumSamples() == 0)
        return;

    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();

    tempBuffer.makeCopyOf(buffer, true);

    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    reverb.process(context);

    for (int channel = 0; channel < numChannels; ++channel)
    {
        auto* wet = buffer.getWritePointer(channel);
        auto* dry = tempBuffer.getReadPointer(channel);
        for (int sample = 0; sample < numSamples; ++sample)
        {
            const float baseWet = wet[sample];
            const float delayed = preDelay.popSample(channel);
            preDelay.pushSample(channel, baseWet + feedback * delayed);
            const float pitched = pitchShifter.processSample(channel, delayed);
            const float shimmerSample = mix * pitched;
            const float baseMix = (1.0f - mix) * baseWet;
            wet[sample] = dry[sample] + baseMix + shimmerSample;
        }
    }
}
} // namespace cloudvox::dsp
