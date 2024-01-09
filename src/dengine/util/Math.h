#pragma once

class Math
{
  public:
	static inline float range(const float value, const float low1, const float high1, const float low2, const float high2)
	{
		return low2 + (value - low1) * (high2 - low2) / (high1 - low1);
	}
};
