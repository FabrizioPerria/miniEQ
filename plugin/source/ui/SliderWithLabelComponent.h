#pragma once

#include "ui/LookAndFeel/CustomLookAndFeel.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>

class SliderWithLabelsComponent : public juce::Slider
{
  public:
	SliderWithLabelsComponent(juce::RangedAudioParameter &parameter, const juce::String &unit);

	~SliderWithLabelsComponent() override;

	void paint(juce::Graphics &g) override;

	void drawBounds(juce::Graphics &g, juce::Colour color, juce::Rectangle<int> bounds);

	void resized() override;

	juce::Rectangle<int> getSliderBounds() const;

	juce::String getDisplayText() const;

	float getTextHeight() const;

	struct LabelWithPosition
	{
		float position;
		juce::String label;
	};

	juce::Array<LabelWithPosition> labels;

  private:
	juce::RangedAudioParameter *param;
	juce::String suffix;
	CustomLookAndFeel lnf;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SliderWithLabelsComponent)
};
