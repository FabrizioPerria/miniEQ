#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>

struct SliderLookAndFeel : public juce::LookAndFeel_V4
{
	void drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle,
						  float rotaryEndAngle, juce::Slider &slider) override
	{
		/* juce::LookAndFeel_V4::drawRotarySlider(g, x, y, width, height, sliderPosProportional, rotaryStartAngle, rotaryEndAngle, slider);
		 */
		auto sliderBounds = juce::Rectangle<float>(x, y, width, height);
		g.setColour(juce::Colours::orange); 
		g.drawEllipse(sliderBounds, 1.0f);
		g.fillEllipse(sliderBounds);

		auto center = sliderBounds.getCentre();
		juce::Path path;
		juce::Rectangle<float> pointer;
		const int margin = 2;
		pointer.setLeft(center.getX() - margin);
		pointer.setRight(center.getX() + margin);
		pointer.setTop(sliderBounds.getY() - margin);
		pointer.setBottom(center.getY());

		path.addRectangle(pointer);
		jassert(rotaryStartAngle < rotaryEndAngle);

		auto rotateAngle = juce::jmap(sliderPosProportional, 0.0f, 1.0f, rotaryStartAngle, rotaryEndAngle);
		path.applyTransform(juce::AffineTransform().rotated(rotateAngle, center.getX(), center.getY()));
		g.setColour(juce::Colours::green);
		g.fillPath(path);

		g.setColour(juce::Colours::black);
		g.fillEllipse(center.getX() - 2, center.getY() - 2, 4, 4);
	}
};

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
		
		drawBounds(g, juce::Colours::white, sliderBounds);
		drawBounds(g, juce::Colours::orange, getLocalBounds());

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

	juce::String getDisplayText() const;

	int getTextHeight() const
	{
		return 14;
	}

  private:
	juce::RangedAudioParameter *param;
	juce::String suffix;
	SliderLookAndFeel lnf;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EQSliderComponent)
};

class CustomSlider : public juce::Component
{
  public:
	CustomSlider(juce::AudioProcessorValueTreeState &state, juce::String parameterID, juce::String unit);

	void paint(juce::Graphics &g) override;
	void resized() override;

  private:
	EQSliderComponent component;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CustomSlider)
};
