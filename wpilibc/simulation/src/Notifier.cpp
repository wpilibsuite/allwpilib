/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Notifier.h"
#include "Timer.h"
#include "Utility.h"
#include "WPIErrors.h"

std::list<Notifier*> Notifier::timerQueue;
priority_recursive_mutex Notifier::queueMutex;
std::atomic<int> Notifier::refcount{0};
std::thread Notifier::m_task;
std::atomic<bool> Notifier::m_stopped(false);

/**
 * Create a Notifier for timer event notification.
 * @param handler The handler is called at the notification time which is set
 * using StartSingle or StartPeriodic.
 */
Notifier::Notifier(TimerEventHandler handler)
{
	if (handler == nullptr)
		wpi_setWPIErrorWithContext(NullParameter, "handler must not be nullptr");
	m_handler = handler;
	m_periodic = false;
	m_expirationTime = 0;
	m_period = 0;
	m_queued = false;
	{
		std::lock_guard<priority_recursive_mutex> sync(queueMutex);
		// do the first time intialization of static variables
		if (refcount.fetch_add(1) == 0) {
			m_task = std::thread(Run);
		}
	}
}

/**
 * Free the resources for a timer event.
 * All resources will be freed and the timer event will be removed from the
 * queue if necessary.
 */
Notifier::~Notifier()
{
	{
		std::lock_guard<priority_recursive_mutex> sync(queueMutex);
		DeleteFromQueue();

		// Delete the static variables when the last one is going away
		if (refcount.fetch_sub(1) == 1)
		{
      m_stopped = true;
			m_task.join();
		}
	}

	// Acquire the semaphore; this makes certain that the handler is
	// not being executed by the interrupt manager.
	std::lock_guard<priority_mutex> lock(m_handlerMutex);
}

/**
 * Update the alarm hardware to reflect the current first element in the queue.
 * Compute the time the next alarm should occur based on the current time and the
 * period for the first element in the timer queue.
 * WARNING: this method does not do synchronization! It must be called from somewhere
 * that is taking care of synchronizing access to the queue.
 */
void Notifier::UpdateAlarm()
{
}

/**
 * ProcessQueue is called whenever there is a timer interrupt.
 * We need to wake up and process the current top item in the timer queue as long
 * as its scheduled time is after the current time. Then the item is removed or
 * rescheduled (repetitive events) in the queue.
 */
void Notifier::ProcessQueue(uint32_t mask, void *params)
{
	Notifier *current;
	while (true)				// keep processing past events until no more
	{
		{
			std::lock_guard<priority_recursive_mutex> sync(queueMutex);
			double currentTime = GetClock();

			if (timerQueue.empty())
			{
				break;
			}
			current = timerQueue.front();
			if (current->m_expirationTime > currentTime)
			{
				break;		// no more timer events to process
			}
			// remove next entry before processing it
			timerQueue.pop_front();

			current->m_queued = false;
			if (current->m_periodic)
			{
				// if periodic, requeue the event
				// compute when to put into queue
				current->InsertInQueue(true);
			}
			else
			{
				// not periodic; removed from queue
				current->m_queued = false;
			}
			// Take handler mutex while holding queue semaphore to make sure
			//  the handler will execute to completion in case we are being deleted.
			current->m_handlerMutex.lock();
		}

		current->m_handler();	// call the event handler
		current->m_handlerMutex.unlock();
	}
	// reschedule the first item in the queue
	std::lock_guard<priority_recursive_mutex> sync(queueMutex);
	UpdateAlarm();
}

/**
 * Insert this Notifier into the timer queue in right place.
 * WARNING: this method does not do synchronization! It must be called from somewhere
 * that is taking care of synchronizing access to the queue.
 * @param reschedule If false, the scheduled alarm is based on the curent time and UpdateAlarm
 * method is called which will enable the alarm if necessary.
 * If true, update the time by adding the period (no drift) when rescheduled periodic from ProcessQueue.
 * This ensures that the public methods only update the queue after finishing inserting.
 */
void Notifier::InsertInQueue(bool reschedule)
{
	if (reschedule)
	{
		m_expirationTime += m_period;
	}
	else
	{
		m_expirationTime = GetClock() + m_period;
	}

	// Attempt to insert new entry into queue
	for (auto i = timerQueue.begin(); i != timerQueue.end(); i++)
	{
		if ((*i)->m_expirationTime > m_expirationTime)
		{
			timerQueue.insert(i, this);
			m_queued = true;
		}
	}

	/* If the new entry wasn't queued, either the queue was empty or the first
	 * element was greater than the new entry.
	 */
	if (!m_queued)
	{
		timerQueue.push_front(this);

		if (!reschedule)
		{
			/* Since the first element changed, update alarm, unless we already
			 * plan to
			 */
			UpdateAlarm();
		}

		m_queued = true;
	}
}

/**
 * Delete this Notifier from the timer queue.
 * WARNING: this method does not do synchronization! It must be called from somewhere
 * that is taking care of synchronizing access to the queue.
 * Remove this Notifier from the timer queue and adjust the next interrupt time to reflect
 * the current top of the queue.
 */
void Notifier::DeleteFromQueue()
{
	if (m_queued)
	{
		m_queued = false;
		wpi_assert(!timerQueue.empty());
		if (timerQueue.front() == this)
		{
			// remove the first item in the list - update the alarm
			timerQueue.pop_front();
			UpdateAlarm();
		}
		else
		{
			timerQueue.remove(this);
		}
	}
}

/**
 * Register for single event notification.
 * A timer event is queued for a single event after the specified delay.
 * @param delay Seconds to wait before the handler is called.
 */
void Notifier::StartSingle(double delay)
{
	std::lock_guard<priority_recursive_mutex> sync(queueMutex);
	m_periodic = false;
	m_period = delay;
	DeleteFromQueue();
	InsertInQueue(false);
}

/**
 * Register for periodic event notification.
 * A timer event is queued for periodic event notification. Each time the interrupt
 * occurs, the event will be immediately requeued for the same time interval.
 * @param period Period in seconds to call the handler starting one period after the call to this method.
 */
void Notifier::StartPeriodic(double period)
{
	std::lock_guard<priority_recursive_mutex> sync(queueMutex);
	m_periodic = true;
	m_period = period;
	DeleteFromQueue();
	InsertInQueue(false);
}

/**
 * Stop timer events from occuring.
 * Stop any repeating timer events from occuring. This will also remove any single
 * notification events from the queue.
 * If a timer-based call to the registered handler is in progress, this function will
 * block until the handler call is complete.
 */
void Notifier::Stop()
{
	{
		std::lock_guard<priority_recursive_mutex> sync(queueMutex);
		DeleteFromQueue();
	}
	// Wait for a currently executing handler to complete before returning from Stop()
	std::lock_guard<priority_mutex> sync(m_handlerMutex);
}

void Notifier::Run() {
    while (!m_stopped) {
        Notifier::ProcessQueue(0, nullptr);
        bool isEmpty;
        {
            std::lock_guard<priority_recursive_mutex> sync(queueMutex);
            isEmpty = timerQueue.empty();
        }
        if (!isEmpty)
        {
            double expirationTime;
            {
                std::lock_guard<priority_recursive_mutex> sync(queueMutex);
                expirationTime = timerQueue.front()->m_expirationTime;
            }
            Wait(expirationTime - GetClock());
        }
        else
        {
            Wait(0.05);
        }
    }
}
