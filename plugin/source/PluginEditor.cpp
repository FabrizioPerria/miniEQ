#include "PluginEditor.h"
#include "BinaryData.h"
#include "PluginProcessor.h"
#include <JuceHeader.h>

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor(AudioPluginAudioProcessor &p)
	: AudioProcessorEditor(&p), processorRef(p), responseCurveComponent(p), peakFreqSlider(p.getAPVTS(), "PEAK_FREQUENCY", "Hz"),
	  peakGainSlider(p.getAPVTS(), "PEAK_GAIN", "dB"), peakQualitySlider(p.getAPVTS(), "PEAK_QUALITY", ""),
	  lowCutFreqSlider(p.getAPVTS(), "LOWCUT_FREQUENCY", "Hz"), highCutFreqSlider(p.getAPVTS(), "HIGHCUT_FREQUENCY", "Hz"),
	  lowCutSlopeSlider(p.getAPVTS(), "LOWCUT_SLOPE", "db/Oct"), highCutSlopeSlider(p.getAPVTS(), "HIGHCUT_SLOPE", "db/Oct"),
	  lowCutBypass(p.getAPVTS(), "LOWCUT_BYPASS", SVGData {BinaryData::power_svg, BinaryData::power_svgSize}),
	  highCutBypass(p.getAPVTS(), "HIGHCUT_BYPASS", SVGData {BinaryData::power_svg, BinaryData::power_svgSize}),
	  peakBypass(p.getAPVTS(), "PEAK_BYPASS", SVGData {BinaryData::power_svg, BinaryData::power_svgSize}),
	  analyzerEnabled(p.getAPVTS(), "ANALYZER_ENABLED", SVGData {BinaryData::power_svg, BinaryData::power_svgSize})
{
	peakFreqSlider.addLabel(0.f, "20Hz");
	peakFreqSlider.addLabel(1.f, "20kHz");
	lowCutFreqSlider.addLabel(0.f, "20Hz");
	lowCutFreqSlider.addLabel(1.f, "20kHz");
	highCutFreqSlider.addLabel(0.f, "20Hz");
	highCutFreqSlider.addLabel(1.f, "20kHz");
	peakGainSlider.addLabel(0.f, "-24dB");
	peakGainSlider.addLabel(1.f, "+24dB");
	peakQualitySlider.addLabel(0.f, "0.1");
	peakQualitySlider.addLabel(1.f, "10.0");
	lowCutSlopeSlider.addLabel(0.f, "12dB/Oct");
	lowCutSlopeSlider.addLabel(1.f, "48dB/Oct");
	highCutSlopeSlider.addLabel(0.f, "12dB/Oct");
	highCutSlopeSlider.addLabel(1.f, "48dB/Oct");

	SafePointer<AudioPluginAudioProcessorEditor> safeThis(this);
	lowCutBypass.onClick = [safeThis]() {
		if (auto components = safeThis.getComponent())
		{
			components->lowCutFreqSlider.setEnabled(!safeThis->lowCutBypass.getToggleState());
			components->lowCutSlopeSlider.setEnabled(!safeThis->lowCutBypass.getToggleState());
		}
	};

	highCutBypass.onClick = [safeThis]() {
		if (auto components = safeThis.getComponent())
		{
			components->highCutFreqSlider.setEnabled(!safeThis->highCutBypass.getToggleState());
			components->highCutSlopeSlider.setEnabled(!safeThis->highCutBypass.getToggleState());
		}
	};

	peakBypass.onClick = [safeThis]() {
		if (auto components = safeThis.getComponent())
		{
			components->peakFreqSlider.setEnabled(!safeThis->peakBypass.getToggleState());
			components->peakGainSlider.setEnabled(!safeThis->peakBypass.getToggleState());
			components->peakQualitySlider.setEnabled(!safeThis->peakBypass.getToggleState());
		}
	};
	
	analyzerEnabled.onClick = [safeThis]() {
		if (auto components = safeThis.getComponent())
		{
			components->responseCurveComponent.toggleAnalysisEnablement(safeThis->analyzerEnabled.getToggleState());
		}
	};

	addAndMakeVisible(peakFreqSlider);
	addAndMakeVisible(peakGainSlider);
	addAndMakeVisible(peakQualitySlider);
	addAndMakeVisible(lowCutFreqSlider);
	addAndMakeVisible(highCutFreqSlider);
	addAndMakeVisible(lowCutSlopeSlider);
	addAndMakeVisible(highCutSlopeSlider);
	addAndMakeVisible(responseCurveComponent);

	addAndMakeVisible(analyzerEnabled);
	addAndMakeVisible(lowCutBypass);
	addAndMakeVisible(peakBypass);
	addAndMakeVisible(highCutBypass);

	setSize(600, 480);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint(juce::Graphics &g)
{
	// (Our component is opaque, so we must completely fill the background with a solid colour)
	g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void AudioPluginAudioProcessorEditor::resized()
{
	juce::FlexBox fbAnalyzer;
	fbAnalyzer.flexDirection = juce::FlexBox::Direction::column;
	fbAnalyzer.items.add(juce::FlexItem(analyzerEnabled).withHeight(25));
	fbAnalyzer.items.add(juce::FlexItem(responseCurveComponent).withFlex(1.0f));

	juce::FlexBox fbLowcut;
	fbLowcut.flexDirection = juce::FlexBox::Direction::column;
	fbLowcut.items.add(juce::FlexItem(lowCutBypass).withHeight(25));
	fbLowcut.items.add(juce::FlexItem(lowCutFreqSlider).withFlex(1.0f));
	fbLowcut.items.add(juce::FlexItem(lowCutSlopeSlider).withFlex(1.0f));

	juce::FlexBox fbPeak;
	fbPeak.flexDirection = juce::FlexBox::Direction::column;
	fbPeak.items.add(juce::FlexItem(peakBypass).withHeight(25));
	fbPeak.items.add(juce::FlexItem(peakFreqSlider).withFlex(1.0f));
	fbPeak.items.add(juce::FlexItem(peakGainSlider).withFlex(1.0f));
	fbPeak.items.add(juce::FlexItem(peakQualitySlider).withFlex(1.0f));

	juce::FlexBox fbHighcut;
	fbHighcut.flexDirection = juce::FlexBox::Direction::column;
	fbHighcut.items.add(juce::FlexItem(highCutBypass).withHeight(25));
	fbHighcut.items.add(juce::FlexItem(highCutFreqSlider).withFlex(1.0f));
	fbHighcut.items.add(juce::FlexItem(highCutSlopeSlider).withFlex(1.0f));

	juce::FlexBox fbSliders;
	fbSliders.flexDirection = juce::FlexBox::Direction::row;
	fbSliders.items.add(juce::FlexItem(fbLowcut).withFlex(1.0f));
	fbSliders.items.add(juce::FlexItem(fbPeak).withFlex(1.0f));
	fbSliders.items.add(juce::FlexItem(fbHighcut).withFlex(1.0f));

	juce::FlexBox fbMain;
	fbMain.flexDirection = juce::FlexBox::Direction::column;
	fbMain.items.add(juce::FlexItem(fbAnalyzer).withFlex(1.f));
	fbMain.items.add(juce::FlexItem(fbSliders).withFlex(2.f));

	fbMain.performLayout(getLocalBounds().toFloat());
}
