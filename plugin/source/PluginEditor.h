#pragma once
#include "PluginProcessor.h"
#include "ui/EQSliderComponent.h"
#include "ui/ResponseCurveComponent.h"
#include <JuceHeader.h>

//==============================================================================
class AudioPluginAudioProcessorEditor : public juce::AudioProcessorEditor
{
  public:
	explicit AudioPluginAudioProcessorEditor(AudioPluginAudioProcessor &);
	~AudioPluginAudioProcessorEditor() override;

	//==============================================================================
	void paint(juce::Graphics &) override;
	void resized() override;

  private:
	// This reference is provided as a quick way for your editor to
	// access the processor object that created it.
	AudioPluginAudioProcessor &processorRef;

	ResponseCurveComponent responseCurveComponent;
	EQSliderComponent peakFreqSlider;
	EQSliderComponent peakGainSlider;
	EQSliderComponent peakQualitySlider;
	EQSliderComponent lowCutFreqSlider;
	EQSliderComponent highCutFreqSlider;
	EQSliderComponent lowCutSlopeSlider;
	EQSliderComponent highCutSlopeSlider;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPluginAudioProcessorEditor)
};
