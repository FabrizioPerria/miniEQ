#include "CustomSlider.h"

CustomSlider::CustomSlider(juce::AudioProcessorValueTreeState& state, juce::String parameterID)
	: juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, juce::Slider::TextEntryBoxPosition::NoTextBox)
{
	attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(state, parameterID, *this);
}

void CustomSlider::paint(juce::Graphics& g) { juce::Slider::paint(g); }

void CustomSlider::resized() { juce::Slider::resized(); }
