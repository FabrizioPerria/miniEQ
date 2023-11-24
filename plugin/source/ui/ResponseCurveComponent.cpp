#include "ui/ResponseCurveComponent.h"
#include "PluginProcessor.h"
#include <juce_gui_extra/juce_gui_extra.h>

ResponseCurveComponent::ResponseCurveComponent(AudioPluginAudioProcessor &processorRef)
	: pluginProcessor(processorRef), leftChannelFifo(&processorRef.leftChannelFifo) //, rightChannelFifo(&processorRef.rightChannelFifo)
{
	for (auto param : processorRef.getParameters())
	{
		param->addListener(this);
	}

	leftDataGenerator.changeOrder(FFTOrder::order2048);
	monoBuffer.setSize(1, (int)leftDataGenerator.getFFTSize());

	updateFilters(pluginProcessor.getAPVTS(), drawChannel, pluginProcessor.getSampleRate());
	startTimerHz(60);
}

ResponseCurveComponent::~ResponseCurveComponent()
{
	stopTimer();

	for (auto param : pluginProcessor.getParameters())
	{
		param->removeListener(this);
	}
}

void ResponseCurveComponent::paint(juce::Graphics &g)
{
	g.drawImage(background, getCanvasArea());

	auto drawResponseArea = getAnalysisArea();

	auto left = drawResponseArea.getX();

	leftFFTCurve.applyTransform(juce::AffineTransform().translation(left, 0));
	g.setColour(juce::Colours::skyblue);
	g.strokePath(leftFFTCurve, juce::PathStrokeType(1.f));

	auto w = drawResponseArea.toNearestInt().getWidth();

	auto &lowCut = drawChannel.get<ChainPositions::LowCut>();
	auto &peak = drawChannel.get<ChainPositions::Peak>();
	auto &highCut = drawChannel.get<ChainPositions::HighCut>();

	auto sampleRate = pluginProcessor.getSampleRate();

	std::vector<float> mags;
	mags.resize((size_t)w);

	for (int i = 0; i < w; ++i)
	{
		double mag = 1.f;
		auto freq = juce::mapToLog10((float)i / (float)w, 20.0f, 20000.0f);
		if (!drawChannel.isBypassed<ChainPositions::Peak>())
			mag *= peak.coefficients->getMagnitudeForFrequency(freq, sampleRate);

		if (!lowCut.isBypassed<0>())
			mag *= lowCut.get<0>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
		if (!lowCut.isBypassed<1>())
			mag *= lowCut.get<1>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
		if (!lowCut.isBypassed<2>())
			mag *= lowCut.get<2>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
		if (!lowCut.isBypassed<3>())
			mag *= lowCut.get<3>().coefficients->getMagnitudeForFrequency(freq, sampleRate);

		if (!highCut.isBypassed<0>())
			mag *= highCut.get<0>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
		if (!highCut.isBypassed<1>())
			mag *= highCut.get<1>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
		if (!highCut.isBypassed<2>())
			mag *= highCut.get<2>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
		if (!highCut.isBypassed<3>())
			mag *= highCut.get<3>().coefficients->getMagnitudeForFrequency(freq, sampleRate);

		mags[(size_t)i] = juce::Decibels::gainToDecibels((float)mag);
	}

	juce::Path responseCurve;
	const float outputMin = drawResponseArea.getBottom();
	const float outputMax = drawResponseArea.getY();
	auto map = [outputMin, outputMax](float input) { return juce::jmap(input, -24.0f, 24.0f, outputMin, outputMax); };

	responseCurve.startNewSubPath(drawResponseArea.getX(), map(mags.front()));

	for (int i = 1; i < w; ++i)
	{
		responseCurve.lineTo(drawResponseArea.getX() + (float)i, map(mags[(size_t)i]));
	}

	g.setColour(juce::Colours::white);
	g.strokePath(responseCurve, juce::PathStrokeType(2.f));
	g.setOpacity(1.0f);
	g.setColour(juce::Colours::orange);
	g.drawRoundedRectangle(getRenderArea(), 4.f, 2.f);
}

