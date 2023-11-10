#pragma once
#include "PluginProcessor.h"
#include "ui/CustomSlider.h"
#include <JuceHeader.h>

//==============================================================================
class AudioPluginAudioProcessorEditor : public juce::AudioProcessorEditor {
public:
	explicit AudioPluginAudioProcessorEditor(AudioPluginAudioProcessor&);
	~AudioPluginAudioProcessorEditor() override;

	//==============================================================================
	void paint(juce::Graphics&) override;
	void resized() override;

private:
	// This reference is provided as a quick way for your editor to
	// access the processor object that created it.
	AudioPluginAudioProcessor& processorRef;

	CustomSlider peakFreqSlider;
	CustomSlider peakGainSlider;
	CustomSlider peakQualitySlider;
	CustomSlider lowCutFreqSlider;
	CustomSlider highCutFreqSlider;
	CustomSlider lowCutSlopeSlider;
	CustomSlider highCutSlopeSlider;

	MonoFilter drawChannel;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPluginAudioProcessorEditor)
};
