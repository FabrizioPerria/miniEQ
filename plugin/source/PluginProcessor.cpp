#include "PluginProcessor.h"
#include "PluginEditor.h"
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
	dsp::Gain<float> g;
	juce::dsp::Reverb reverb;
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
	return 1; // NB: some hosts don't cope very well if you tell them there are 0 programs,
			  // so this should be at least 1, even if you're not really implementing programs.
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
	spec.maximumBlockSize = ( uint32_t ) samplesPerBlock;
	spec.numChannels = 1;

	rightChannelFilter.prepare(spec);
	leftChannelFilter.prepare(spec);

	auto parameters = getEQParams();

	auto peakCoefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(
		sampleRate, parameters.peakFreq, parameters.peakQuality, juce::Decibels::decibelsToGain(parameters.peakGainDb));

	*leftChannelFilter.get<ChainPositions::Peak>().coefficients = *peakCoefficients;
	*rightChannelFilter.get<ChainPositions::Peak>().coefficients = *peakCoefficients;

	auto lowCutCoefficients = juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(
		parameters.lowCutFreq, sampleRate, 2 * (parameters.lowCutSlope + 1));

	auto &leftLowCut = leftChannelFilter.get<ChainPositions::LowCut>();
	auto &rightLowCut = rightChannelFilter.get<ChainPositions::LowCut>();

	leftLowCut.setBypassed<0>(true);
	rightLowCut.setBypassed<0>(true);
	leftLowCut.setBypassed<1>(true);
	rightLowCut.setBypassed<1>(true);
	leftLowCut.setBypassed<2>(true);
	rightLowCut.setBypassed<2>(true);
	leftLowCut.setBypassed<3>(true);
	rightLowCut.setBypassed<3>(true);

	switch (parameters.lowCutSlope)
	{
	case Slope::_12:
		*leftLowCut.get<0>().coefficients = *lowCutCoefficients[0];
		*rightLowCut.get<0>().coefficients = *lowCutCoefficients[0];
		leftLowCut.template setBypassed<0>(false);
		rightLowCut.template setBypassed<0>(false);
		break;
	case Slope::_24:
		*leftLowCut.get<0>().coefficients = *lowCutCoefficients[0];
		*rightLowCut.get<0>().coefficients = *lowCutCoefficients[0];
		*leftLowCut.get<1>().coefficients = *lowCutCoefficients[1];
		*rightLowCut.get<1>().coefficients = *lowCutCoefficients[1];
		leftLowCut.template setBypassed<0>(false);
		rightLowCut.template setBypassed<0>(false);
		leftLowCut.template setBypassed<1>(false);
		rightLowCut.template setBypassed<1>(false);
		break;
	case Slope::_36:
		*leftLowCut.get<0>().coefficients = *lowCutCoefficients[0];
		*rightLowCut.get<0>().coefficients = *lowCutCoefficients[0];
		*leftLowCut.get<1>().coefficients = *lowCutCoefficients[1];
		*rightLowCut.get<1>().coefficients = *lowCutCoefficients[1];
		*leftLowCut.get<2>().coefficients = *lowCutCoefficients[2];
		*rightLowCut.get<2>().coefficients = *lowCutCoefficients[2];
		leftLowCut.template setBypassed<0>(false);
		rightLowCut.template setBypassed<0>(false);
		leftLowCut.template setBypassed<1>(false);
		rightLowCut.template setBypassed<1>(false);
		leftLowCut.template setBypassed<2>(false);
		rightLowCut.template setBypassed<2>(false);
		break;
	case Slope::_48:
		*leftLowCut.get<0>().coefficients = *lowCutCoefficients[0];
		*rightLowCut.get<0>().coefficients = *lowCutCoefficients[0];
		*leftLowCut.get<1>().coefficients = *lowCutCoefficients[1];
		*rightLowCut.get<1>().coefficients = *lowCutCoefficients[1];
		*leftLowCut.get<2>().coefficients = *lowCutCoefficients[2];
		*rightLowCut.get<2>().coefficients = *lowCutCoefficients[2];
		*leftLowCut.get<3>().coefficients = *lowCutCoefficients[3];
		*rightLowCut.get<3>().coefficients = *lowCutCoefficients[3];
		leftLowCut.template setBypassed<0>(false);
		rightLowCut.template setBypassed<0>(false);
		leftLowCut.template setBypassed<1>(false);
		rightLowCut.template setBypassed<1>(false);
		leftLowCut.template setBypassed<2>(false);
		rightLowCut.template setBypassed<2>(false);
		leftLowCut.template setBypassed<3>(false);
		rightLowCut.template setBypassed<3>(false);
		break;
	default:
		jassertfalse;
		break;
	}
	auto highCutCoefficients = juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(
		parameters.highCutFreq, sampleRate, 2 * (parameters.highCutSlope + 1));

	auto &leftHighCut = leftChannelFilter.get<ChainPositions::HighCut>();
	auto &rightHighCut = rightChannelFilter.get<ChainPositions::HighCut>();

	leftHighCut.setBypassed<0>(true);
	rightHighCut.setBypassed<0>(true);
	leftHighCut.setBypassed<1>(true);
	rightHighCut.setBypassed<1>(true);
	leftHighCut.setBypassed<2>(true);
	rightHighCut.setBypassed<2>(true);
	leftHighCut.setBypassed<3>(true);
	rightHighCut.setBypassed<3>(true);

	switch (parameters.highCutSlope)
	{
	case Slope::_12:
		*leftHighCut.get<0>().coefficients = *highCutCoefficients[0];
		*rightHighCut.get<0>().coefficients = *highCutCoefficients[0];
		leftHighCut.template setBypassed<0>(false);
		rightHighCut.template setBypassed<0>(false);
		break;
	case Slope::_24:
		*leftHighCut.get<0>().coefficients = *highCutCoefficients[0];
		*rightHighCut.get<0>().coefficients = *highCutCoefficients[0];
		*leftHighCut.get<1>().coefficients = *highCutCoefficients[1];
		*rightHighCut.get<1>().coefficients = *highCutCoefficients[1];
		leftHighCut.template setBypassed<0>(false);
		rightHighCut.template setBypassed<0>(false);
		leftHighCut.template setBypassed<1>(false);
		rightHighCut.template setBypassed<1>(false);
		break;
	case Slope::_36:
		*leftHighCut.get<0>().coefficients = *highCutCoefficients[0];
		*rightHighCut.get<0>().coefficients = *highCutCoefficients[0];
		*leftHighCut.get<1>().coefficients = *highCutCoefficients[1];
		*rightHighCut.get<1>().coefficients = *highCutCoefficients[1];
		*leftHighCut.get<2>().coefficients = *highCutCoefficients[2];
		*rightHighCut.get<2>().coefficients = *highCutCoefficients[2];
		leftHighCut.template setBypassed<0>(false);
		rightHighCut.template setBypassed<0>(false);
		leftHighCut.template setBypassed<1>(false);
		rightHighCut.template setBypassed<1>(false);
		leftHighCut.template setBypassed<2>(false);
		rightHighCut.template setBypassed<2>(false);
		break;
	case Slope::_48:
		*leftHighCut.get<0>().coefficients = *highCutCoefficients[0];
		*rightHighCut.get<0>().coefficients = *highCutCoefficients[0];
		*leftHighCut.get<1>().coefficients = *highCutCoefficients[1];
		*rightHighCut.get<1>().coefficients = *highCutCoefficients[1];
		*leftHighCut.get<2>().coefficients = *highCutCoefficients[2];
		*rightHighCut.get<2>().coefficients = *highCutCoefficients[2];
		*leftHighCut.get<3>().coefficients = *highCutCoefficients[3];
		*rightHighCut.get<3>().coefficients = *highCutCoefficients[3];
		leftHighCut.template setBypassed<0>(false);
		rightHighCut.template setBypassed<0>(false);
		leftHighCut.template setBypassed<1>(false);
		rightHighCut.template setBypassed<1>(false);
		leftHighCut.template setBypassed<2>(false);
		rightHighCut.template setBypassed<2>(false);
		leftHighCut.template setBypassed<3>(false);
		rightHighCut.template setBypassed<3>(false);
		break;
	default:
		jassertfalse;
		break;
	}
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

	auto parameters = getEQParams();

	auto peakCoefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(
		getSampleRate(), parameters.peakFreq, parameters.peakQuality,
		juce::Decibels::decibelsToGain(parameters.peakGainDb));

	*leftChannelFilter.get<ChainPositions::Peak>().coefficients = *peakCoefficients;
	*rightChannelFilter.get<ChainPositions::Peak>().coefficients = *peakCoefficients;

	auto lowCutCoefficients = juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(
		parameters.lowCutFreq, getSampleRate(), 2 * (parameters.lowCutSlope + 1));

	auto &leftLowCut = leftChannelFilter.get<ChainPositions::LowCut>();
	auto &rightLowCut = rightChannelFilter.get<ChainPositions::LowCut>();

	leftLowCut.setBypassed<0>(true);
	rightLowCut.setBypassed<0>(true);
	leftLowCut.setBypassed<1>(true);
	rightLowCut.setBypassed<1>(true);
	leftLowCut.setBypassed<2>(true);
	rightLowCut.setBypassed<2>(true);
	leftLowCut.setBypassed<3>(true);
	rightLowCut.setBypassed<3>(true);

	switch (parameters.lowCutSlope)
	{
	case Slope::_12:
		*leftLowCut.get<0>().coefficients = *lowCutCoefficients[0];
		*rightLowCut.get<0>().coefficients = *lowCutCoefficients[0];
		leftLowCut.template setBypassed<0>(false);
		rightLowCut.template setBypassed<0>(false);
		break;
	case Slope::_24:
		*leftLowCut.get<0>().coefficients = *lowCutCoefficients[0];
		*rightLowCut.get<0>().coefficients = *lowCutCoefficients[0];
		*leftLowCut.get<1>().coefficients = *lowCutCoefficients[1];
		*rightLowCut.get<1>().coefficients = *lowCutCoefficients[1];
		leftLowCut.template setBypassed<0>(false);
		rightLowCut.template setBypassed<0>(false);
		leftLowCut.template setBypassed<1>(false);
		rightLowCut.template setBypassed<1>(false);
		break;
	case Slope::_36:
		*leftLowCut.get<0>().coefficients = *lowCutCoefficients[0];
		*rightLowCut.get<0>().coefficients = *lowCutCoefficients[0];
		*leftLowCut.get<1>().coefficients = *lowCutCoefficients[1];
		*rightLowCut.get<1>().coefficients = *lowCutCoefficients[1];
		*leftLowCut.get<2>().coefficients = *lowCutCoefficients[2];
		*rightLowCut.get<2>().coefficients = *lowCutCoefficients[2];
		leftLowCut.template setBypassed<0>(false);
		rightLowCut.template setBypassed<0>(false);
		leftLowCut.template setBypassed<1>(false);
		rightLowCut.template setBypassed<1>(false);
		leftLowCut.template setBypassed<2>(false);
		rightLowCut.template setBypassed<2>(false);
		break;
	case Slope::_48:
		*leftLowCut.get<0>().coefficients = *lowCutCoefficients[0];
		*rightLowCut.get<0>().coefficients = *lowCutCoefficients[0];
		*leftLowCut.get<1>().coefficients = *lowCutCoefficients[1];
		*rightLowCut.get<1>().coefficients = *lowCutCoefficients[1];
		*leftLowCut.get<2>().coefficients = *lowCutCoefficients[2];
		*rightLowCut.get<2>().coefficients = *lowCutCoefficients[2];
		*leftLowCut.get<3>().coefficients = *lowCutCoefficients[3];
		*rightLowCut.get<3>().coefficients = *lowCutCoefficients[3];
		leftLowCut.template setBypassed<0>(false);
		rightLowCut.template setBypassed<0>(false);
		leftLowCut.template setBypassed<1>(false);
		rightLowCut.template setBypassed<1>(false);
		leftLowCut.template setBypassed<2>(false);
		rightLowCut.template setBypassed<2>(false);
		leftLowCut.template setBypassed<3>(false);
		rightLowCut.template setBypassed<3>(false);
		break;
	default:
		jassertfalse;
		break;
	}

	auto highCutCoefficients = juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(
		parameters.highCutFreq, getSampleRate(), 2 * (parameters.highCutSlope + 1));

	auto &leftHighCut = leftChannelFilter.get<ChainPositions::HighCut>();
	auto &rightHighCut = rightChannelFilter.get<ChainPositions::HighCut>();

	leftHighCut.setBypassed<0>(true);
	rightHighCut.setBypassed<0>(true);
	leftHighCut.setBypassed<1>(true);
	rightHighCut.setBypassed<1>(true);
	leftHighCut.setBypassed<2>(true);
	rightHighCut.setBypassed<2>(true);
	leftHighCut.setBypassed<3>(true);
	rightHighCut.setBypassed<3>(true);

	switch (parameters.highCutSlope)
	{
	case Slope::_12:
		*leftHighCut.get<0>().coefficients = *highCutCoefficients[0];
		*rightHighCut.get<0>().coefficients = *highCutCoefficients[0];
		leftHighCut.template setBypassed<0>(false);
		rightHighCut.template setBypassed<0>(false);
		break;
	case Slope::_24:
		*leftHighCut.get<0>().coefficients = *highCutCoefficients[0];
		*rightHighCut.get<0>().coefficients = *highCutCoefficients[0];
		*leftHighCut.get<1>().coefficients = *highCutCoefficients[1];
		*rightHighCut.get<1>().coefficients = *highCutCoefficients[1];
		leftHighCut.template setBypassed<0>(false);
		rightHighCut.template setBypassed<0>(false);
		leftHighCut.template setBypassed<1>(false);
		rightHighCut.template setBypassed<1>(false);
		break;
	case Slope::_36:
		*leftHighCut.get<0>().coefficients = *highCutCoefficients[0];
		*rightHighCut.get<0>().coefficients = *highCutCoefficients[0];
		*leftHighCut.get<1>().coefficients = *highCutCoefficients[1];
		*rightHighCut.get<1>().coefficients = *highCutCoefficients[1];
		*leftHighCut.get<2>().coefficients = *highCutCoefficients[2];
		*rightHighCut.get<2>().coefficients = *highCutCoefficients[2];
		leftHighCut.template setBypassed<0>(false);
		rightHighCut.template setBypassed<0>(false);
		leftHighCut.template setBypassed<1>(false);
		rightHighCut.template setBypassed<1>(false);
		leftHighCut.template setBypassed<2>(false);
		rightHighCut.template setBypassed<2>(false);
		break;
	case Slope::_48:
		*leftHighCut.get<0>().coefficients = *highCutCoefficients[0];
		*rightHighCut.get<0>().coefficients = *highCutCoefficients[0];
		*leftHighCut.get<1>().coefficients = *highCutCoefficients[1];
		*rightHighCut.get<1>().coefficients = *highCutCoefficients[1];
		*leftHighCut.get<2>().coefficients = *highCutCoefficients[2];
		*rightHighCut.get<2>().coefficients = *highCutCoefficients[2];
		*leftHighCut.get<3>().coefficients = *highCutCoefficients[3];
		*rightHighCut.get<3>().coefficients = *highCutCoefficients[3];
		leftHighCut.template setBypassed<0>(false);
		rightHighCut.template setBypassed<0>(false);
		leftHighCut.template setBypassed<1>(false);
		rightHighCut.template setBypassed<1>(false);
		leftHighCut.template setBypassed<2>(false);
		rightHighCut.template setBypassed<2>(false);
		leftHighCut.template setBypassed<3>(false);
		rightHighCut.template setBypassed<3>(false);
		break;
	default:
		jassertfalse;
		break;
	}

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
	return new GenericAudioProcessorEditor(*this); // TODO: replace with your editor class
												   /* return new AudioPluginAudioProcessorEditor(*this); */
}

