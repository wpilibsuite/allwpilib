/*
 * NetworkTableNode.h
 *
 *  Created on: Sep 24, 2012
 *      Author: Mitchell Wills
 */

#ifndef NETWORKTABLENODE_H_
#define NETWORKTABLENODE_H_


class NetworkTableNode;

#include "networktables2/AbstractNetworkTableEntryStore.h"
#include "networktables2/client/ClientConnectionListenerManager.h"
#include "networktables2/type/NetworkTableEntryType.h"
#include "networktables2/type/ComplexData.h"
#include "networktables2/type/ComplexEntryType.h"
#include "tables/IRemote.h"
#include <string>
#include <vector>


/**
 * represents a node (either a client or a server) in a network tables 2.0
 * <br>
 * implementers of the class must ensure that they call {@link #init(NetworkTableTransactionPool, AbstractNetworkTableEntryStore)} before calling any other methods on this class
 * 
 * @author Mitchell
 *
 */
class NetworkTableNode : public TableListenerManager, public ClientConnectionListenerManager, public IRemote{

protected:
	AbstractNetworkTableEntryStore& entryStore;
	NetworkTableNode(AbstractNetworkTableEntryStore& entryStore);
	
public:
	/**
	 * @return the entry store used by this node
	 */
	AbstractNetworkTableEntryStore& GetEntryStore();
	virtual ~NetworkTableNode();

	
	
	void PutBoolean(std::string& name, bool value);
	bool GetBoolean(std::string& name);

	void PutDouble(std::string& name, double value);
	double GetDouble(std::string& name);

	void PutString(std::string& name, std::string& value);
	std::string& GetString(std::string& name);
        
    void PutComplex(std::string& name, ComplexData& value);
        
	void retrieveValue(std::string& name, ComplexData& externalData);
	
	/**
	 * Put a value with a specific network table type
	 * @param name the name of the entry to associate with the given value
	 * @param type the type of the entry
	 * @param value the actual value of the entry
	 */
	void PutValue(std::string& name, NetworkTableEntryType* type, EntryValue value);
	void PutValue(NetworkTableEntry* entry, EntryValue value);
	
	EntryValue GetValue(std::string& name);
	
	
	/**
	 * @param key the key to check for existence
	 * @return true if the table has the given key
	 */
	bool ContainsKey(std::string& key);

	/**
	 * close all networking activity related to this node
	 */
	virtual void Close() = 0;
	
private:
	std::vector<IRemoteConnectionListener*> remoteListeners;
public:
	void AddConnectionListener(IRemoteConnectionListener* listener, bool immediateNotify);
	void RemoveConnectionListener(IRemoteConnectionListener* listener);
	void FireConnectedEvent();
	void FireDisconnectedEvent();
	

private:
	std::vector<ITableListener*> tableListeners;
public:
	void AddTableListener(ITableListener* listener, bool immediateNotify);
	void RemoveTableListener(ITableListener* listener);
	void FireTableListeners(std::string& key, EntryValue value, bool isNew);
};




#endif /* NETWORKTABLENODE_H_ */
