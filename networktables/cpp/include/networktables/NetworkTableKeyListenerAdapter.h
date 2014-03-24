/*
 * NetworkTableKeyListenerAdapter.h
 *
 *  Created on: Oct 17, 2012
 *      Author: Mitchell Wills
 */

#ifndef NETWORKTABLEKEYLISTENERADAPTER_H_
#define NETWORKTABLEKEYLISTENERADAPTER_H_


class NetworkTableKeyListenerAdapter;

#include "tables/ITableListener.h"
#include "tables/ITable.h"
#include <string>
#include "networktables/NetworkTable.h"



class NetworkTableKeyListenerAdapter : public ITableListener{
private:
	std::string relativeKey;
	std::string fullKey;
	NetworkTable* targetSource;
	ITableListener* targetListener;
public:
	NetworkTableKeyListenerAdapter(std::string relativeKey, std::string fullKey, NetworkTable* targetSource, ITableListener* targetListener);
	virtual ~NetworkTableKeyListenerAdapter();
    void ValueChanged(ITable* source, const std::string& key, EntryValue value, bool isNew);
};


#endif /* NETWORKTABLEKEYLISTENERADAPTER_H_ */
