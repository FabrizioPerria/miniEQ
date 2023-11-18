#pragma once

#include <juce_core/juce_core.h>

class Slope
{
  public:
	enum Value
	{
		_12,
		_24,
		_36,
		_48
	};

	static juce::StringArray toArray()
	{
		return {"12", "24", "36", "48"};
	}

	static Slope fromFloat(float f)
	{
		return static_cast<Value>((int)f);
	}

	constexpr Slope(Value aType) : value(aType)
	{
	}

	constexpr operator Value() const
	{
		return value;
	}

  private:
	Value value;
};
