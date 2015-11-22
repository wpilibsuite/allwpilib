#include "HAL/Notifier.hpp"
#include "ChipObject.h"
#include "HAL/HAL.hpp"
#include "HAL/cpp/priority_mutex.h"
#include <atomic>
#include <cstdlib>
#include <mutex>

static const uint32_t kTimerInterruptNumber = 28;

static priority_mutex notifierInterruptMutex;
static priority_recursive_mutex notifierMutex;
static tAlarm *notifierAlarm = nullptr;
static tInterruptManager *notifierManager = nullptr;
static uint32_t closestTrigger = UINT32_MAX;
struct Notifier {
	Notifier *prev, *next;
	void *param;
	void (*process)(uint32_t, void*);
	uint32_t triggerTime = UINT32_MAX;
};
static Notifier *notifiers = nullptr;
static std::atomic_flag notifierAtexitRegistered = ATOMIC_FLAG_INIT;
static std::atomic_int notifierRefCount{0};

static void alarmCallback(uint32_t, void*)
{
	std::unique_lock<priority_recursive_mutex> sync(notifierMutex);

	int32_t status = 0;
	uint32_t currentTime = 0;

	// the hardware disables itself after each alarm
	closestTrigger = UINT32_MAX;

	// process all notifiers
	Notifier *notifier = notifiers;
	while (notifier) {
		if (notifier->triggerTime != UINT32_MAX) {
			if (currentTime == 0)
				currentTime = getFPGATime(&status);
			if (notifier->triggerTime < currentTime) {
				notifier->triggerTime = UINT32_MAX;
				auto process = notifier->process;
				auto param = notifier->param;
				sync.unlock();
				process(currentTime, param);
				sync.lock();
			} else if (notifier->triggerTime < closestTrigger) {
				updateNotifierAlarm(notifier, notifier->triggerTime, &status);
			}
		}
		notifier = notifier->next;
	}
}

static void cleanupNotifierAtExit() {
	notifierAlarm = nullptr;
	notifierManager = nullptr;
}

void* initializeNotifier(void (*ProcessQueue)(uint32_t, void*), void *param, int32_t *status)
{
	if (!ProcessQueue) {
		*status = NULL_PARAMETER;
		return nullptr;
	}
	if (!notifierAtexitRegistered.test_and_set())
		std::atexit(cleanupNotifierAtExit);
	if (notifierRefCount.fetch_add(1) == 0) {
		std::lock_guard<priority_mutex> sync(notifierInterruptMutex);
		// create manager and alarm if not already created
		if (!notifierManager) {
			notifierManager = new tInterruptManager(1 << kTimerInterruptNumber, false, status);
			notifierManager->registerHandler(alarmCallback, NULL, status);
			notifierManager->enable(status);
		}
		if (!notifierAlarm) notifierAlarm = tAlarm::create(status);
	}

	std::lock_guard<priority_recursive_mutex> sync(notifierMutex);
	// create notifier structure and add to list
	Notifier* notifier = new Notifier();
	notifier->prev = nullptr;
	notifier->next = notifiers;
	if (notifier->next) notifier->next->prev = notifier;
	notifier->param = param;
	notifier->process = ProcessQueue;
	notifiers = notifier;
	return notifier;
}

void cleanNotifier(void* notifier_pointer, int32_t *status)
{
	{
		std::lock_guard<priority_recursive_mutex> sync(notifierMutex);
		Notifier* notifier = (Notifier*)notifier_pointer;

		// remove from list and delete
		if (notifier->prev) notifier->prev->next = notifier->next;
		if (notifier->next) notifier->next->prev = notifier->prev;
		if (notifiers == notifier) notifiers = notifier->next;
		delete notifier;
	}

	if (notifierRefCount.fetch_sub(1) == 1) {
		std::lock_guard<priority_mutex> sync(notifierInterruptMutex);
		// if this was the last notifier, clean up alarm and manager
		if (notifierAlarm) {
			notifierAlarm->writeEnable(false, status);
			delete notifierAlarm;
			notifierAlarm = nullptr;
		}
		if (notifierManager) {
			notifierManager->disable(status);
			delete notifierManager;
			notifierManager = nullptr;
		}
	}
}

void updateNotifierAlarm(void* notifier_pointer, uint32_t triggerTime, int32_t *status)
{
	std::lock_guard<priority_recursive_mutex> sync(notifierMutex);

	Notifier* notifier = (Notifier*)notifier_pointer;
	notifier->triggerTime = triggerTime;
	bool wasActive = (closestTrigger != UINT32_MAX);

        if (!notifierInterruptMutex.try_lock() || notifierRefCount == 0 ||
            !notifierAlarm)
          return;

        // Update alarm time if closer than current.
	if (triggerTime < closestTrigger) {
		closestTrigger = triggerTime;
		notifierAlarm->writeTriggerTime(triggerTime, status);
	}
	// Enable the alarm.  The hardware disables itself after each alarm.
	if (!wasActive) notifierAlarm->writeEnable(true, status);

	notifierInterruptMutex.unlock();
}
