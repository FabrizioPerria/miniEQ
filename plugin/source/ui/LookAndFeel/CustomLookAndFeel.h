#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>

struct CustomLookAndFeel : public juce::LookAndFeel_V4
{
	void drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle,
						  float rotaryEndAngle, juce::Slider &slider) override;

	void drawToggleButton(juce::Graphics &g, juce::ToggleButton &toggleButton, bool shouldDrawButtonAsHighlighted,
						  bool shouldDrawButtonAsDown) override;
};
