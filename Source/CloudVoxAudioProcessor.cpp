#include "CloudVoxAudioProcessor.h"
#include "CloudVoxAudioProcessorEditor.h"

CloudVoxAudioProcessor::CloudVoxAudioProcessor()
    : juce::AudioProcessor(BusesProperties()
                               .withInput("Input", juce::AudioChannelSet::stereo(), true)
                               .withOutput("Output", juce::AudioChannelSet::stereo(), true))
    , parameters(*this, nullptr, "CloudVox", createParameterLayout())
    , presetManager(parameters)
{
    intensitySmoothed.reset(44100.0, 0.05);
    airSmoothed.reset(44100.0, 0.05);
    glueSmoothed.reset(44100.0, 0.1);
    widthSmoothed.reset(44100.0, 0.05);
    detuneSmoothed.reset(44100.0, 0.05);
    tightnessSmoothed.reset(44100.0, 0.05);
    saturateSmoothed.reset(44100.0, 0.1);
    inputSmoothed.reset(44100.0, 0.05);
    outputSmoothed.reset(44100.0, 0.05);
}

void CloudVoxAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec { sampleRate, static_cast<juce::uint32>(samplesPerBlock), static_cast<juce::uint32>(getTotalNumOutputChannels()) };

    tiltEq.prepare(spec);
    microPitch.prepare(spec);
    shimmer.prepare(spec);
    saturator.prepare(spec);
    autoGain.prepare(spec);

    intensitySmoothed.reset(sampleRate, 0.05);
    airSmoothed.reset(sampleRate, 0.05);
    glueSmoothed.reset(sampleRate, 0.1);
    widthSmoothed.reset(sampleRate, 0.05);
    detuneSmoothed.reset(sampleRate, 0.05);
    tightnessSmoothed.reset(sampleRate, 0.05);
    saturateSmoothed.reset(sampleRate, 0.1);
    inputSmoothed.reset(sampleRate, 0.05);
    outputSmoothed.reset(sampleRate, 0.05);

    doublerBuffer.setSize(getTotalNumOutputChannels(), samplesPerBlock);
    shimmerBuffer.setSize(getTotalNumOutputChannels(), samplesPerBlock);
}

void CloudVoxAudioProcessor::releaseResources()
{
}

bool CloudVoxAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    if (layouts.getMainInputChannelSet() != layouts.getMainOutputChannelSet())
        return false;

    return true;
}

void CloudVoxAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    juce::ignoreUnused(midiMessages);

    updateMacroTargets();

    const int numSamples = buffer.getNumSamples();

    auto nextValueForBlock = [numSamples](juce::SmoothedValue<float>& value)
    {
        auto current = value.getNextValue();
        if (numSamples > 1)
            value.skip(numSamples - 1);
        return current;
    };

    const float intensity = nextValueForBlock(intensitySmoothed);
    const float air = nextValueForBlock(airSmoothed);
    const float glue = nextValueForBlock(glueSmoothed);
    const float width = nextValueForBlock(widthSmoothed);
    const float detune = nextValueForBlock(detuneSmoothed);
    const float tightness = nextValueForBlock(tightnessSmoothed);
    const float saturate = nextValueForBlock(saturateSmoothed);
    const float inputGainDb = nextValueForBlock(inputSmoothed);
    const float outputGainDb = nextValueForBlock(outputSmoothed);

    const float inputGain = juce::Decibels::decibelsToGain(inputGainDb);
    buffer.applyGain(inputGain);

    tiltEq.setTilt(juce::jmap(air, -6.0f, 6.0f, 750.0f, 1100.0f), air);
    tiltEq.process(buffer);

    doublerBuffer.makeCopyOf(buffer);
    microPitch.setDetuneCents(detune);
    microPitch.setModulationRate(juce::jmap(tightness, 0.0f, 1.0f, 0.35f, 0.15f));
    microPitch.setModulationDepth(juce::jmap(tightness, 0.0f, 1.0f, 0.4f, 2.5f));
    microPitch.processBlock(doublerBuffer);

    const float doublerMix = juce::jmap(intensity, 0.0f, 1.0f, 0.0f, 0.75f);
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* write = buffer.getWritePointer(channel);
        const auto* doublePtr = doublerBuffer.getReadPointer(channel);
        for (int sample = 0; sample < numSamples; ++sample)
            write[sample] = juce::jlimit(-1.0f, 1.0f, write[sample] + doublerMix * doublePtr[sample]);
    }

    applyWidth(buffer, juce::jlimit(0.0f, 1.5f, width));

    shimmerBuffer.makeCopyOf(buffer);
    shimmer.setMix(juce::jmap(intensity, 0.0f, 1.0f, 0.1f, 0.6f));
    shimmer.setFeedbackGain(juce::jmap(intensity, 0.0f, 1.0f, 0.2f, 0.45f));
    shimmer.process(shimmerBuffer);
    const float shimmerMix = juce::jmap(intensity, 0.0f, 1.0f, 0.0f, 0.5f);
    const int channels = juce::jmin(buffer.getNumChannels(), shimmerBuffer.getNumChannels());
    for (int channel = 0; channel < channels; ++channel)
        buffer.addFromWithRamp(channel, 0, shimmerBuffer.getReadPointer(channel), numSamples, shimmerMix, shimmerMix);

    saturator.setDrive(saturate);
    saturator.process(buffer);

    const bool autoGainEnabled = parameters.getRawParameterValue("autogain")->load() > 0.5f;
    autoGain.setEnabled(autoGainEnabled);
    const float autoGainValue = autoGain.getGainForBuffer(buffer);
    buffer.applyGain(autoGainValue);

    buffer.applyGain(juce::Decibels::decibelsToGain(outputGainDb));
}

