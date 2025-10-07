#pragma once

#include <juce_gui_extra/juce_gui_extra.h>

#include "CloudVoxAudioProcessor.h"
#include "ui/Knob.h"
#include "ui/Meter.h"
#include "ui/PresetBar.h"

class CloudVoxAudioProcessorEditor : public juce::AudioProcessorEditor
                                   , private cloudvox::ui::PresetBar::Listener
{
public:
    explicit CloudVoxAudioProcessorEditor(CloudVoxAudioProcessor&);
    ~CloudVoxAudioProcessorEditor() override = default;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void presetSelected(const juce::String& presetId) override;

    CloudVoxAudioProcessor& processor;

    cloudvox::ui::Knob intensityKnob;
    cloudvox::ui::Knob airKnob;
    cloudvox::ui::Knob glueKnob;
    cloudvox::ui::Knob widthKnob;
    cloudvox::ui::Knob detuneKnob;
    cloudvox::ui::Knob tightnessKnob;
    cloudvox::ui::Knob saturateKnob;
    cloudvox::ui::Knob inputKnob;
    cloudvox::ui::Knob outputKnob;

    juce::ToggleButton autoGainButton { "Auto Gain" };

    cloudvox::ui::Meter inputMeter;
    cloudvox::ui::Meter outputMeter;
    cloudvox::ui::PresetBar presetBar;

    juce::OwnedArray<juce::AudioProcessorValueTreeState::SliderAttachment> sliderAttachments;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> autoGainAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CloudVoxAudioProcessorEditor)
};
