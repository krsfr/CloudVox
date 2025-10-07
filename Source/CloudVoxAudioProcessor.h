#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

#include "dsp/TiltEQ.h"
#include "dsp/MicroPitch.h"
#include "dsp/Shimmer.h"
#include "dsp/Saturator.h"
#include "dsp/AutoGain.h"
#include "state/PresetManager.h"

class CloudVoxAudioProcessor : public juce::AudioProcessor
{
public:
    CloudVoxAudioProcessor();
    ~CloudVoxAudioProcessor() override = default;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return JucePlugin_Name; }

    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 2.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int index) override { juce::ignoreUnused(index); }
    const juce::String getProgramName(int index) override { juce::ignoreUnused(index); return {}; }
    void changeProgramName(int index, const juce::String& name) override { juce::ignoreUnused(index, name); }

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState& getValueTreeState() { return parameters; }
    cloudvox::PresetManager& getPresetManager() { return presetManager; }

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

private:
    void updateMacroTargets();
    void applyWidth(juce::AudioBuffer<float>& buffer, float widthAmount);

    juce::AudioProcessorValueTreeState parameters;

    juce::SmoothedValue<float> intensitySmoothed;
    juce::SmoothedValue<float> airSmoothed;
    juce::SmoothedValue<float> glueSmoothed;
    juce::SmoothedValue<float> widthSmoothed;
    juce::SmoothedValue<float> detuneSmoothed;
    juce::SmoothedValue<float> tightnessSmoothed;
    juce::SmoothedValue<float> saturateSmoothed;
    juce::SmoothedValue<float> inputSmoothed;
    juce::SmoothedValue<float> outputSmoothed;

    cloudvox::dsp::TiltEQ tiltEq;
    cloudvox::dsp::MicroPitch microPitch;
    cloudvox::dsp::Shimmer shimmer;
    cloudvox::dsp::Saturator saturator;
    cloudvox::dsp::AutoGain autoGain;

    cloudvox::PresetManager presetManager;

    juce::AudioBuffer<float> doublerBuffer;
    juce::AudioBuffer<float> shimmerBuffer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CloudVoxAudioProcessor)
};
