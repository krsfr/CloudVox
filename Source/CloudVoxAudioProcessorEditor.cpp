#include "CloudVoxAudioProcessorEditor.h"

CloudVoxAudioProcessorEditor::CloudVoxAudioProcessorEditor(CloudVoxAudioProcessor& proc)
    : juce::AudioProcessorEditor(&proc)
    , processor(proc)
{
    setSize(640, 360);

    auto& apvts = processor.getValueTreeState();
    const auto addKnob = [&](cloudvox::ui::Knob& knob, const juce::String& paramID, const juce::String& label)
    {
        knob.setName(label);
        addAndMakeVisible(knob);
        sliderAttachments.add(new juce::AudioProcessorValueTreeState::SliderAttachment(apvts, paramID, knob));
    };

    addKnob(intensityKnob, "intensity", "Intensity");
    addKnob(airKnob, "air", "Air/Dark");
    addKnob(glueKnob, "glue", "Glue");
    addKnob(widthKnob, "width", "Width");
    addKnob(detuneKnob, "detune", "Detune");
    addKnob(tightnessKnob, "tightness", "Tightness");
    addKnob(saturateKnob, "saturate", "Saturate");
    addKnob(inputKnob, "input", "Input");
    addKnob(outputKnob, "output", "Output");

    addAndMakeVisible(autoGainButton);
    autoGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, "autogain", autoGainButton);

    addAndMakeVisible(inputMeter);
    addAndMakeVisible(outputMeter);

    addAndMakeVisible(presetBar);
    presetBar.addListener(this);

    juce::StringArray presetNames;
    juce::StringArray presetIds;
    for (const auto& preset : processor.getPresetManager().getPresets())
    {
        presetNames.add(preset.name);
        presetIds.add(preset.identifier);
    }
    presetBar.setPresets(presetNames, presetIds);
}

void CloudVoxAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkslategrey);
    g.setColour(juce::Colours::white);
    g.setFont(18.0f);
    g.drawFittedText("CloudVox", getLocalBounds().removeFromTop(32), juce::Justification::centred, 1);
}

void CloudVoxAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    auto header = bounds.removeFromTop(48);
    presetBar.setBounds(header.removeFromBottom(36).reduced(8, 0));

    auto meterArea = bounds.removeFromRight(80).reduced(8);
    inputMeter.setBounds(meterArea.removeFromTop(bounds.getHeight() / 2));
    outputMeter.setBounds(meterArea);

    auto gainArea = bounds.removeFromBottom(80);
    autoGainButton.setBounds(gainArea.removeFromTop(24));

    auto gridArea = bounds.reduced(12);
    const int numColumns = 3;
    const int numRows = 3;
    const int knobWidth = gridArea.getWidth() / numColumns;
    const int knobHeight = gridArea.getHeight() / numRows;

    auto placeKnob = [&](cloudvox::ui::Knob& knob, int index)
    {
        const int row = index / numColumns;
        const int column = index % numColumns;
        juce::Rectangle<int> area(gridArea.getX() + column * knobWidth,
                                  gridArea.getY() + row * knobHeight,
                                  knobWidth,
                                  knobHeight);
        knob.setBounds(area.reduced(12));
    };

    placeKnob(intensityKnob, 0);
    placeKnob(airKnob, 1);
    placeKnob(glueKnob, 2);
    placeKnob(widthKnob, 3);
    placeKnob(detuneKnob, 4);
    placeKnob(tightnessKnob, 5);
    placeKnob(saturateKnob, 6);
    placeKnob(inputKnob, 7);
    placeKnob(outputKnob, 8);
}

void CloudVoxAudioProcessorEditor::presetSelected(const juce::String& presetId)
{
    processor.getPresetManager().applyPreset(presetId);
}
