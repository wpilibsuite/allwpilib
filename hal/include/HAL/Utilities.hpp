#pragma once

#include <stdint.h>

extern "C"
{
	extern const int32_t HAL_NO_WAIT;
	extern const int32_t HAL_WAIT_FOREVER;

	void delayTicks(int32_t ticks);
	void delayMillis(double ms);
	void delaySeconds(double s);
}
