#pragma once

#include "ui/LookAndFeel/SliderLookAndFeel.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>

class EQSliderComponent : public juce::Slider
{
  public:
	EQSliderComponent(juce::RangedAudioParameter &parameter, const juce::String &unit)
		: juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, juce::Slider::TextEntryBoxPosition::NoTextBox),
		  param(&parameter), suffix(unit)
	{
		setLookAndFeel(&lnf);
	}

	~EQSliderComponent() override
	{
		setLookAndFeel(nullptr);
	}

	void paint(juce::Graphics &g) override
	{
		// The compensate is a full 2pi rotation needed to make sure the max angle is greater than the min angle, but keeping the angles
		// correct
		auto compensate = juce::MathConstants<float>::twoPi;
		auto minAngle = juce::degreesToRadians(180.f + 45.f);
		auto maxAngle = juce::degreesToRadians(180.f - 45.f) + compensate;

		auto range = getRange();
		auto sliderBounds = getSliderBounds();

		/* drawBounds(g, juce::Colours::white, sliderBounds);		// TODO: remove */
		/* drawBounds(g, juce::Colours::orange, getLocalBounds()); // TODO: remove */

		lnf.drawRotarySlider(g, sliderBounds.getX(), sliderBounds.getY(), sliderBounds.getWidth(), sliderBounds.getHeight(),
							 (float)juce::jmap(getValue(), range.getStart(), range.getEnd(), 0.0, 1.0), minAngle, maxAngle, *this);
	}

	void drawBounds(juce::Graphics &g, juce::Colour color, juce::Rectangle<int> bounds)
	{
		g.setColour(color);
		g.drawRect(bounds, 1.0f);
	}

	void resized() override
	{
		juce::Slider::resized();
	}

	juce::Rectangle<int> getSliderBounds() const
	{
		auto size = juce::jmin(getWidth(), getHeight());
		size -= getTextHeight() * 2;
		juce::Rectangle<int> bounds;
		bounds.setSize(size, size);
		bounds.setCentre(getLocalBounds().getCentreX(), 0);
		bounds.setY(2);
		return bounds;
	}

	juce::String getDisplayText() const
	{
		return param->getCurrentValueAsText() + suffix;
	}

	float getTextHeight() const
	{
		return 14.f;
	}

  private:
	juce::RangedAudioParameter *param;
	juce::String suffix;
	SliderLookAndFeel lnf;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EQSliderComponent)
};
