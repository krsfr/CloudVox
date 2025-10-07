#include "PresetManager.h"

namespace cloudvox
{
PresetManager::PresetManager(juce::AudioProcessorValueTreeState& state)
    : apvts(state)
{
    buildDefaultPresets();
}

const juce::Array<Preset>& PresetManager::getPresets() const noexcept
{
    return presets;
}

void PresetManager::applyPreset(const juce::String& identifier)
{
    for (const auto& preset : presets)
    {
        if (preset.identifier == identifier)
        {
            preset.apply(apvts);
            return;
        }
    }
}

void PresetManager::buildDefaultPresets()
{
    presets.clear();

    const auto setValue = [](juce::AudioProcessorValueTreeState& state, const juce::String& paramID, float value)
    {
        if (auto* parameter = dynamic_cast<juce::RangedAudioParameter*>(state.getParameter(paramID)))
            parameter->setValueNotifyingHost(parameter->convertTo0to1(value));
    };

    presets.add({
        "Bladee – Drain Cloud",
        "drain-cloud",
        [setValue](juce::AudioProcessorValueTreeState& state)
        {
            setValue(state, "intensity", 0.55f);
            setValue(state, "air", 3.0f);
            setValue(state, "glue", 0.5f);
            setValue(state, "width", 1.35f);
            setValue(state, "detune", 7.0f);
            setValue(state, "tightness", 0.35f);
            setValue(state, "saturate", 0.2f);
        }
    });

    presets.add({
        "pluko – Glass Pop",
        "glass-pop",
        [setValue](juce::AudioProcessorValueTreeState& state)
        {
            setValue(state, "intensity", 0.45f);
            setValue(state, "air", 2.0f);
            setValue(state, "glue", 0.35f);
            setValue(state, "width", 1.2f);
            setValue(state, "detune", 4.0f);
            setValue(state, "tightness", 0.25f);
            setValue(state, "saturate", 0.15f);
        }
    });

    presets.add({
        "Heaven Radio",
        "heaven-radio",
        [setValue](juce::AudioProcessorValueTreeState& state)
        {
            setValue(state, "intensity", 0.7f);
            setValue(state, "glue", 0.75f);
            setValue(state, "width", 1.4f);
            setValue(state, "detune", 9.0f);
            setValue(state, "tightness", 0.4f);
            setValue(state, "saturate", 0.1f);
        }
    });
}
} // namespace cloudvox
