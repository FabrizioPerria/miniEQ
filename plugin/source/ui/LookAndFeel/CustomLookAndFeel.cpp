#include "ui/LookAndFeel/CustomLookAndFeel.h"
#include "ui/BypassButtonComponent.h"
#include "ui/SliderWithLabelComponent.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>

void CustomLookAndFeel::drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPosProportional,
										 float rotaryStartAngle, float rotaryEndAngle, juce::Slider &slider)
{
	if (auto customSlider = dynamic_cast<SliderWithLabelsComponent *>(&slider))
	{
		jassert(rotaryStartAngle < rotaryEndAngle);

		auto sliderBounds = juce::Rectangle<int>(x, y, width, height);
		/* g.setColour(juce::Colours::orange); */
		/* g.drawEllipse(sliderBounds.toFloat(), 1.0f); */
		/* g.setColour(juce::Colours::purple); */
		/* g.fillEllipse(sliderBounds.toFloat()); */

		auto center = sliderBounds.getCentre();
		/* juce::Path path; */
		/* juce::Rectangle<float> pointer; */
		/* const int margin = 2; */
		/* pointer.setLeft((float)(center.getX() - margin)); */
		/* pointer.setRight((float)(center.getX() + margin)); */
		/* pointer.setTop((float)sliderBounds.getY()); */
		/* pointer.setBottom((float)center.getY() - (float)customSlider->getTextHeight() * 1.8f); */
		/* path.addRoundedRectangle(pointer, 2.0f); */

		/* auto rotateAngle = juce::jmap(sliderPosProportional, 0.0f, 1.0f, rotaryStartAngle, rotaryEndAngle); */
		/* path.applyTransform(juce::AffineTransform().rotated(rotateAngle, (float)center.getX(), (float)center.getY())); */
		/* g.setColour(juce::Colours::green); */
		/*         g.fillPath(path); */

		auto knobDiameter = juce::jmin(width, height);
		auto knobBounds = juce::Rectangle<int>(x, y, knobDiameter, knobDiameter);
		auto rotateAngle = juce::jmap(sliderPosProportional, 0.0f, 1.0f, rotaryStartAngle, rotaryEndAngle);

		auto image = customSlider->knob.get();
		image->setSize(knobDiameter, knobDiameter);
		image->setTransform(
			juce::AffineTransform::rotation(rotateAngle, image->getDrawableBounds().getCentreX(), image->getDrawableBounds().getCentreY()));
		image->drawWithin(g, knobBounds.toFloat(), juce::RectanglePlacement::stretchToFit, 1.0f);

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
	else
	{
		LookAndFeel_V4::drawRotarySlider(g, x, y, width, height, sliderPosProportional, rotaryStartAngle, rotaryEndAngle, slider);
	}
}

void CustomLookAndFeel::drawToggleButton(juce::Graphics &g, juce::ToggleButton &toggleButton, bool shouldDrawButtonAsHighlighted,
										 bool shouldDrawButtonAsDown)
{
	if (auto bypassButton = dynamic_cast<BypassButtonComponent *>(&toggleButton))
	{
		auto bypassButtonBounds = juce::Rectangle<int>(0, 0, 20, 20);
		bypassButtonBounds.setCentre(toggleButton.getLocalBounds().getCentre());

		auto image = toggleButton.getToggleState() ? bypassButton->buttonImageSelected.get() : bypassButton->buttonImage.get();
		float opacity = shouldDrawButtonAsDown ? 0.5f : 1.0f;

		image->drawWithin(g, bypassButtonBounds.toFloat().reduced(3), juce::RectanglePlacement::stretchToFit, opacity);

		g.setColour(toggleButton.getToggleState() ? juce::Colours::red : juce::Colours::limegreen);
		g.setOpacity(opacity);
		g.drawEllipse(bypassButtonBounds.toFloat(), 1.0f);
	}
	else
	{
		LookAndFeel_V4::drawToggleButton(g, toggleButton, shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);
	}
}
