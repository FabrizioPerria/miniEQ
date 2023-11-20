#include "ui/SliderWithLabelComponent.h"

SliderWithLabelsComponent::SliderWithLabelsComponent(juce::RangedAudioParameter &parameter, const juce::String &unit)
	: juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, juce::Slider::TextEntryBoxPosition::NoTextBox),
	  param(&parameter), suffix(unit)
{
	setLookAndFeel(&lnf);
}

SliderWithLabelsComponent::~SliderWithLabelsComponent()
{
	setLookAndFeel(nullptr);
}

void SliderWithLabelsComponent::paint(juce::Graphics &g)
{
	// The compensate is a full 2pi rotation needed to make sure the max angle is greater than the min angle, but keeping the angles
	// correct
	auto compensate = juce::MathConstants<float>::twoPi;
	auto minAngle = juce::degreesToRadians(180.f + 45.f);
	auto maxAngle = juce::degreesToRadians(180.f - 45.f) + compensate;

	auto range = getRange();
	auto sliderBounds = getSliderBounds();

	lnf.drawRotarySlider(g, sliderBounds.getX(), sliderBounds.getY(), sliderBounds.getWidth(), sliderBounds.getHeight(),
						 (float)juce::jmap(getValue(), range.getStart(), range.getEnd(), 0.0, 1.0), minAngle, maxAngle, *this);

	auto center = sliderBounds.toFloat().getCentre();
	auto radius = sliderBounds.getWidth() * 0.5f;

	for (auto label : labels)
	{
		auto pos = label.position;
		jassert(pos >= 0 && pos <= 1);
		auto angle = juce::jmap(pos, 0.0f, 1.0f, minAngle, maxAngle);
		auto c = center.getPointOnCircumference(radius + getTextHeight() * 0.5f + 1, angle);
		juce::Rectangle<float> r;
		auto str = label.label;
		r.setSize(g.getCurrentFont().getStringWidth(str) + 2, getTextHeight());
		r.setCentre(c);
		r.setY(r.getY() + getTextHeight());
		g.setColour(juce::Colours::cyan);
		g.drawFittedText(str, r.toNearestInt(), juce::Justification::centred, 1);
	}
}

void SliderWithLabelsComponent::drawBounds(juce::Graphics &g, juce::Colour color, juce::Rectangle<int> bounds)
{
	g.setColour(color);
	g.drawRect(bounds, 1.0f);
}

void SliderWithLabelsComponent::resized()
{
	juce::Slider::resized();
}

juce::Rectangle<int> SliderWithLabelsComponent::getSliderBounds() const
{
	auto size = juce::jmin(getWidth(), getHeight());
	size -= (int)getTextHeight() * 2;
	juce::Rectangle<int> bounds;
	bounds.setSize(size, size);
	bounds.setCentre(getLocalBounds().getCentreX(), 0);
	bounds.setY(5);
	return bounds;
}

juce::String SliderWithLabelsComponent::getDisplayText() const
{
	if (auto *audioParam = dynamic_cast<juce::AudioParameterFloat *>(param))
	{
		auto value = getValue();
		bool isValueLarge = value > 999.f;
		if (isValueLarge)
		{
			value /= 1000.f;
		}
		return juce::String(value, isValueLarge ? 2 : 0) + (isValueLarge ? "k" : "") + suffix;
	}
	return param->getCurrentValueAsText() + suffix;
}

float SliderWithLabelsComponent::getTextHeight() const
{
	return 14.f;
}