//==============================================================================
void AudioPluginAudioProcessor::getStateInformation(juce::MemoryBlock &destData)
{
	// You should use this method to store your parameters in the memory block.
	// You could do that either as raw data, or use the XML or ValueTree classes
	// as intermediaries to make it easy to save and load complex data.
	juce::ignoreUnused(destData);
}

void AudioPluginAudioProcessor::setStateInformation(const void *data, int sizeInBytes)
{
	// You should use this method to restore your parameters from this memory block,
	// whose contents will have been created by the getStateInformation() call.
	juce::ignoreUnused(data, sizeInBytes);
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
	juce::AudioProcessorValueTreeState::ParameterLayout params;

	using Range = juce::NormalisableRange<float>;

	params.add(std::make_unique<juce::AudioParameterFloat>(ParameterID{"LOWCUT_FREQUENCY", 1}, "Lowcut Frequency",
														   Range(20.f, 20000.f, 1.f, 0.25f), 20.f));
	params.add(std::make_unique<juce::AudioParameterFloat>(ParameterID{"HIGHCUT_FREQUENCY", 1}, "Highcut Frequency",
														   Range(20.f, 20000.f, 1.f, 0.25f), 20000.f));
	params.add(std::make_unique<juce::AudioParameterFloat>(ParameterID{"PEAK_FREQUENCY", 1}, "Peak Frequency",
														   Range(20.f, 20000.f, 1.f, 0.25f), 750.f));
	params.add(std::make_unique<juce::AudioParameterFloat>(ParameterID{"PEAK_GAIN", 1}, "Peak Gain",
														   Range(-24.f, 24.f, 0.5f, 1.f), 0.f));
	params.add(std::make_unique<juce::AudioParameterFloat>(ParameterID{"PEAK_QUALITY", 1}, "Peak Quality",
														   Range(0.1f, 10.f, 0.05f, 1.f), 1.f));
	params.add(std::make_unique<juce::AudioParameterChoice>(ParameterID{"LOWCUT_SLOPE", 1}, "Lowcut Slope",
															Slope::toArray(), 0));
	params.add(std::make_unique<juce::AudioParameterChoice>(ParameterID{"HIGHCUT_SLOPE", 1}, "Highcut Slope",
															Slope::toArray(), 0));

	return params;
}

EQParams AudioPluginAudioProcessor::getEQParams()
{
	EQParams p;
	p.lowCutFreq = apvts.getRawParameterValue("LOWCUT_FREQUENCY")->load();
	p.highCutFreq = apvts.getRawParameterValue("HIGHCUT_FREQUENCY")->load();
	p.peakFreq = apvts.getRawParameterValue("PEAK_FREQUENCY")->load();
	p.peakGainDb = apvts.getRawParameterValue("PEAK_GAIN")->load();
	p.peakQuality = apvts.getRawParameterValue("PEAK_QUALITY")->load();
	p.lowCutSlope = Slope::fromFloat(apvts.getRawParameterValue("LOWCUT_SLOPE")->load());
	p.highCutSlope = Slope::fromFloat(apvts.getRawParameterValue("HIGHCUT_SLOPE")->load());

	return p;
}
