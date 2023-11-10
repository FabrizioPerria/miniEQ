#include "PluginEditor.h"
#include "PluginProcessor.h"
#include <JuceHeader.h>

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor(AudioPluginAudioProcessor& p)
	: AudioProcessorEditor(&p)
	, processorRef(p)
	, peakFreqSlider(p.getAPVTS(), "PEAK_FREQUENCY")
	, peakGainSlider(p.getAPVTS(), "PEAK_GAIN")
	, peakQualitySlider(p.getAPVTS(), "PEAK_QUALITY")
	, lowCutFreqSlider(p.getAPVTS(), "LOWCUT_FREQUENCY")
	, highCutFreqSlider(p.getAPVTS(), "HIGHCUT_FREQUENCY")
	, lowCutSlopeSlider(p.getAPVTS(), "LOWCUT_SLOPE")
	, highCutSlopeSlider(p.getAPVTS(), "HIGHCUT_SLOPE")
{
	addAndMakeVisible(peakFreqSlider);
	addAndMakeVisible(peakGainSlider);
	addAndMakeVisible(peakQualitySlider);
	addAndMakeVisible(lowCutFreqSlider);
	addAndMakeVisible(highCutFreqSlider);
	addAndMakeVisible(lowCutSlopeSlider);
	addAndMakeVisible(highCutSlopeSlider);

	setSize(600, 400);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor() { }

//==============================================================================
void AudioPluginAudioProcessorEditor::paint(juce::Graphics& g)
{
	// (Our component is opaque, so we must completely fill the background with a solid colour)
	g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

	/* g.setColour(JUCE_LIVE_CONSTANT(juce::Colours::white)); */
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
