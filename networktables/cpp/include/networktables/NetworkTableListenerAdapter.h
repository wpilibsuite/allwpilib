/*
 * NetworkTableListenerAdapter.h
 *
 *  Created on: Oct 17, 2012
 *      Author: Mitchell Wills
 */

#ifndef NETWORKTABLELISTENERADAPTER_H_
#define NETWORKTABLELISTENERADAPTER_H_

class NetworkTableListenerAdapter;

#include "tables/ITableListener.h"
#include "tables/ITable.h"
#include <string>


class NetworkTableListenerAdapter : public ITableListener{
private:
	std::string prefix;
	ITable* targetSource;
	ITableListener* targetListener;
public:
	NetworkTableListenerAdapter(std::string prefix, ITable* targetSource, ITableListener* targetListener);
	virtual ~NetworkTableListenerAdapter();
    void ValueChanged(ITable* source, const std::string& key, EntryValue value, bool isNew);
};


#endif /* NETWORKTABLELISTENERADAPTER_H_ */
