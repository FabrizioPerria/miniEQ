#pragma once

#include "utils/Slope.h"
#include <cassert>
#include <juce_audio_processors/juce_audio_processors.h>

class EQParams
{
  public:
	float lowCutFreq {0}, highCutFreq {0};
	float peakFreq {0}, peakGainDb {0}, peakQuality {0};
	Slope lowCutSlope {Slope::_12}, highCutSlope {Slope::_12};

	enum Value
	{
		LOWCUT_FREQUENCY,
		HIGHCUT_FREQUENCY,
		PEAK_FREQUENCY,
		PEAK_GAIN,
		PEAK_QUALITY,
		LOWCUT_SLOPE,
		HIGHCUT_SLOPE
	};

	static EQParams getEQParams(juce::AudioProcessorValueTreeState &apvts)
	{
		EQParams p;
		p.lowCutFreq = getParameter(apvts, Value::LOWCUT_FREQUENCY);
		p.highCutFreq = getParameter(apvts, Value::HIGHCUT_FREQUENCY);
		p.peakFreq = getParameter(apvts, Value::PEAK_FREQUENCY);
		p.peakGainDb = getParameter(apvts, Value::PEAK_GAIN);
		p.peakQuality = getParameter(apvts, Value::PEAK_QUALITY);
		p.lowCutSlope = Slope::fromFloat(getParameter(apvts, Value::LOWCUT_SLOPE));
		p.highCutSlope = Slope::fromFloat(getParameter(apvts, Value::HIGHCUT_SLOPE));

		return p;
	}

	static const juce::String &toParamId(Value value)
	{
		auto it = valueMap.find(value);
		if (it == valueMap.end())
			assert(false);
		return it->second;
	}

  private:
	static const std::map<EQParams::Value, juce::String> valueMap;

	static float getParameter(juce::AudioProcessorValueTreeState &apvts, Value value)
	{
		return apvts.getRawParameterValue(toParamId(value))->load();
	}
};
const std::map<EQParams::Value, juce::String> valueMap = {
	{EQParams::LOWCUT_FREQUENCY, "LOWCUT_FREQUENCY"}, {EQParams::HIGHCUT_FREQUENCY, "HIGHCUT_FREQUENCY"},
	{EQParams::PEAK_FREQUENCY, "PEAK_FREQUENCY"},	  {EQParams::PEAK_GAIN, "PEAK_GAIN"},
	{EQParams::PEAK_QUALITY, "PEAK_QUALITY"},		  {EQParams::LOWCUT_SLOPE, "LOWCUT_SLOPE"},
	{EQParams::HIGHCUT_SLOPE, "HIGHCUT_SLOPE"}};
