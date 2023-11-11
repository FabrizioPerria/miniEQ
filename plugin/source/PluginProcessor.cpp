#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "utils/EQParams.h"
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

	updateFilters(sampleRate);
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

	updateFilters(getSampleRate());

	auto block = juce::dsp::AudioBlock<float>(buffer);
	auto leftBlock = block.getSingleChannelBlock(0);
	auto rightBlock = block.getSingleChannelBlock(1);

	rightChannelFilter.process(juce::dsp::ProcessContextReplacing<float>(rightBlock));
	leftChannelFilter.process(juce::dsp::ProcessContextReplacing<float>(leftBlock));
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
		updateFilters(getSampleRate());
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

	params.add(std::make_unique<juce::AudioParameterFloat>(ParameterID {EQParams::LOWCUT_FREQUENCY, 1}, "Lowcut Frequency",
														   Range(20.f, 20000.f, 1.f, 0.25f), 20.f));
	params.add(std::make_unique<juce::AudioParameterFloat>(ParameterID {EQParams::HIGHCUT_FREQUENCY, 1}, "Highcut Frequency",
														   Range(20.f, 20000.f, 1.f, 0.25f), 20000.f));
	params.add(std::make_unique<juce::AudioParameterFloat>(ParameterID {EQParams::PEAK_FREQUENCY, 1}, "Peak Frequency",
														   Range(20.f, 20000.f, 1.f, 0.25f), 750.f));
	params.add(
		std::make_unique<juce::AudioParameterFloat>(ParameterID {EQParams::PEAK_GAIN, 1}, "Peak Gain", Range(-24.f, 24.f, 0.5f, 1.f), 0.f));
	params.add(std::make_unique<juce::AudioParameterFloat>(ParameterID {EQParams::PEAK_QUALITY, 1}, "Peak Quality",
														   Range(0.1f, 10.f, 0.05f, 1.f), 1.f));
	params.add(std::make_unique<juce::AudioParameterChoice>(ParameterID {EQParams::LOWCUT_SLOPE, 1}, "Lowcut Slope", Slope::toArray(), 0));
	params.add(
		std::make_unique<juce::AudioParameterChoice>(ParameterID {EQParams::HIGHCUT_SLOPE, 1}, "Highcut Slope", Slope::toArray(), 0));

	return params;
}

void AudioPluginAudioProcessor::updateFilters(const double sampleRate)
{
	auto parameters = EQParams::getEQParams(apvts);

	updatePeakFilter(parameters, sampleRate);
	updateLowCutFilter(parameters, sampleRate);
	updateHighCutFilter(parameters, sampleRate);
}

void AudioPluginAudioProcessor::updatePeakFilter(const EQParams &parameters, const double sampleRate)
{
	auto peakCoefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, parameters.peakFreq, parameters.peakQuality,
																				juce::Decibels::decibelsToGain(parameters.peakGainDb));

	auto &leftPeak = leftChannelFilter.get<ChainPositions::Peak>();
	auto &rightPeak = rightChannelFilter.get<ChainPositions::Peak>();

	updateCoefficients(leftPeak.coefficients, peakCoefficients);
	updateCoefficients(rightPeak.coefficients, peakCoefficients);
}

void AudioPluginAudioProcessor::updateLowCutFilter(const EQParams &parameters, const double sampleRate)
{
	auto lowCutCoefficients = juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(parameters.lowCutFreq, sampleRate,
																										  2 * (parameters.lowCutSlope + 1));

	auto &leftLowCut = leftChannelFilter.get<ChainPositions::LowCut>();
	auto &rightLowCut = rightChannelFilter.get<ChainPositions::LowCut>();

	updateCutFilter(leftLowCut, lowCutCoefficients, parameters.lowCutSlope);
	updateCutFilter(rightLowCut, lowCutCoefficients, parameters.lowCutSlope);
}

void AudioPluginAudioProcessor::updateHighCutFilter(const EQParams &parameters, const double sampleRate)
{
	auto highCutCoefficients = juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(
		parameters.highCutFreq, sampleRate, 2 * (parameters.highCutSlope + 1));

	auto &leftHighCut = leftChannelFilter.get<ChainPositions::HighCut>();
	auto &rightHighCut = rightChannelFilter.get<ChainPositions::HighCut>();

	updateCutFilter(leftHighCut, highCutCoefficients, parameters.highCutSlope);
	updateCutFilter(rightHighCut, highCutCoefficients, parameters.highCutSlope);
}

void AudioPluginAudioProcessor::updateCoefficients(Coefficients &old, const Coefficients &replacements)
{
	*old = *replacements;
}

template <int Index, typename ChainT, typename CoefficientT>
void AudioPluginAudioProcessor::updateStageFilter(ChainT &filterChain, const CoefficientT &coefficients)
{
	updateCoefficients(filterChain.template get<Index>().coefficients, coefficients[Index]);
	filterChain.template setBypassed<Index>(false);
}

template <typename ChainT, typename CoefficientT>
void AudioPluginAudioProcessor::updateCutFilter(ChainT &filterChain, const CoefficientT &coefficients, const Slope &slope)
{
	filterChain.template setBypassed<0>(true);
	filterChain.template setBypassed<1>(true);
	filterChain.template setBypassed<2>(true);
	filterChain.template setBypassed<3>(true);

	switch (slope)
	{
	case Slope::_48:
		updateStageFilter<3>(filterChain, coefficients);
	case Slope::_36:
		updateStageFilter<2>(filterChain, coefficients);
	case Slope::_24:
		updateStageFilter<1>(filterChain, coefficients);
	case Slope::_12:
		updateStageFilter<0>(filterChain, coefficients);
		break;
	default:
		jassertfalse;
		break;
	}
}
