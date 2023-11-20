#include "ui/LookAndFeel/SliderLookAndFeel.h"
#include "ui/SliderWithLabelComponent.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>

void SliderLookAndFeel::drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPosProportional,
										 float rotaryStartAngle, float rotaryEndAngle, juce::Slider &slider)
{
	jassert(rotaryStartAngle < rotaryEndAngle);

	auto sliderBounds = juce::Rectangle<int>(x, y, width, height);
	g.setColour(juce::Colours::orange);
	g.drawEllipse(sliderBounds.toFloat(), 1.0f);
	g.setColour(juce::Colours::purple);
	g.fillEllipse(sliderBounds.toFloat());

	if (auto customSlider = dynamic_cast<SliderWithLabelsComponent *>(&slider))
	{
		auto center = sliderBounds.getCentre();
		juce::Path path;
		juce::Rectangle<float> pointer;
		const int margin = 2;
		pointer.setLeft((float)(center.getX() - margin));
		pointer.setRight((float)(center.getX() + margin));
		pointer.setTop((float)sliderBounds.getY());
		pointer.setBottom((float)center.getY() - (float)customSlider->getTextHeight() * 1.5f);
		path.addRoundedRectangle(pointer, 2.0f);

		auto rotateAngle = juce::jmap(sliderPosProportional, 0.0f, 1.0f, rotaryStartAngle, rotaryEndAngle);
		path.applyTransform(juce::AffineTransform().rotated(rotateAngle, (float)center.getX(), (float)center.getY()));
		g.setColour(juce::Colours::green);
		g.fillPath(path);

		g.setFont((float)customSlider->getTextHeight());
		auto textWidth = g.getCurrentFont().getStringWidth(customSlider->getDisplayText());
		juce::Rectangle<int> textBounds;
		textBounds.setSize(textWidth + 4, (int)customSlider->getTextHeight() + 2);
		textBounds.setCentre(center.getX(), center.getY());
		g.setColour(juce::Colours::black);
		g.fillRect(textBounds);
		g.setColour(juce::Colours::white);
		g.drawFittedText(customSlider->getDisplayText(), textBounds, juce::Justification::centredBottom, 1);
	}
}
