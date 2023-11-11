#pragma once

#include "data/EQParams.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

using Filter = juce::dsp::IIR::Filter<float>;
using ChainedFilters = juce::dsp::ProcessorChain<Filter, Filter, Filter, Filter>;
using MonoFilter = juce::dsp::ProcessorChain<ChainedFilters, Filter, ChainedFilters>;

enum ChainPositions
{
	LowCut,
	Peak,
	HighCut
};

using Coefficients = Filter::CoefficientsPtr;

void updateFilters(juce::AudioProcessorValueTreeState &apvts, MonoFilter &channel, const double sampleRate);
void updateCoefficients(Coefficients &old, const Coefficients &replacements);
template <int Index, typename ChainT, typename CoefficientT> void updateStageFilter(ChainT &filterChain, const CoefficientT &coefficients);
template <typename ChainT, typename CoefficientT>
void updateCutFilter(ChainT &filterChain, const CoefficientT &coefficients, const Slope &slope);
template <typename ChainT> void updatePeakFilter(const EQParams &parameters, ChainT &filter, const double sampleRate);
template <typename ChainT> void updateLowCutFilter(const EQParams &parameters, ChainT &filters, const double sampleRate);
template <typename ChainT> void updateHighCutFilter(const EQParams &parameters, ChainT &filter, const double sampleRate);
