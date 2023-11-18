#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>

struct SliderLookAndFeel : public juce::LookAndFeel_V4
{
	void drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle,
						  float rotaryEndAngle, juce::Slider &slider) override;
};
