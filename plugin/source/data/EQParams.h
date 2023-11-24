#pragma once

#include "utils/Slope.h"
#include <juce_audio_processors/juce_audio_processors.h>

class EQParams
{
  public:
	float lowCutFreq {0}, highCutFreq {0};
	float peakFreq {0}, peakGainDb {0}, peakQuality {0};
	Slope lowCutSlope {Slope::_12}, highCutSlope {Slope::_12};
	bool lowCutBypassed {false}, peakBypassed {false}, highCutBypassed {false};

	enum Value
	{
		LOWCUT_FREQUENCY,
		HIGHCUT_FREQUENCY,
		PEAK_FREQUENCY,
		PEAK_GAIN,
		PEAK_QUALITY,
		LOWCUT_SLOPE,
		HIGHCUT_SLOPE,
		LOWCUT_BYPASS,
		PEAK_BYPASS,
		HIGHCUT_BYPASS,
	};

	static EQParams getEQParams(juce::AudioProcessorValueTreeState &apvts);
	static const juce::String &toParamId(Value value);

  private:
	static float getParameter(juce::AudioProcessorValueTreeState &apvts, Value value);
};
