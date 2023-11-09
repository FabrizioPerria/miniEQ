#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "utils/MonoFilter.h"
#include "utils/EQParams.h"

//==============================================================================
class AudioPluginAudioProcessor : public juce::AudioProcessor
{
  public:
	//==============================================================================
	AudioPluginAudioProcessor();
	~AudioPluginAudioProcessor() override;

	//==============================================================================
	void prepareToPlay(double sampleRate, int samplesPerBlock) override;
	void releaseResources() override;

	bool isBusesLayoutSupported(const BusesLayout &layouts) const override;

	void processBlock(juce::AudioBuffer<float> &, juce::MidiBuffer &) override;
	using AudioProcessor::processBlock;

	//==============================================================================
	juce::AudioProcessorEditor *createEditor() override;
	bool hasEditor() const override;

	//==============================================================================
	const juce::String getName() const override;

	bool acceptsMidi() const override;
	bool producesMidi() const override;
	bool isMidiEffect() const override;
	double getTailLengthSeconds() const override;

	//==============================================================================
	int getNumPrograms() override;
	int getCurrentProgram() override;
	void setCurrentProgram(int index) override;
	const juce::String getProgramName(int index) override;
	void changeProgramName(int index, const juce::String &newName) override;

	//==============================================================================
	void getStateInformation(juce::MemoryBlock &destData) override;
	void setStateInformation(const void *data, int sizeInBytes) override;

	juce::AudioProcessorValueTreeState &getAPVTS();

  private:
	juce::AudioProcessorValueTreeState::ParameterLayout createParameters();
	EQParams getEQParams();
	void updatePeakFilter(const EQParams &params, const double sampleRate);
	void updateLowCutFilter(const EQParams &params, const double sampleRate);
	void updateHighCutFilter(const EQParams &params, const double sampleRate);

	juce::AudioProcessorValueTreeState apvts{*this, nullptr, "Parameters", createParameters()};
	MonoFilter rightChannelFilter, leftChannelFilter;

	//==============================================================================
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPluginAudioProcessor)
};
