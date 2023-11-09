#pragma once

#include <juce_dsp/juce_dsp.h>

using Filter = juce::dsp::IIR::Filter<float>;
using ChainedFilters = juce::dsp::ProcessorChain<Filter, Filter, Filter, Filter>;
using MonoFilter = juce::dsp::ProcessorChain<ChainedFilters, Filter, ChainedFilters>;
