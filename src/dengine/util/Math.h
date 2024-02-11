#pragma once

class Math
{
  public:
	static inline float range(const float value, const float low1, const float high1, const float low2, const float high2)
	{
		return low2 + (value - low1) * (high2 - low2) / (high1 - low1);
	}

	static inline float randomFloat(float min, float max)
	{
		float r = (float)rand() / (float)RAND_MAX;
		return min + r * (max - min);
	}
};
