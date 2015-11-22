#pragma once

#include <stdint.h>

extern "C"
{
	void* initializeNotifier(void (*ProcessQueue)(uint32_t, void*), void* param, int32_t *status);
	void cleanNotifier(void* notifier_pointer, int32_t *status);

	void updateNotifierAlarm(void* notifier_pointer, uint32_t triggerTime, int32_t *status);
}
