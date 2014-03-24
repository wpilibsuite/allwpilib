/*
 * NetworkTableSubListenerAdapter.cpp
 *
 *  Created on: Oct 17, 2012
 *      Author: Mitchell Wills
 */

#include "networktables/NetworkTableSubListenerAdapter.h"



	/**
	 * Create a new adapter
	 * @param prefix the prefix of the current table
	 * @param targetSource the source that events passed to the target listener will appear to come from
	 * @param targetListener the listener where events are forwarded to
	 */
NetworkTableSubListenerAdapter::NetworkTableSubListenerAdapter(std::string& _prefix, NetworkTable* _targetSource, ITableListener* _targetListener):
	prefix(_prefix), targetSource(_targetSource), targetListener(_targetListener){}

NetworkTableSubListenerAdapter::~NetworkTableSubListenerAdapter(){}

	void NetworkTableSubListenerAdapter::ValueChanged(ITable* source, const std::string& key, EntryValue value, bool isNew) {//TODO use string cache
            if(key.compare(0,prefix.size(),prefix)==0){
                std::string relativeKey = key.substr(prefix.length()+1);
                int endSubTable = -1;//TODO implement sub table listening better
                for(unsigned int i = 0; i<relativeKey.length(); ++i){
                    if(relativeKey.at(i)==NetworkTable::PATH_SEPARATOR_CHAR){//is sub table
                        endSubTable = i;
                        break;
                    }
                }
                if(endSubTable!=-1){
                    std::string subTableKey = relativeKey.substr(0, endSubTable);
                    if(notifiedTables.find(subTableKey)==notifiedTables.end()){
                        notifiedTables.insert(subTableKey);
                        EntryValue eValue;
                        eValue.ptr = targetSource->GetSubTable(subTableKey);
                        targetListener->ValueChanged(targetSource, subTableKey, eValue, true);
                    }
                }
            }
	}
