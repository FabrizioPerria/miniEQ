#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "data/EQParams.h"
#include <JuceHeader.h>
#include <tracer.hpp>

//==============================================================================
AudioPluginAudioProcessor::AudioPluginAudioProcessor()
	: AudioProcessor(BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
						 .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
						 .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
	  )
{
}

AudioPluginAudioProcessor::~AudioPluginAudioProcessor()
{
}

//==============================================================================
const juce::String AudioPluginAudioProcessor::getName() const
{
	return JucePlugin_Name;
}

bool AudioPluginAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
	return true;
#else
	return false;
#endif
}

bool AudioPluginAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
	return true;
#else
	return false;
#endif
}

bool AudioPluginAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
	return true;
#else
	return false;
#endif
}

double AudioPluginAudioProcessor::getTailLengthSeconds() const
{
	return 0.0;
}

int AudioPluginAudioProcessor::getNumPrograms()
{
	return 1; // NB: some hosts don't cope very well if you tell them there are
			  // 0 programs, so this should be at least 1, even if you're not
			  // really implementing programs.
}

int AudioPluginAudioProcessor::getCurrentProgram()
{
	return 0;
}

void AudioPluginAudioProcessor::setCurrentProgram(int index)
{
	juce::ignoreUnused(index);
}

const juce::String AudioPluginAudioProcessor::getProgramName(int index)
{
	juce::ignoreUnused(index);
	return {};
}

void AudioPluginAudioProcessor::changeProgramName(int index, const juce::String &newName)
{
	juce::ignoreUnused(index, newName);
}

//==============================================================================
void AudioPluginAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
	juce::dsp::ProcessSpec spec;
	spec.sampleRate = sampleRate;
	spec.maximumBlockSize = (uint32_t)samplesPerBlock;
	spec.numChannels = 1;

	rightChannelFilter.prepare(spec);
	leftChannelFilter.prepare(spec);

	leftChannelFifo.prepare(samplesPerBlock);
	rightChannelFifo.prepare(samplesPerBlock);

	updateFilters(apvts, leftChannelFilter, sampleRate);
	updateFilters(apvts, rightChannelFilter, sampleRate);

	spec.numChannels = getTotalNumOutputChannels();
	osc.prepare(spec);
}

void AudioPluginAudioProcessor::releaseResources()
{
	// When playback stops, you can use this as an opportunity to free up any
	// spare memory, etc.
}

bool AudioPluginAudioProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const
{
#if JucePlugin_IsMidiEffect
	juce::ignoreUnused(layouts);
	return true;
#else
	// This is the place where you check if the layout is supported.
	// In this template code we only support mono or stereo.
	// Some plugin hosts, such as certain GarageBand versions, will only
	// load plugins that support stereo bus layouts.
	if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() &&
		layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
		return false;

// This checks if the input layout matches the output layout
#if !JucePlugin_IsSynth
	if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
		return false;
#endif

	return true;
#endif
}

void AudioPluginAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages)
{
	juce::ignoreUnused(midiMessages);

	juce::ScopedNoDenormals noDenormals;
	auto totalNumInputChannels = getTotalNumInputChannels();
	auto totalNumOutputChannels = getTotalNumOutputChannels();

	for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
		buffer.clear(i, 0, buffer.getNumSamples());

	updateFilters(apvts, leftChannelFilter, getSampleRate());
	updateFilters(apvts, rightChannelFilter, getSampleRate());

	auto block = juce::dsp::AudioBlock<float>(buffer);

	/* buffer.clear(); */
	/* osc.setFrequency(JUCE_LIVE_CONSTANT(1000)); */
	/* osc.process(juce::dsp::ProcessContextReplacing<float>(block)); */

	auto leftBlock = block.getSingleChannelBlock(0);
	auto rightBlock = block.getSingleChannelBlock(1);

	rightChannelFilter.process(juce::dsp::ProcessContextReplacing<float>(rightBlock));
	leftChannelFilter.process(juce::dsp::ProcessContextReplacing<float>(leftBlock));

	leftChannelFifo.update(buffer);
	rightChannelFifo.update(buffer);
}

