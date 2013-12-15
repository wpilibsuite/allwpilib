/*
 * FlushableOutgoingEntryReceiver.h
 *
 *  Created on: Sep 22, 2012
 *      Author: Mitchell Wills
 */

#ifndef FLUSHABLEOUTGOINGENTRYRECEIVER_H_
#define FLUSHABLEOUTGOINGENTRYRECEIVER_H_

class FlushableOutgoingEntryReceiver;

#include "networktables2/OutgoingEntryReceiver.h"

class FlushableOutgoingEntryReceiver : public OutgoingEntryReceiver
{
public:
	virtual ~FlushableOutgoingEntryReceiver()
	{
	}
	virtual void flush() = 0;
	virtual void ensureAlive() = 0;
};

#endif /* FLUSHABLEOUTGOINGENTRYRECEIVER_H_ */
