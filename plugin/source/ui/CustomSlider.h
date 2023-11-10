#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

class CustomSlider : public juce::Slider {
public:
	CustomSlider(juce::AudioProcessorValueTreeState& state, juce::String parameterID);

	void paint(juce::Graphics& g) override;
	void resized() override;

private:
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;
};
