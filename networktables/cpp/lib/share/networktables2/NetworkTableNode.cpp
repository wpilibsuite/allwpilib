/*
 * NetworkTableNode.cpp
 *
 *  Created on: Sep 24, 2012
 *      Author: Mitchell Wills
 */

#include "networktables2/NetworkTableNode.h"
#include "networktables2/TableKeyExistsWithDifferentTypeException.h"
#include "networktables2/type/DefaultEntryTypes.h"
#include "tables/TableKeyNotDefinedException.h"
#include <algorithm>



NetworkTableNode::NetworkTableNode(AbstractNetworkTableEntryStore& _entryStore):
	entryStore(_entryStore){
}

NetworkTableNode::~NetworkTableNode(){}

AbstractNetworkTableEntryStore& NetworkTableNode::GetEntryStore(){
	return entryStore;
}



void NetworkTableNode::PutBoolean(std::string& name, bool value){
	EntryValue eValue;
	eValue.b = value;
	PutValue(name, &DefaultEntryTypes::BOOLEAN, eValue);
}
bool NetworkTableNode::GetBoolean(std::string& name){
	NetworkTableEntry* entry = entryStore.GetEntry(name);
	if(entry==NULL)
		throw TableKeyNotDefinedException(name);
	return entry->GetValue().b;
}

void NetworkTableNode::PutDouble(std::string& name, double value){
	EntryValue eValue;
	eValue.f = value;
	PutValue(name, &DefaultEntryTypes::DOUBLE, eValue);
}
double NetworkTableNode::GetDouble(std::string& name){
	NetworkTableEntry* entry = entryStore.GetEntry(name);
	if(entry==NULL)
		throw TableKeyNotDefinedException(name);
	return entry->GetValue().f;
}

void NetworkTableNode::PutString(std::string& name, std::string& value){
	EntryValue eValue;
	eValue.ptr = &value;
	PutValue(name, &DefaultEntryTypes::STRING, eValue);
}
std::string& NetworkTableNode::GetString(std::string& name) {
	NetworkTableEntry* entry = entryStore.GetEntry(name);
	if(entry==NULL)
		throw TableKeyNotDefinedException(name);
	return *(std::string*)(entry->GetValue().ptr);
}
	
void NetworkTableNode::PutComplex(std::string& name, ComplexData& value){
	EntryValue eValue;
	eValue.ptr = &value;
	PutValue(name, &value.GetType(), eValue);
}
	
void NetworkTableNode::retrieveValue(std::string& name, ComplexData& externalData){
	{ 
		NTSynchronized sync(entryStore.LOCK);
		NetworkTableEntry* entry = entryStore.GetEntry(name);
		if(entry==NULL)
			throw TableKeyNotDefinedException(name);
		NetworkTableEntryType* entryType = entry->GetType();
		if(!entryType->isComplex())
			throw TableKeyExistsWithDifferentTypeException(name, entryType, "Is not a complex data type");
		ComplexEntryType* complexType = (ComplexEntryType*)entryType;
		complexType->exportValue(name, entry->GetValue(), externalData);
	}
}


void NetworkTableNode::PutValue(std::string& name, NetworkTableEntryType* type, EntryValue value){
	if(type->isComplex()){
		{ 
			NTSynchronized sync(entryStore.LOCK);
			ComplexData* complexData = (ComplexData*)value.ptr;
			ComplexEntryType* entryType = (ComplexEntryType*)type;
			NetworkTableEntry* entry = entryStore.GetEntry(name);
			if(entry!=NULL)
				entryStore.PutOutgoing(entry, entryType->internalizeValue(entry->name, *complexData, entry->GetValue()));
			else{
				EntryValue nullValue = {0};
				EntryValue entryValue = entryType->internalizeValue(name, *complexData, nullValue);
				entryStore.PutOutgoing(name, type, entryValue);//TODO the entry gets copied when creating the entry should make lifecycle cleaner
				type->deleteValue(entryValue);
			}
		}
	}
	else
		entryStore.PutOutgoing(name, type, value);
}

void NetworkTableNode::PutValue(NetworkTableEntry* entry, EntryValue value){
	if(entry->GetType()->isComplex()){
		{ 
			NTSynchronized sync(entryStore.LOCK);
			ComplexEntryType* entryType = (ComplexEntryType*)entry->GetType();
			
			entryStore.PutOutgoing(entry, entryType->internalizeValue(entry->name, *(ComplexData*)value.ptr, entry->GetValue()));
		}
	}
	else
		entryStore.PutOutgoing(entry, value);
}

EntryValue NetworkTableNode::GetValue(std::string& name){//TODO don't allow get of complex types
	{ 
		NTSynchronized sync(entryStore.LOCK);
		NetworkTableEntry* entry = entryStore.GetEntry(name);
		if(entry==NULL)
			throw TableKeyNotDefinedException(name);
		return entry->GetValue();
	}
}


bool NetworkTableNode::ContainsKey(std::string& key){
	return entryStore.GetEntry(key)!=NULL;
}

void NetworkTableNode::AddConnectionListener(IRemoteConnectionListener* listener, bool immediateNotify) {
	remoteListeners.push_back(listener);
	if(IsConnected())
		listener->Connected(this);
	else
		listener->Disconnected(this);
}
void NetworkTableNode::RemoveConnectionListener(IRemoteConnectionListener* listener) {
	std::vector<IRemoteConnectionListener*>::iterator listenerPosition = std::find(remoteListeners.begin(), remoteListeners.end(), listener);
	if(listenerPosition!=remoteListeners.end())
		remoteListeners.erase(listenerPosition);
}
void NetworkTableNode::FireConnectedEvent(){
	for(unsigned int i = 0; i<remoteListeners.size(); ++i)
		remoteListeners.at(i)->Connected(this);
}
void NetworkTableNode::FireDisconnectedEvent(){
	for(unsigned int i = 0; i<remoteListeners.size(); ++i)
		remoteListeners.at(i)->Disconnected(this);
}


void NetworkTableNode::AddTableListener(ITableListener* listener, bool immediateNotify) {
	tableListeners.push_back(listener);
	if(immediateNotify)
		entryStore.notifyEntries(NULL, listener);
}
void NetworkTableNode::RemoveTableListener(ITableListener* listener) {
	std::vector<ITableListener*>::iterator listenerPosition = std::find(tableListeners.begin(), tableListeners.end(), listener);
	if(listenerPosition!=tableListeners.end())
		tableListeners.erase(listenerPosition);
}
void NetworkTableNode::FireTableListeners(std::string& key, EntryValue value, bool isNew){
	for(unsigned int i = 0; i<tableListeners.size(); ++i)
		tableListeners.at(i)->ValueChanged(NULL, key, value, isNew);
}


