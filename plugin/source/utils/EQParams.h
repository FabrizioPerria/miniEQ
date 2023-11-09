#pragma once

#include "utils/Slope.h"

struct EQParams
{
	float lowCutFreq{0}, highCutFreq{0};
	float peakFreq{0}, peakGainDb{0}, peakQuality{0};
	Slope lowCutSlope{Slope::_12}, highCutSlope{Slope::_12};
};
