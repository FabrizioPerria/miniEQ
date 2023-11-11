#pragma once

#include "PluginProcessor.h"
#include "data/MonoFilter.h"
#include <juce_audio_processors/juce_audio_processors.h>

class ResponseCurveComponent : public juce::Component, public juce::AudioProcessorParameter::Listener, public juce::Timer
{
  public:
	ResponseCurveComponent(AudioPluginAudioProcessor &processorRef);
	~ResponseCurveComponent() override;

	void paint(juce::Graphics &g) override;
	void resized() override;

  private:
	void parameterValueChanged(int parameterIndex, float newValue) override;
	void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) override;
	void timerCallback() override;

	void updateChain();
	void updateResponseCurve();

	juce::Atomic<bool> parametersChanged {false};
	MonoFilter drawChannel;
	AudioPluginAudioProcessor &p;
};
