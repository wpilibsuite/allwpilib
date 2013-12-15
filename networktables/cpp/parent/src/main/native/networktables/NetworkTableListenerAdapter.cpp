/*
 * NetworkTableListenerAdapter.cpp
 *
 *  Created on: Oct 17, 2012
 *      Author: Mitchell Wills
 */

#include "networktables/NetworkTableListenerAdapter.h"
#include "networktables/NetworkTable.h"



NetworkTableListenerAdapter::NetworkTableListenerAdapter(std::string _prefix, ITable* _targetSource, ITableListener* _targetListener) :
	prefix(_prefix), targetSource(_targetSource), targetListener(_targetListener){}

NetworkTableListenerAdapter::~NetworkTableListenerAdapter(){}

void NetworkTableListenerAdapter::ValueChanged(ITable* source, const std::string& key, EntryValue value, bool isNew) {//TODO use string cache
	if(key.compare(0,prefix.size(),prefix)==0){
		std::string relativeKey = key.substr(prefix.length());
		if(std::string::npos != relativeKey.find(NetworkTable::PATH_SEPARATOR_CHAR))
			return;
		targetListener->ValueChanged(targetSource, relativeKey, value, isNew);
	}
}

