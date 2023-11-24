#pragma once

#include "utils/SVGData.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

class BypassButtonComponent : public juce::Component
{
  public:
	BypassButtonComponent(juce::AudioProcessorValueTreeState &apvts, const juce::String &parameterID, SVGData svg);
	~BypassButtonComponent() override;

	void paint(juce::Graphics &g) override;
	void resized() override;

  private:
	juce::DrawableButton icon;
	std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> buttonAttachment;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BypassButtonComponent)
};
