#include "ui/BypassButtonComponent.h"

BypassButtonComponent::BypassButtonComponent(juce::AudioProcessorValueTreeState &apvts, const juce::String &parameterID, SVGData svg)
	: buttonAttachment(std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, parameterID, *this))
{
	buttonImage = juce::Drawable::createFromImageData(svg.svg, svg.size);
	buttonImageSelected = juce::Drawable::createFromImageData(svg.svg, svg.size);
	buttonImage->replaceColour(juce::Colours::black, juce::Colours::limegreen);
	buttonImageSelected->replaceColour(juce::Colours::black, juce::Colours::red);

	setLookAndFeel(&lnf);
}

BypassButtonComponent::~BypassButtonComponent()
{
	setLookAndFeel(nullptr);
}
