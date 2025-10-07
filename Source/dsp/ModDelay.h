#pragma once

#include <vector>

namespace cloudvox::dsp
{
class ModDelay
{
public:
    void prepare(double sampleRate, double maxDelayMs);
    void reset();

    float process(float input, float baseDelayMs, float modulationDepthMs, float lfoValue);

private:
    float readFractional(float index) const;

    std::vector<float> buffer;
    int writeIndex = 0;
    double sr = 44100.0;
};
} // namespace cloudvox::dsp
