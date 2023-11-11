#include "ui/ResponseCurveComponent.h"
#include "PluginProcessor.h"

ResponseCurveComponent::ResponseCurveComponent(AudioPluginAudioProcessor &processorRef) : p(processorRef)
{
	for (auto param : processorRef.getParameters())
	{
		param->addListener(this);
	}

	startTimerHz(60);
}

ResponseCurveComponent::~ResponseCurveComponent()
{
	stopTimer();

	for (auto param : p.getParameters())
	{
		param->removeListener(this);
	}
}

void ResponseCurveComponent::paint(juce::Graphics &g)
{
	auto drawResponseArea = getLocalBounds(); //.removeFromTop((int)((float)getHeight() * 0.33f));
	auto h = drawResponseArea.getHeight();
	auto w = drawResponseArea.getWidth();

	g.setColour(juce::Colours::black);
	g.fillRect(drawResponseArea);

	auto &lowCut = drawChannel.get<ChainPositions::LowCut>();
	auto &peak = drawChannel.get<ChainPositions::Peak>();
	auto &highCut = drawChannel.get<ChainPositions::HighCut>();

	auto sampleRate = p.getSampleRate();

	std::vector<double> mags;
	mags.resize((size_t)w);

	for (int i = 0; i < w; ++i)
	{
		double mag = 1.f;
		auto freq = juce::mapToLog10((double)i / (double)w, 20.0, 20000.0);
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

		mags[(size_t)i] = juce::Decibels::gainToDecibels(mag);
	}

	g.setColour(juce::Colours::orange);
	g.drawRoundedRectangle(drawResponseArea.toFloat(), 4.f, 1.f);

	g.setColour(juce::Colours::cyan);
	g.setOpacity(0.5f);
	g.setFont(10.0f);

	std::vector<float> freqs {20.0f, 50.0f, 100.0f, 200.0f, 500.0f, 1000.0f, 2000.0f, 5000.0f, 10000.0f};
	for (auto freq : freqs)
	{
		float x = (float)w * juce::mapFromLog10(freq, 20.0f, 20000.0f);
		g.drawLine(x, 0, x, (float)h);
		g.drawText(juce::String(freq) + " Hz", (int)x, 0, 50, 20, juce::Justification::centred, false);
	}

	for (int i = 1; i < 4; ++i)
	{
		float y = (float)(i * h) / 4.0f;
		g.drawLine(0, y, (float)w, y);
		g.drawText(juce::String((i - 2) * 12) + " dB", 0, (int)y, 50, 20, juce::Justification::centred, false);
	}

	juce::Path responseCurve;
	const double outputMin = drawResponseArea.getBottom();
	const double outputMax = drawResponseArea.getY();
	auto map = [outputMin, outputMax](double input) { return juce::jmap(input, -24.0, 24.0, outputMin, outputMax); };

	responseCurve.startNewSubPath((float)drawResponseArea.getX(), (float)map(mags.front()));

	for (int i = 1; i < w; ++i)
	{
		responseCurve.lineTo((float)(drawResponseArea.getX() + i), (float)map(mags[(size_t)i]));
	}

	g.setColour(juce::Colours::white);
	g.strokePath(responseCurve, juce::PathStrokeType(2.f));
}

void ResponseCurveComponent::resized()
{
}

void ResponseCurveComponent::timerCallback()
{
	if (parametersChanged.compareAndSetBool(false, true))
	{
		updateFilters(p.getAPVTS(), drawChannel, p.getSampleRate());
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