void ResponseCurveComponent::resized()
{
	auto canvas = getCanvasArea().toNearestInt();
	background = juce::Image(juce::Image::PixelFormat::RGB, canvas.getWidth(), canvas.getHeight(), true);
	auto g = juce::Graphics(background);

	g.setColour(juce::Colours::black);
	g.fillRect(canvas);

	auto drawResponseArea = getAnalysisArea();
	auto w = drawResponseArea.getWidth();
	auto left = drawResponseArea.getX();
	auto top = drawResponseArea.getY();
	auto bottom = drawResponseArea.getBottom();
	auto right = drawResponseArea.getRight();

	g.setColour(juce::Colours::cyan);
	g.setOpacity(0.5f);
	auto fontHeight = 10.0f;
	g.setFont(fontHeight);
	auto font = g.getCurrentFont();

	std::vector<float> freqs {20.0f,	/*30.0f,   40.0f,*/ 50.0f,
							  100.0f,	200.0f, /*300.0f,	 400.0f,*/
							  500.0f,	1000.0f,
							  2000.0f,	/*3000.0f, 4000.0f,*/ 5000.0f,
							  10000.0f, 20000.0f};
	for (auto freq : freqs)
	{
		float x = w * juce::mapFromLog10(freq, 20.0f, 20000.0f);
		g.drawVerticalLine(int(left + x), top, bottom);
		auto addK = freq > 999.9f ? "k" : "";
		auto freqToPrint = juce::String(freq > 999.9f ? (freq / 1000.0f) : freq);
		auto text = juce::String(freqToPrint) + addK + "Hz";
		juce::Rectangle<float> r;
		r.setSize((float)font.getStringWidth(text), fontHeight);
		r.setCentre(left + x, 0);
		r.setTop(3);
		g.drawFittedText(text, r.toNearestInt(), juce::Justification::centred, 1, 1.0f);
	}

	std::vector<float> gains {-24.0f, -12.0f, 0.0f, 12.0f, 24.0f};
	for (auto gain : gains)
	{
		float y = juce::jmap(gain, -24.0f, 24.0f, bottom, top);
		g.setColour(gain == 0.0f ? juce::Colours::lime : juce::Colours::cyan);
		g.setOpacity(gain == 0.0f ? 1 : 0.5);
		g.drawHorizontalLine((int)y, left, right);

		juce::Rectangle<float> responseGains;
		auto responseText = (gain > 0 ? "+" : "") + juce::String(gain);
		auto responseTextSize = (float)font.getStringWidth(responseText);
		responseGains.setSize(responseTextSize, fontHeight);
		responseGains.setCentre(0, y);
		responseGains.setX(right + 2);
		g.drawFittedText(responseText, responseGains.toNearestInt(), juce::Justification::left, 1, 1.0f);

		juce::Rectangle<float> spectrumGains;
		auto spectrumText = juce::String(gain - 24);
		auto spectrumTextSize = (float)font.getStringWidth(spectrumText);
		spectrumGains.setSize(spectrumTextSize, fontHeight);
		spectrumGains.setCentre(0, y);
		spectrumGains.setX(left - spectrumTextSize - 2);
		g.drawFittedText(spectrumText, spectrumGains.toNearestInt(), juce::Justification::right, 1, 1.0f);
	}
}

juce::Rectangle<float> ResponseCurveComponent::getCanvasArea()
{
	juce::Rectangle<float> bounds = getLocalBounds().toFloat();
	return bounds;
}

juce::Rectangle<float> ResponseCurveComponent::getRenderArea()
{
	auto bounds = getCanvasArea();
	bounds.removeFromTop(12);
	bounds.removeFromBottom(2);
	bounds.removeFromRight(20);
	bounds.removeFromLeft(20);

	return bounds;
}

juce::Rectangle<float> ResponseCurveComponent::getAnalysisArea()
{
	auto bounds = getRenderArea();
	bounds.removeFromTop(4);
	bounds.removeFromBottom(4);

	return bounds;
}

void ResponseCurveComponent::timerCallback()
{
	juce::AudioBuffer<float> tempIncomingBuffer;

	while (leftChannelFifo->getNumCompleteBuffersAvailable() > 0)
	{
		if (leftChannelFifo->getAudioBuffer(tempIncomingBuffer))
		{
			auto size = tempIncomingBuffer.getNumSamples();
			juce::FloatVectorOperations::copy(monoBuffer.getWritePointer(0, 0), monoBuffer.getReadPointer(0, size),
											  monoBuffer.getNumSamples() - size);
			juce::FloatVectorOperations::copy(monoBuffer.getWritePointer(0, monoBuffer.getNumSamples() - size),
											  tempIncomingBuffer.getReadPointer(0, 0), size);

			leftDataGenerator.produceFFTDataForRendering(monoBuffer, -48.0f);
		}

		const auto bounds = getAnalysisArea();
		const auto fftSize = (int)leftDataGenerator.getFFTSize();
		const auto binWidth = (float)pluginProcessor.getSampleRate() / (float)fftSize;

		while (leftDataGenerator.getNumAvailableFFTDataBlocks() > 0)
		{
			std::vector<float> fftData;
			if (leftDataGenerator.getFFTData(fftData))
			{
				fftPathGenerator.generatePath(fftData, bounds, fftSize, binWidth, -48.0f);
			}
		}

		while (fftPathGenerator.getNumPathsAvailable() > 0)
		{
			fftPathGenerator.getPath(leftFFTCurve);
		}

		if (parametersChanged.compareAndSetBool(false, true))
		{
			updateFilters(pluginProcessor.getAPVTS(), drawChannel, pluginProcessor.getSampleRate());
		}
		repaint();
	}
}

void ResponseCurveComponent::parameterValueChanged(int parameterIndex, float newValue)
{
	juce::ignoreUnused(parameterIndex, newValue);
	parametersChanged.set(true);
}

void ResponseCurveComponent::parameterGestureChanged(int parameterIndex, bool gestureIsStarting)
{
	juce::ignoreUnused(parameterIndex, gestureIsStarting);
}
