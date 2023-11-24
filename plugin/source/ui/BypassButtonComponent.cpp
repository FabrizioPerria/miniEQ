#include "ui/BypassButtonComponent.h"

BypassButtonComponent::BypassButtonComponent(juce::AudioProcessorValueTreeState &apvts, const juce::String &parameterID, SVGData svg)
	: icon("Icon", juce::DrawableButton::ButtonStyle::ImageFitted),
	  buttonAttachment(std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, parameterID, icon))
{
	auto buttonImage = juce::Drawable::createFromImageData(svg.svg, svg.size);
	auto buttonImageSelected = juce::Drawable::createFromImageData(svg.svg, svg.size);
	buttonImage->replaceColour(juce::Colours::black, juce::Colours::cyan);
	buttonImageSelected->replaceColour(juce::Colours::black, juce::Colours::red);

	icon.setImages(buttonImage.get(), nullptr, nullptr, nullptr, buttonImageSelected.get());

	icon.setClickingTogglesState(true);
	icon.setToggleState(false, juce::dontSendNotification);
	icon.setSize(25, 25);
	addAndMakeVisible(icon);
}

BypassButtonComponent::~BypassButtonComponent()
{
}

void BypassButtonComponent::paint(juce::Graphics &g)
{
}

void BypassButtonComponent::resized()
{
	juce::FlexBox fb;
	fb.flexDirection = juce::FlexBox::Direction::row;
	fb.flexWrap = juce::FlexBox::Wrap::noWrap;

	fb.items.add(juce::FlexItem(icon).withFlex(0, 0, 20));

	fb.performLayout(getLocalBounds());
}
