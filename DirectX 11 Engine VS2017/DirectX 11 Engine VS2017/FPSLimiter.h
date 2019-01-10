#pragma once
#include "Timer.h"
class FPSLimiter
{
public:
	FPSLimiter();
	void Pulse(double maxfps);
private:
	Timer t;
};