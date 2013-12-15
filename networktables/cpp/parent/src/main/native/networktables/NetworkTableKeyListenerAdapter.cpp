/*
 * NetworkTableKeyListenerAdapter.cpp
 *
 *  Created on: Oct 17, 2012
 *      Author: Mitchell Wills
 */

#include "networktables/NetworkTableKeyListenerAdapter.h"



NetworkTableKeyListenerAdapter::NetworkTableKeyListenerAdapter(std::string _relativeKey, std::string _fullKey, NetworkTable* _targetSource, ITableListener* _targetListener):
		relativeKey(_relativeKey), fullKey(_fullKey), targetSource(_targetSource), targetListener(_targetListener){}

NetworkTableKeyListenerAdapter::~NetworkTableKeyListenerAdapter(){}

void NetworkTableKeyListenerAdapter::ValueChanged(ITable* source, const std::string& key, EntryValue value, bool isNew) {
	if(key==fullKey){
		targetListener->ValueChanged(targetSource, relativeKey, value, isNew);
	}
}
