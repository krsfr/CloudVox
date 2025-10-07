#pragma once

#include <cmath>
#include <vector>
#include <juce_dsp/juce_dsp.h>

namespace cloudvox::dsp
{
/** Simple time-domain pitch shifter used for the shimmer effect. */
class PitchShifter
{
public:
    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        sampleRate = spec.sampleRate;
        numChannels = static_cast<int>(spec.numChannels);
        initialiseChannels();
    }

    void reset()
    {
        for (auto& channel : channels)
        {
            std::fill(channel.buffer.begin(), channel.buffer.end(), 0.0f);
            channel.writeIndex = 0;
            channel.readIndex[0] = 0.0f;
            channel.readIndex[1] = static_cast<float>(windowSize) * 0.5f;
            channel.crossfadePhase = 0.0f;
            channel.activeReader = 0;
        }
    }

    void setPitchRatio(float newRatio)
    {
        pitchRatio = juce::jlimit(0.25f, 4.0f, newRatio);
    }

    void setWindowSize(int newSize)
    {
        windowSize = juce::jmax(128, newSize);
        initialiseChannels();
    }

    float processSample(int channelIndex, float inputSample)
    {
        if (channels.empty())
            return inputSample;

        auto& channel = channels[juce::jlimit(0, numChannels - 1, channelIndex)];

        channel.buffer[channel.writeIndex] = inputSample;
        channel.writeIndex = (channel.writeIndex + 1) % channel.buffer.size();

        auto readWithWrap = [&channel, bufferSize = static_cast<float>(channel.buffer.size())](float index)
        {
            index = std::fmod(index, bufferSize);
            if (index < 0.0f)
                index += bufferSize;

            const auto baseIndex = static_cast<int>(index);
            const auto nextIndex = (baseIndex + 1) % channel.buffer.size();
            const auto frac = index - static_cast<float>(baseIndex);
            const auto sampleA = channel.buffer[baseIndex];
            const auto sampleB = channel.buffer[nextIndex];
            return juce::jmap(frac, sampleA, sampleB);
        };

        const float sample0 = readWithWrap(channel.readIndex[0]);
        const float sample1 = readWithWrap(channel.readIndex[1]);

        const float phase = channel.crossfadePhase;
        const int active = channel.activeReader;
        const int inactive = 1 - active;

        const float output = (1.0f - phase) * (active == 0 ? sample0 : sample1)
                           + phase * (inactive == 0 ? sample0 : sample1);

        channel.readIndex[0] += pitchRatio;
        channel.readIndex[1] += pitchRatio;

        const float bufferSize = static_cast<float>(channel.buffer.size());
        if (channel.readIndex[0] >= bufferSize)
            channel.readIndex[0] -= bufferSize;
        if (channel.readIndex[1] >= bufferSize)
            channel.readIndex[1] -= bufferSize;

        channel.crossfadePhase += crossfadeIncrement;
        if (channel.crossfadePhase >= 1.0f)
        {
            const int previousActive = channel.activeReader;
            channel.crossfadePhase -= 1.0f;
            channel.activeReader = 1 - channel.activeReader;

            const int readerToReset = previousActive;
            channel.readIndex[readerToReset] = static_cast<float>(channel.writeIndex);
        }

        return output;
    }

private:
    struct ChannelState
    {
        std::vector<float> buffer;
        int writeIndex = 0;
        float readIndex[2] { 0.0f, 0.0f };
        float crossfadePhase = 0.0f;
        int activeReader = 0;
    };

    void initialiseChannels()
    {
        crossfadeIncrement = 1.0f / static_cast<float>(windowSize);

        channels.resize(numChannels);
        const auto bufferLength = juce::jmax(windowSize * 2, static_cast<int>(sampleRate * 0.1));

        for (auto& channel : channels)
        {
            channel.buffer.assign(static_cast<size_t>(bufferLength), 0.0f);
            channel.writeIndex = 0;
            channel.readIndex[0] = 0.0f;
            channel.readIndex[1] = static_cast<float>(windowSize) * 0.5f;
            channel.crossfadePhase = 0.0f;
            channel.activeReader = 0;
        }
    }

    double sampleRate = 44100.0;
    int numChannels = 2;
    int windowSize = 1024;
    float pitchRatio = 1.0f;
    float crossfadeIncrement = 1.0f / 1024.0f;

    std::vector<ChannelState> channels;
};
} // namespace cloudvox::dsp
