#include "HAL/Utilities.hpp"
#include <time.h>

const int32_t HAL_NO_WAIT = 0;
const int32_t HAL_WAIT_FOREVER = -1;

void delayTicks(int32_t ticks)
{
	struct timespec test, remaining;
	test.tv_sec = 0;
	test.tv_nsec = ticks * 3;

	/* Sleep until the requested number of ticks has passed, with additional
		time added if nanosleep is interrupted. */
	while(nanosleep(&test, &remaining) == -1) {
		test = remaining;
	}
}

void delayMillis(double ms)
{
	struct timespec test, remaining;
	test.tv_sec = ms / 1000;
	test.tv_nsec = 1000 * (((uint64_t)ms) % 1000000);

	/* Sleep until the requested number of milliseconds has passed, with
		additional time added if nanosleep is interrupted. */
	while(nanosleep(&test, &remaining) == -1) {
		test = remaining;
	}
}

void delaySeconds(double s)
{
	struct timespec test, remaining;
	test.tv_sec = (int)s;
	test.tv_nsec = (s - (int)s) * 1000000000.0;

	/* Sleep until the requested number of seconds has passed, with additional
		time added if nanosleep is interrupted. */
	while(nanosleep(&test, &remaining) == -1) {
		test = remaining;
	}
}
