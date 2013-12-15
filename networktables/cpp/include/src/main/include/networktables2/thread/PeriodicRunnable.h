/*
 * PeriodicRunnable.h
 *
 *  Created on: Sep 21, 2012
 *      Author: Mitchell Wills
 */

#ifndef PERIODICRUNNABLE_H_
#define PERIODICRUNNABLE_H_

/**
 * A runnable where the run method will be called periodically 
 * 
 * @author Mitchell
 *
 */
class PeriodicRunnable
{
public:
	virtual ~PeriodicRunnable()
	{
	}
	/**
	 * the method that will be called periodically on a thread
	 * 
	 * @throws InterruptedException thrown when the thread is supposed to be interrupted and stop (implementers should always let this exception fall through)
	 */
	virtual void run() = 0;
};

#endif /* PERIODICRUNNABLE_H_ */
