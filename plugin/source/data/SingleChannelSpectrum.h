#pragma once

#include "PluginProcessor.h"
#include "utils/FFTDataGenerator.h"
#include "utils/FFTPathGenerator.h"

class SingleChannelSpectrum
{
  public:
	SingleChannelSpectrum(AudioPluginAudioProcessor::ChannelFifo *channelFifoToUse) : channelFifo(channelFifoToUse)
	{
		dataGenerator.changeOrder(FFTOrder::order2048);
		monoBuffer.setSize(1, (int)dataGenerator.getFFTSize());
	}

	void drawNextFrameOfSpectrum(juce::Graphics &g, const juce::Colour &colourToUse)
	{
		g.setColour(colourToUse);
		g.strokePath(fftCurve, juce::PathStrokeType(1.f));
	}

	void process(float sampleRate, juce::Rectangle<float> drawArea)
	{
		juce::AudioBuffer<float> tempIncomingBuffer;

		while (channelFifo->getNumCompleteBuffersAvailable() > 0)
		{
			if (channelFifo->getAudioBuffer(tempIncomingBuffer))
			{
				auto size = tempIncomingBuffer.getNumSamples();
				juce::FloatVectorOperations::copy(monoBuffer.getWritePointer(0, 0), monoBuffer.getReadPointer(0, size),
												  monoBuffer.getNumSamples() - size);
				juce::FloatVectorOperations::copy(monoBuffer.getWritePointer(0, monoBuffer.getNumSamples() - size),
												  tempIncomingBuffer.getReadPointer(0, 0), size);

				dataGenerator.produceFFTDataForRendering(monoBuffer, -48.0f);
			}
		}

		const auto fftSize = (int)dataGenerator.getFFTSize();
		const auto binWidth = sampleRate / (float)fftSize;

		while (dataGenerator.getNumAvailableFFTDataBlocks() > 0)
		{
			std::vector<float> fftData;
			if (dataGenerator.getFFTData(fftData))
			{
				fftPathGenerator.generatePath(fftData, drawArea, fftSize, binWidth, -48.0f);
			}
		}

		while (fftPathGenerator.getNumPathsAvailable() > 0)
		{
			fftPathGenerator.getPath(fftCurve);
		}
		fftCurve.applyTransform(juce::AffineTransform().translation(drawArea.getX(), 0));
	}

  private:
	AudioPluginAudioProcessor::ChannelFifo *channelFifo;

	juce::AudioBuffer<float> monoBuffer;

	FFTDataGenerator<std::vector<float>> dataGenerator;

	FFTPathGenerator<juce::Path> fftPathGenerator;

	juce::Path fftCurve;
};
