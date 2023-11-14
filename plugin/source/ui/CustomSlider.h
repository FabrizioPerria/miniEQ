#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

struct SliderLookAndFeel : public juce::LookAndFeel_V4
{
	void drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle,
						  float rotaryEndAngle, juce::Slider &slider) override
	{
		juce::LookAndFeel_V4::drawRotarySlider(g, x, y, width, height, sliderPosProportional, rotaryStartAngle, rotaryEndAngle, slider);
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
		juce::Slider::paint(g);
	}

	void resized() override
	{
		juce::Slider::resized();
	}

	juce::Rectangle<int> getSliderBounds() const;
	juce::String getDisplayText() const;

  private:
	juce::RangedAudioParameter *param;
	juce::String suffix;
	SliderLookAndFeel lnf;
	const int textHeight = 14;

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
