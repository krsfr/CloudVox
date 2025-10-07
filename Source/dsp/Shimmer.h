#pragma once

#include <juce_dsp/juce_dsp.h>

namespace cloudvox::dsp
{
class Shimmer
{
public:
    void prepare(const juce::dsp::ProcessSpec& spec);
    void reset();

    void setMix(float newMix);
    void setFeedbackGain(float newFeedback);
    void setPreDelayMs(float newPreDelay);
    void setDecay(float decaySeconds);

    void process(juce::AudioBuffer<float>& buffer);

private:
    juce::dsp::Reverb reverb;
    juce::dsp::Reverb::Parameters reverbParams;
    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear> preDelay { 48000 };
    juce::dsp::PitchShifter<float> pitchShifter;

    juce::AudioBuffer<float> tempBuffer;
    float mix = 0.5f;
    float feedback = 0.35f;
    float preDelayMs = 20.0f;
    float decayTime = 2.0f;
    double sampleRate = 44100.0;
};
} // namespace cloudvox::dsp
