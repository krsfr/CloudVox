#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace cloudvox::ui
{
class Meter : public juce::Component
{
public:
    void setLevel(float newLevel)
    {
        level = juce::jlimit(0.0f, 1.0f, newLevel);
        repaint();
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::black);
        auto bounds = getLocalBounds().toFloat();
        const float height = bounds.getHeight() * level;
        juce::Rectangle<float> bar(bounds.removeFromBottom(height));
        g.setColour(juce::Colours::aqua);
        g.fillRect(bar);
    }

private:
    float level = 0.0f;
};
} // namespace cloudvox::ui
