#include "ui/EQSliderComponent.h"

EQSliderComponent::EQSliderComponent(juce::AudioProcessorValueTreeState &state, juce::String parameterID, juce::String unit)
	: component(*state.getParameter(parameterID), unit)
{
	attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(state, parameterID, component);
	addAndMakeVisible(component);
}

void EQSliderComponent::resized()
{
	juce::FlexBox fb;
	fb.flexDirection = juce::FlexBox::Direction::column;
	fb.items.add(juce::FlexItem(component).withFlex(1.0f));

	fb.performLayout(getLocalBounds().toFloat());
}

void EQSliderComponent::addLabel(float position, juce::String label)
{
	component.labels.add({position, label});
}
