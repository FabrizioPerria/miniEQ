#pragma once
#include "FFTOrder.h"
#include "utils/SingleChannelSampleFifo.h"
#include <juce_dsp/juce_dsp.h>

template <typename BlockType> class FFTDataGenerator
{
  public:
	void produceFFTDataForRendering(const juce::AudioBuffer<float> &audioData, const float negativeInfinity)
	{
		const auto fftSize = getFFTSize();

		fftCurrentData.assign(fftCurrentData.size(), 0);
		auto readPtr = audioData.getReadPointer(0);
		std::copy(readPtr, readPtr + fftSize, fftCurrentData.begin());

		window->multiplyWithWindowingTable(fftCurrentData.data(), fftSize);
		forwardFFT->performFrequencyOnlyForwardTransform(fftCurrentData.data());

		int numBins = (int)fftSize / 2;

		std::for_each_n(fftCurrentData.begin(), numBins, [numBins](auto &x) { x /= numBins; });

		std::for_each_n(fftCurrentData.begin(), numBins,
						[negativeInfinity](auto &x) { x = juce::Decibels::gainToDecibels(x, negativeInfinity); });

		fftDatafifo.push(fftCurrentData);
	}

	void changeOrder(FFTOrder newOrder)
	{
		order = newOrder;

		auto fftSize = getFFTSize();
		forwardFFT = std::make_unique<juce::dsp::FFT>(order);
		window = std::make_unique<juce::dsp::WindowingFunction<float>>(fftSize, juce::dsp::WindowingFunction<float>::blackmanHarris);
		fftCurrentData.clear();
		fftCurrentData.resize(fftSize * 2, 0);

		fftDatafifo.prepare(fftCurrentData.size());
	}

	size_t getFFTSize() const noexcept
	{
		return 1 << order;
	}

	int getNumAvailableFFTDataBlocks() const noexcept
	{
		return fftDatafifo.getNumAvailableForReading();
	}

	bool getFFTData(BlockType &fftData)
	{
		return fftDatafifo.pull(fftData);
	}

  private:
	FFTOrder order;
	BlockType fftCurrentData;
	std::unique_ptr<juce::dsp::FFT> forwardFFT;
	std::unique_ptr<juce::dsp::WindowingFunction<float>> window;

	Fifo<BlockType> fftDatafifo;
};
