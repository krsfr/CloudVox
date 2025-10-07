#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace cloudvox::ui
{
class Knob : public juce::Slider
{
public:
    Knob()
    {
        setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    }
};
} // namespace cloudvox::ui
