#pragma once

struct EQParams
{
	float lowCutFreq{0}, highCutFreq{0};
	float peakFreq{0}, peakGainDb{0}, peakQuality{0};
	int lowCutSlope{0}, highCutSlope{0};
};
