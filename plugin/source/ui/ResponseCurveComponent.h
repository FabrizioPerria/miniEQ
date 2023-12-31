#pragma once

#include "PluginProcessor.h"
#include "data/MonoFilter.h"
#include "data/SingleChannelSpectrum.h"
#include <juce_audio_processors/juce_audio_processors.h>

class ResponseCurveComponent : public juce::Component, public juce::AudioProcessorParameter::Listener, public juce::Timer
{
  public:
	ResponseCurveComponent(AudioPluginAudioProcessor &processorRef);
	~ResponseCurveComponent() override;

	void paint(juce::Graphics &g) override;
	void resized() override;

	void toggleAnalysisEnablement(bool enabled);

  private:
	void parameterValueChanged(int parameterIndex, float newValue) override;
	void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) override;
	void timerCallback() override;

	void updateChain();
	void updateResponseCurve();
	juce::Rectangle<float> getCanvasArea();
	juce::Rectangle<float> getRenderArea();
	juce::Rectangle<float> getAnalysisArea();

	juce::Atomic<bool> analysisEnabled {true};
	juce::Atomic<bool> parametersChanged {false};
	MonoFilter drawChannel;
	juce::Image background;
	AudioPluginAudioProcessor &pluginProcessor;

	SingleChannelSpectrum leftChannelSpectrum, rightChannelSpectrum;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ResponseCurveComponent)
};
