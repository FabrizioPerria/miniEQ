#pragma once
#include "PluginProcessor.h"
#include "ui/CustomSlider.h"
#include <JuceHeader.h>

//==============================================================================
class AudioPluginAudioProcessorEditor : public juce::AudioProcessorEditor, public AudioProcessorParameter::Listener, public juce::Timer
{
  public:
	explicit AudioPluginAudioProcessorEditor(AudioPluginAudioProcessor &);
	~AudioPluginAudioProcessorEditor() override;

	//==============================================================================
	void paint(juce::Graphics &) override;
	void resized() override;
	void parameterValueChanged(int parameterIndex, float newValue) override;
	void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) override;
	void timerCallback() override;

  private:
	// This reference is provided as a quick way for your editor to
	// access the processor object that created it.
	AudioPluginAudioProcessor &processorRef;

	juce::Atomic<bool> parametersChanged {false};

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
