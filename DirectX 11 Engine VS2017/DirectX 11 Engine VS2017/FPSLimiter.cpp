#include "FPSLimiter.h"
#include <chrono>
#include <thread>


FPSLimiter::FPSLimiter()
{
	t.Start();
}

void FPSLimiter::Pulse(double maxfps)
{
	t.Stop();
	double currentfps = 1000.0 / t.GetMilisecondsElapsed();
	if (currentfps > maxfps)
	{
		double maxElapsedTime = 1000.0 / maxfps;
		double waitTime = maxElapsedTime - t.GetMilisecondsElapsed();
		if (waitTime > 0) //sanity check
		{
			std::this_thread::sleep_for(std::chrono::microseconds((long)(waitTime * 1000)));
		}
	}
	t.Start();
}