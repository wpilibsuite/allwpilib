
#include "HAL/Notifier.h"

#include "ChipObject.h"

static const uint32_t kTimerInterruptNumber = 28;

struct notifier_t {
  tAlarm *alarm;
  tInterruptManager *manager;
};
typedef struct notifier_t Notifier;

void* initializeNotifier(void (*ProcessQueue)(uint32_t, void*), int32_t *status) {
  Notifier* notifier = new Notifier();
  notifier->manager = new tInterruptManager(1 << kTimerInterruptNumber, false, status);
  notifier->manager->registerHandler(ProcessQueue, NULL, status);
  notifier->manager->enable(status);
  notifier->alarm = tAlarm::create(status);
  return notifier;
}

void cleanNotifier(void* notifier_pointer, int32_t *status) {
  Notifier* notifier = (Notifier*) notifier_pointer;
  notifier->alarm->writeEnable(false, status);
  delete notifier->alarm;
  notifier->alarm = NULL;
  notifier->manager->disable(status);
  delete notifier->manager;
  notifier->manager = NULL;
}


void updateNotifierAlarm(void* notifier_pointer, uint32_t triggerTime, int32_t *status) {
  Notifier* notifier = (Notifier*) notifier_pointer;
  // write the first item in the queue into the trigger time
  notifier->alarm->writeTriggerTime(triggerTime, status);
  // Enable the alarm.  The hardware disables itself after each alarm.
  notifier->alarm->writeEnable(true, status);
}
