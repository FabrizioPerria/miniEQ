#include "ui/lookandfeel/SliderLookAndFeel.h"
#include "ui/EQSliderComponent.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>

void SliderLookAndFeel::drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPosProportional,
										 float rotaryStartAngle, float rotaryEndAngle, juce::Slider &slider)
{
	/* juce::LookAndFeel_V4::drawRotarySlider(g, x, y, width, height, sliderPosProportional, rotaryStartAngle, rotaryEndAngle, slider);
	 */
	jassert(rotaryStartAngle < rotaryEndAngle);

	auto sliderBounds = juce::Rectangle<int>(x, y, width, height);
	g.setColour(juce::Colours::orange);
	g.drawEllipse(sliderBounds.toFloat(), 1.0f);
	g.setColour(juce::Colours::purple);
	g.fillEllipse(sliderBounds.toFloat());

	if (auto customSlider = dynamic_cast<EQSliderComponent *>(&slider))
	{
		auto center = sliderBounds.getCentre();
		juce::Path path;
		juce::Rectangle<float> pointer;
		const int margin = 2;
		pointer.setLeft(center.getX() - margin);
		pointer.setRight(center.getX() + margin);
		pointer.setTop(sliderBounds.getY());
		pointer.setBottom(center.getY() - customSlider->getTextHeight() * 1.5f);
		path.addRoundedRectangle(pointer, 2.0f);

		auto rotateAngle = juce::jmap(sliderPosProportional, 0.0f, 1.0f, rotaryStartAngle, rotaryEndAngle);
		path.applyTransform(juce::AffineTransform().rotated(rotateAngle, center.getX(), center.getY()));
		g.setColour(juce::Colours::green);
		g.fillPath(path);

		g.setFont(customSlider->getTextHeight());
		auto textWidth = g.getCurrentFont().getStringWidth(customSlider->getDisplayText());
		juce::Rectangle<int> textBounds;
		textBounds.setSize(textWidth + 4, customSlider->getTextHeight() + 2);
		textBounds.setCentre(center.getX(), center.getY());
		g.setColour(juce::Colours::black);
		g.fillRect(textBounds);
		g.setColour(juce::Colours::white);
		g.drawText(customSlider->getDisplayText(), textBounds, juce::Justification::centredBottom);
	}
}
