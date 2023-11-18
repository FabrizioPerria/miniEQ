#pragma once

#include "EQSliderComponent.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>

class CustomSlider : public juce::Component
{
  public:
	CustomSlider(juce::AudioProcessorValueTreeState &state, juce::String parameterID, juce::String unit);

	void resized() override;

  private:
	EQSliderComponent component;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CustomSlider)
};
