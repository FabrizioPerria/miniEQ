#pragma once

#include "utils/SingleChannelSampleFifo.h"
#include <cassert>
#include <juce_core/juce_core.h>
#include <juce_dsp/juce_dsp.h>
#include <juce_gui_extra/juce_gui_extra.h>

#include <vector>

template <typename PathType> class FFTPathGenerator
{
  public:
	void generatePath(const std::vector<float> &renderData, juce::Rectangle<float> drawArea, int fftSize, float binWidth,
					  float negativeInfinity)
	{
		auto top = drawArea.getY();
		auto bottom = drawArea.getBottom();
		auto width = drawArea.getWidth();

		int numBins = fftSize / 2;
		PathType p;
		p.preallocateSpace(3 * (int)width);
		auto map = [bottom, top, negativeInfinity](float v) {
			auto tmp = juce::jmap(v, negativeInfinity, 0.f, bottom, top);
			assert(!std::isnan(tmp) && !std::isinf(tmp));
			return tmp;
		};
		auto y = map(renderData[0]);
		p.startNewSubPath(0, y);

		for (int bin = 1; bin < numBins; ++bin)
		{
			y = map(renderData[(size_t)bin]);
			auto binFreq = (float)bin * binWidth;
			auto normalizedBinX = juce::mapFromLog10(binFreq, 20.0f, 20000.0f);
			auto binX = normalizedBinX * width;

			p.lineTo(binX, y);
		}
		pathFifo.push(p);
	}

	bool getPath(PathType &path)
	{
		return pathFifo.pull(path);
	}

	int getNumPathsAvailable() const noexcept
	{
		return pathFifo.getNumAvailableForReading();
	}

  private:
	Fifo<PathType> pathFifo;
};
