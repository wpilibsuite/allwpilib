#ifndef _LIVE_WINDOW_STATUS_LISTENER_H
#define _LIVE_WINDOW_STATUS_LISTENER_H

#include "tables/ITable.h"
#include "tables/ITableListener.h"

class LiveWindowStatusListener : public ITableListener {
public:
	virtual void ValueChanged(ITable* source, const std::string& key, EntryValue value, bool isNew);
};

#endif 
