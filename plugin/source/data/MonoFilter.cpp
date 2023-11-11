#include "data/MonoFilter.h"

void updateCoefficients(Coefficients &old, const Coefficients &replacements)
{
	*old = *replacements;
}

template <int Index, typename ChainT, typename CoefficientT> void updateStageFilter(ChainT &filterChain, const CoefficientT &coefficients)
{
	updateCoefficients(filterChain.template get<Index>().coefficients, coefficients[Index]);
	filterChain.template setBypassed<Index>(false);
}

template <typename ChainT, typename CoefficientT>
void updateCutFilter(ChainT &filterChain, const CoefficientT &coefficients, const Slope &slope)
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

template <typename ChainT> void updatePeakFilter(const EQParams &parameters, ChainT &filter, const double sampleRate)
{
	auto peakCoefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, parameters.peakFreq, parameters.peakQuality,
																				juce::Decibels::decibelsToGain(parameters.peakGainDb));
	updateCoefficients(filter.coefficients, peakCoefficients);
}

template <typename ChainT> void updateLowCutFilter(const EQParams &parameters, ChainT &filters, const double sampleRate)
{
	auto lowCutCoefficients = juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(parameters.lowCutFreq, sampleRate,
																										  2 * (parameters.lowCutSlope + 1));
	updateCutFilter(filters, lowCutCoefficients, parameters.lowCutSlope);
}

template <typename ChainT> void updateHighCutFilter(const EQParams &parameters, ChainT &filter, const double sampleRate)
{
	auto highCutCoefficients = juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(
		parameters.highCutFreq, sampleRate, 2 * (parameters.highCutSlope + 1));
	updateCutFilter(filter, highCutCoefficients, parameters.highCutSlope);
}

void updateFilters(juce::AudioProcessorValueTreeState &apvts, MonoFilter &channel, const double sampleRate)
{
	auto parameters = EQParams::getEQParams(apvts);

	updatePeakFilter(parameters, channel.get<ChainPositions::Peak>(), sampleRate);
	updateLowCutFilter(parameters, channel.get<ChainPositions::LowCut>(), sampleRate);
	updateHighCutFilter(parameters, channel.get<ChainPositions::HighCut>(), sampleRate);
}
