#pragma once

#ifdef __vxworks
#include <vxWorks.h>
#else
#include <stdint.h>
#endif

extern "C"
{
	void* initializeNotifier(void (*ProcessQueue)(uint32_t, void*), int32_t *status);
	void cleanNotifier(void* notifier_pointer, int32_t *status);

	void updateNotifierAlarm(void* notifier_pointer, uint32_t triggerTime, int32_t *status);
}