void CloudVoxAudioProcessor::applyWidth(juce::AudioBuffer<float>& buffer, float widthAmount)
{
    if (buffer.getNumChannels() < 2)
        return;

    auto* left = buffer.getWritePointer(0);
    auto* right = buffer.getWritePointer(1);
    const int numSamples = buffer.getNumSamples();

    for (int sample = 0; sample < numSamples; ++sample)
    {
        const float mid = 0.5f * (left[sample] + right[sample]);
        float side = 0.5f * (left[sample] - right[sample]);
        side *= widthAmount;
        left[sample] = juce::jlimit(-1.0f, 1.0f, mid + side);
        right[sample] = juce::jlimit(-1.0f, 1.0f, mid - side);
    }
}

juce::AudioProcessorEditor* CloudVoxAudioProcessor::createEditor()
{
    return new CloudVoxAudioProcessorEditor(*this);
}

void CloudVoxAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    if (auto state = parameters.copyState(); state.isValid())
    {
        if (auto xml = state.createXml())
            copyXmlToBinary(*xml, destData);
    }
}

void CloudVoxAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary(data, sizeInBytes))
    {
        if (xml->hasTagName(parameters.state.getType()))
            parameters.replaceState(juce::ValueTree::fromXml(*xml));
    }
}

juce::AudioProcessorValueTreeState::ParameterLayout CloudVoxAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>("intensity", "Intensity", juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("air", "Air/Dark", juce::NormalisableRange<float>(-6.0f, 6.0f), 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("glue", "Glue", juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("width", "Width", juce::NormalisableRange<float>(0.0f, 1.5f), 1.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("detune", "Detune", juce::NormalisableRange<float>(0.0f, 12.0f), 6.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("tightness", "Tightness", juce::NormalisableRange<float>(0.0f, 1.0f), 0.35f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("saturate", "Saturate", juce::NormalisableRange<float>(0.0f, 1.0f), 0.2f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("input", "Input", juce::NormalisableRange<float>(-12.0f, 12.0f), 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("output", "Output", juce::NormalisableRange<float>(-12.0f, 12.0f), 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterBool>("autogain", "Auto Gain", true));

    return { params.begin(), params.end() };
}

void CloudVoxAudioProcessor::updateMacroTargets()
{
    const auto load = [&](const juce::String& paramID)
    {
        return parameters.getRawParameterValue(paramID)->load();
    };

    const float intensity = load("intensity");
    intensitySmoothed.setTargetValue(intensity);

    const float air = load("air");
    airSmoothed.setTargetValue(air);

    const float glue = load("glue");
    glueSmoothed.setTargetValue(glue);

    const float width = load("width");
    widthSmoothed.setTargetValue(width);

    const float detune = load("detune");
    detuneSmoothed.setTargetValue(detune);

    const float tightness = load("tightness");
    tightnessSmoothed.setTargetValue(tightness);

    const float saturate = load("saturate");
    saturateSmoothed.setTargetValue(saturate);

    const float inputGain = load("input");
    inputSmoothed.setTargetValue(inputGain);

    const float outputGain = load("output");
    outputSmoothed.setTargetValue(outputGain);

    shimmer.setPreDelayMs(juce::jmap(glue, 0.0f, 1.0f, 10.0f, 35.0f));
    shimmer.setDecay(juce::jmap(glue, 0.0f, 1.0f, 1.0f, 3.0f));
    autoGain.setTargetRms(juce::jmap(intensity, 0.0f, 1.0f, -20.0f, -16.0f));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new CloudVoxAudioProcessor();
}
