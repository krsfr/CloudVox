#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <functional>

namespace cloudvox
{
struct Preset
{
    juce::String name;
    juce::String identifier;
    std::function<void(juce::AudioProcessorValueTreeState&)> apply;
};

class PresetManager
{
public:
    explicit PresetManager(juce::AudioProcessorValueTreeState& state);

    const juce::Array<Preset>& getPresets() const noexcept;
    void applyPreset(const juce::String& identifier);

private:
    void buildDefaultPresets();

    juce::AudioProcessorValueTreeState& apvts;
    juce::Array<Preset> presets;
};
} // namespace cloudvox
