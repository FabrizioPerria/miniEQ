#include "PluginEditor.h"
#include "PluginProcessor.h"
#include "data/MonoFilter.h"
#include <JuceHeader.h>

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor(AudioPluginAudioProcessor &p)
	: AudioProcessorEditor(&p), processorRef(p), peakFreqSlider(p.getAPVTS(), "PEAK_FREQUENCY"), peakGainSlider(p.getAPVTS(), "PEAK_GAIN"),
	  peakQualitySlider(p.getAPVTS(), "PEAK_QUALITY"), lowCutFreqSlider(p.getAPVTS(), "LOWCUT_FREQUENCY"),
	  highCutFreqSlider(p.getAPVTS(), "HIGHCUT_FREQUENCY"), lowCutSlopeSlider(p.getAPVTS(), "LOWCUT_SLOPE"),
	  highCutSlopeSlider(p.getAPVTS(), "HIGHCUT_SLOPE")
{
	addAndMakeVisible(peakFreqSlider);
	addAndMakeVisible(peakGainSlider);
	addAndMakeVisible(peakQualitySlider);
	addAndMakeVisible(lowCutFreqSlider);
	addAndMakeVisible(highCutFreqSlider);
	addAndMakeVisible(lowCutSlopeSlider);
	addAndMakeVisible(highCutSlopeSlider);

	for (auto param : processorRef.getParameters())
	{
		param->addListener(this);
	}

	startTimerHz(60);
	setSize(600, 400);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
	for (auto param : processorRef.getParameters())
	{
		param->removeListener(this);
	}
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint(juce::Graphics &g)
{
	// (Our component is opaque, so we must completely fill the background with a solid colour)
	g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

	auto drawResponseArea = getLocalBounds().removeFromTop((int)((float)getHeight() * 0.33f));
	auto h = drawResponseArea.getHeight();
	auto w = drawResponseArea.getWidth();

	g.setColour(JUCE_LIVE_CONSTANT(juce::Colours::black));
	g.fillRect(drawResponseArea);

	auto &lowCut = drawChannel.get<ChainPositions::LowCut>();
	auto &peak = drawChannel.get<ChainPositions::Peak>();
	auto &highCut = drawChannel.get<ChainPositions::HighCut>();

	auto sampleRate = processorRef.getSampleRate();

	std::vector<double> mags;
	mags.resize((size_t)w);

	for (int i = 0; i < w; ++i)
	{
		double mag = 1.f;
		auto freq = mapToLog10((double)i / (double)w, 20.0, 20000.0);
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

	Path responseCurve;
	const double outputMin = drawResponseArea.getBottom();
	const double outputMax = drawResponseArea.getY();
	auto map = [outputMin, outputMax](double input) { return jmap(input, -24.0, 24.0, outputMin, outputMax); };

	responseCurve.startNewSubPath((float)drawResponseArea.getX(), (float)map(mags.front()));

	for (int i = 1; i < w; ++i)
	{
		responseCurve.lineTo((float)(drawResponseArea.getX() + i), (float)map(mags[(size_t)i]));
	}

	g.setColour(juce::Colours::white);
	g.strokePath(responseCurve, juce::PathStrokeType(2.f));
}

void AudioPluginAudioProcessorEditor::resized()
{
	auto bounds = getLocalBounds();
	auto slidersArea = bounds.removeFromBottom(int((float)bounds.getHeight() * 0.66f));

	juce::FlexBox fbLowcut;
	fbLowcut.flexDirection = juce::FlexBox::Direction::column;
	fbLowcut.items.add(juce::FlexItem(lowCutFreqSlider).withFlex(1.0f));
	fbLowcut.items.add(juce::FlexItem(lowCutSlopeSlider).withFlex(1.0f));

	juce::FlexBox fbPeak;
	fbPeak.flexDirection = juce::FlexBox::Direction::column;
	fbPeak.items.add(juce::FlexItem(peakFreqSlider).withFlex(1.0f));
	fbPeak.items.add(juce::FlexItem(peakGainSlider).withFlex(1.0f));
	fbPeak.items.add(juce::FlexItem(peakQualitySlider).withFlex(1.0f));

	juce::FlexBox fbHighcut;
	fbHighcut.flexDirection = juce::FlexBox::Direction::column;
	fbHighcut.items.add(juce::FlexItem(highCutFreqSlider).withFlex(1.0f));
	fbHighcut.items.add(juce::FlexItem(highCutSlopeSlider).withFlex(1.0f));

	juce::FlexBox fbMain;
	fbMain.flexDirection = juce::FlexBox::Direction::row;
	fbMain.items.add(juce::FlexItem(fbLowcut).withFlex(1.0f));
	fbMain.items.add(juce::FlexItem(fbPeak).withFlex(1.0f));
	fbMain.items.add(juce::FlexItem(fbHighcut).withFlex(1.0f));

	fbMain.performLayout(slidersArea);
}

void AudioPluginAudioProcessorEditor::timerCallback()
{
	if (parametersChanged.compareAndSetBool(false, true))
	{
		updateFilters(processorRef.getAPVTS(), drawChannel, processorRef.getSampleRate());
		repaint();
	}
}

void AudioPluginAudioProcessorEditor::parameterValueChanged(int parameterIndex, float newValue)
{
	juce::ignoreUnused(parameterIndex, newValue);
	parametersChanged.set(true);
}

void AudioPluginAudioProcessorEditor::parameterGestureChanged(int parameterIndex, bool gestureIsStarting)
{
	juce::ignoreUnused(parameterIndex, gestureIsStarting);
}
