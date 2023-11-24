#pragma once

#include "ui/LookAndFeel/CustomLookAndFeel.h"
#include "utils/SVGData.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

class BypassButtonComponent : public juce::ToggleButton
{
  public:
	BypassButtonComponent(juce::AudioProcessorValueTreeState &apvts, const juce::String &parameterID, SVGData svg);
	~BypassButtonComponent() override;

	/* void paint(juce::Graphics &g) override; */
	/* void resized() override; */

	std::unique_ptr<juce::Drawable> buttonImage;
	std::unique_ptr<juce::Drawable> buttonImageSelected;

  private:
	CustomLookAndFeel lnf;
	std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> buttonAttachment;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BypassButtonComponent)
};
