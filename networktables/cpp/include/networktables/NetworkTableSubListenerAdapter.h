/*
 * NetworkTableSubListenerAdapter.h
 *
 *  Created on: Oct 17, 2012
 *      Author: Mitchell Wills
 */

#ifndef NETWORKTABLESUBLISTENERADAPTER_H_
#define NETWORKTABLESUBLISTENERADAPTER_H_


class NetworkTableSubListenerAdapter;

#include "tables/ITableListener.h"
#include "tables/ITable.h"
#include <string>
#include <set>
#include "networktables/NetworkTable.h"


class NetworkTableSubListenerAdapter : public ITableListener{
private:
	std::string& prefix;
	NetworkTable* targetSource;
	ITableListener* targetListener;
	std::set<std::string> notifiedTables;

public:
	NetworkTableSubListenerAdapter(std::string& prefix, NetworkTable* targetSource, ITableListener* targetListener);
	virtual ~NetworkTableSubListenerAdapter();
	void ValueChanged(ITable* source, const std::string& key, EntryValue value, bool isNew);
};



#endif /* NETWORKTABLESUBLISTENERADAPTER_H_ */
