#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace cloudvox::ui
{
class PresetBar : public juce::Component
                  , public juce::Button::Listener
{
public:
    class Listener
    {
    public:
        virtual ~Listener() = default;
        virtual void presetSelected(const juce::String& presetId) = 0;
    };

    void setPresets(const juce::StringArray& presetNames, const juce::StringArray& presetIds)
    {
        jassert(presetNames.size() == presetIds.size());
        buttons.clear();
        presetIdentifiers = presetIds;
        for (int i = 0; i < presetNames.size(); ++i)
        {
            auto* button = buttons.add(new juce::TextButton(presetNames[i]));
            button->addListener(this);
            addAndMakeVisible(button);
        }
        resized();
    }

    void addListener(Listener* newListener)
    {
        listeners.add(newListener);
    }

    void removeListener(Listener* listener)
    {
        listeners.remove(listener);
    }

    void resized() override
    {
        auto area = getLocalBounds();
        for (auto* button : buttons)
            button->setBounds(area.removeFromLeft(120).reduced(4));
    }

    void buttonClicked(juce::Button* button) override
    {
        const int index = buttons.indexOf(static_cast<juce::TextButton*>(button));
        if (juce::isPositiveAndBelow(index, presetIdentifiers.size()))
            listeners.call([&](Listener& l) { l.presetSelected(presetIdentifiers[index]); });
    }

private:
    juce::ListenerList<Listener> listeners;
    juce::OwnedArray<juce::TextButton> buttons;
    juce::StringArray presetIdentifiers;
};
} // namespace cloudvox::ui
