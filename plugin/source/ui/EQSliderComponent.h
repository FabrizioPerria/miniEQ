#pragma once

#include "ui/SliderWithLabelComponent.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>

class EQSliderComponent: public juce::Component
{
  public:
	EQSliderComponent(juce::AudioProcessorValueTreeState &state, juce::String parameterID, juce::String unit);

	void resized() override;
	
	void addLabel(float position, juce::String label);

  private:
	SliderWithLabelsComponent component;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EQSliderComponent)
};
