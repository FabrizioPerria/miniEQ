#pragma once

#include "utils/Channel.h"
#include <array>
#include <juce_core/juce_core.h>
#include <juce_dsp/juce_dsp.h>
#include <unistd.h>

template <typename T> class Fifo
{
  public:
	void prepare(int numChannels, int numSamples)
	{
		static_assert(std::is_same_v<T, juce::AudioBuffer<float>>,
					  "prepare(numChannels, numSamples) should only be used when the Fifo is holding juce::AudioBuffer<float>");
		for (auto &buffer : buffers)
		{
			buffer.setSize(numChannels,
						   numSamples,
						   false, // clear everything?
						   true,  // including the extra space?
						   true); // avoid reallocating if you can?
			buffer.clear();
		}
	}

	void prepare(size_t numElements)
	{
		static_assert(std::is_same_v<T, std::vector<float>>,
					  "prepare(numElements) should only be used when the Fifo is holding std::vector<float>");
		for (auto &buffer : buffers)
		{
			buffer.clear();
			buffer.resize(numElements, 0);
		}
	}

	bool push(const T &data)
	{
		auto write = fifo.write(1);
		if (write.blockSize1 > 0)
		{
			buffers[(size_t)write.startIndex1] = data;
			return true;
		}
		return false;
	}

	bool pull(T &data)
	{
		auto read = fifo.read(1);
		if (read.blockSize1 > 0)
		{
			data = buffers[(size_t)read.startIndex1];
			return true;
		}
		return false;
	}

	int getNumAvailableForReading() const
	{
		return fifo.getNumReady();
	}

  private:
	juce::AbstractFifo fifo {Capacity};
	static constexpr int Capacity = 30;
	std::array<T, Capacity> buffers;
};

template <typename BlockType> class SingleChannelSampleFifo
{
  public:
	SingleChannelSampleFifo(Channel currentChannel) : channel(currentChannel)
	{
	}

	void prepare(int bufferSize)
	{
		prepared.set(false);
		size.set(bufferSize);
		bufferToFill.setSize(1, bufferSize, false, true, true);
		audioBufferFifo.prepare(1, bufferSize);
		fifoIndex = 0;
		prepared.set(true);
	}

	void update(const BlockType data)
	{
		jassert(isPrepared());
		jassert(data.getNumChannels() > channel);
		auto channelPtr = data.getReadPointer(channel);
		for (auto i = 0; i < data.getNumSamples(); ++i)
		{
			pushToBuffer(channelPtr[i]);
		}
	}

	int getNumCompleteBuffersAvailable() const
	{
		return audioBufferFifo.getNumAvailableForReading();
	}
	bool isPrepared() const
	{
		return prepared.get();
	}
	int getSize() const
	{
		return size.get();
	}

	bool getAudioBuffer(BlockType &buf)
	{
		return audioBufferFifo.pull(buf);
	}

  private:
	void pushToBuffer(const float sample)
	{
		if (fifoIndex == bufferToFill.getNumSamples())
		{
			audioBufferFifo.push(bufferToFill);
			fifoIndex = 0;
		}
		bufferToFill.setSample(0, fifoIndex++, sample);
	}

	Channel channel;
	juce::Atomic<bool> prepared {false};
	juce::Atomic<int> size;

	int fifoIndex;
	Fifo<BlockType> audioBufferFifo;
	BlockType bufferToFill;
};