//==============================================================================
bool AudioPluginAudioProcessor::hasEditor() const
{
	return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor *AudioPluginAudioProcessor::createEditor()
{
	return new AudioPluginAudioProcessorEditor(*this);
}

//==============================================================================
void AudioPluginAudioProcessor::getStateInformation(juce::MemoryBlock &destData)
{
	juce::MemoryOutputStream mos(destData, true);
	apvts.state.writeToStream(mos);
}

void AudioPluginAudioProcessor::setStateInformation(const void *data, int sizeInBytes)
{
	auto tree = juce::ValueTree::readFromData(data, (size_t)sizeInBytes);

	if (tree.isValid())
	{
		apvts.replaceState(tree);
		updateFilters(apvts, leftChannelFilter, getSampleRate());
		updateFilters(apvts, rightChannelFilter, getSampleRate());
	}
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter()
{
	return new AudioPluginAudioProcessor();
}

juce::AudioProcessorValueTreeState &AudioPluginAudioProcessor::getAPVTS()
{
	return apvts;
}

juce::AudioProcessorValueTreeState::ParameterLayout AudioPluginAudioProcessor::createParameters()
{
	using Range = juce::NormalisableRange<float>;

	juce::AudioProcessorValueTreeState::ParameterLayout params;

	params.add(std::make_unique<juce::AudioParameterFloat>(ParameterID {EQParams::toParamId(EQParams::LOWCUT_FREQUENCY), 1},
														   "Lowcut Frequency", Range(20.f, 20000.f, 1.f, 0.25f), 20.f));
	params.add(std::make_unique<juce::AudioParameterFloat>(ParameterID {EQParams::toParamId(EQParams::HIGHCUT_FREQUENCY), 1},
														   "Highcut Frequency", Range(20.f, 20000.f, 1.f, 0.25f), 20000.f));
	params.add(std::make_unique<juce::AudioParameterFloat>(ParameterID {EQParams::toParamId(EQParams::PEAK_FREQUENCY), 1}, "Peak Frequency",
														   Range(20.f, 20000.f, 1.f, 0.25f), 750.f));
	params.add(std::make_unique<juce::AudioParameterFloat>(ParameterID {EQParams::toParamId(EQParams::PEAK_GAIN), 1}, "Peak Gain",
														   Range(-24.f, 24.f, 0.5f, 1.f), 0.f));
	params.add(std::make_unique<juce::AudioParameterFloat>(ParameterID {EQParams::toParamId(EQParams::PEAK_QUALITY), 1}, "Peak Quality",
														   Range(0.1f, 10.f, 0.05f, 1.f), 1.f));
	params.add(std::make_unique<juce::AudioParameterChoice>(ParameterID {EQParams::toParamId(EQParams::LOWCUT_SLOPE), 1}, "Lowcut Slope",
															Slope::toArray(), 0));
	params.add(std::make_unique<juce::AudioParameterChoice>(ParameterID {EQParams::toParamId(EQParams::HIGHCUT_SLOPE), 1}, "Highcut Slope",
															Slope::toArray(), 0));
	params.add(
		std::make_unique<juce::AudioParameterBool>(ParameterID {EQParams::toParamId(EQParams::LOWCUT_BYPASS), 1}, "Lowcut Bypass", false));
	params.add(
		std::make_unique<juce::AudioParameterBool>(ParameterID {EQParams::toParamId(EQParams::PEAK_BYPASS), 1}, "Peak Bypass", false));
	params.add(std::make_unique<juce::AudioParameterBool>(ParameterID {EQParams::toParamId(EQParams::HIGHCUT_BYPASS), 1}, "Highcut Bypass",
														  false));
	params.add(std::make_unique<juce::AudioParameterBool>(ParameterID {"ANALYZER_ENABLED", 1}, "Analyzer Enabled", true));

	return params;
}
