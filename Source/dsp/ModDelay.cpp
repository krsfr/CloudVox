#include "ModDelay.h"

#include <algorithm>
#include <cmath>

namespace cloudvox::dsp
{
void ModDelay::prepare(double sampleRate, double maxDelayMs)
{
    sr = sampleRate;
    const int maxSamples = static_cast<int>(std::ceil(sr * maxDelayMs / 1000.0)) + 2;
    buffer.assign(maxSamples, 0.0f);
    writeIndex = 0;
}

void ModDelay::reset()
{
    std::fill(buffer.begin(), buffer.end(), 0.0f);
    writeIndex = 0;
}

float ModDelay::process(float input, float baseDelayMs, float modulationDepthMs, float lfoValue)
{
    if (buffer.empty())
        return input;

    buffer[writeIndex] = input;

    const float delaySamples = static_cast<float>((baseDelayMs + modulationDepthMs * lfoValue) * sr / 1000.0);
    const float readIndex = static_cast<float>(writeIndex) - delaySamples;
    const float output = readFractional(readIndex);

    if (++writeIndex >= static_cast<int>(buffer.size()))
        writeIndex = 0;

    return output;
}

float ModDelay::readFractional(float index) const
{
    const int bufferSize = static_cast<int>(buffer.size());
    while (index < 0.0f)
        index += bufferSize;

    const int index0 = static_cast<int>(index) % bufferSize;
    const int index1 = (index0 + 1) % bufferSize;
    const float fraction = index - static_cast<float>(index0);
    return buffer[index0] + (buffer[index1] - buffer[index0]) * fraction;
}
} // namespace cloudvox::dsp
