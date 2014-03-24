#include <map>
#include "networktables/NetworkTable.h"
#include <string>
#include "networktables2/thread/NTThreadManager.h"
#include "networktables2/thread/DefaultThreadManager.h"
#include "networktables2/NetworkTableEntry.h"
#include "networktables2/util/StringCache.h"
#include "networktables/NetworkTableProvider.h"
#include "networktables/NetworkTableMode.h"
#include "OSAL/Synchronized.h"
#include "tables/TableKeyNotDefinedException.h"
#include "networktables2/type/DefaultEntryTypes.h"
#include "tables/ITableListener.h"
#include "networktables/NetworkTableConnectionListenerAdapter.h"
#include "networktables/NetworkTableKeyListenerAdapter.h"
#include "networktables/NetworkTableListenerAdapter.h"
#include "networktables/NetworkTableSubListenerAdapter.h"


const char NetworkTable::PATH_SEPARATOR_CHAR = '/';
const std::string NetworkTable::PATH_SEPARATOR("/");
const int NetworkTable::DEFAULT_PORT = 1735;

DefaultThreadManager NetworkTable::threadManager;
NetworkTableProvider* NetworkTable::staticProvider = NULL;
NetworkTableNode* NetworkTable::staticNode = NULL;
void* NetworkTable::streamFactory = NULL;
NetworkTableEntryTypeManager* NetworkTable::typeManager = NULL;
StreamDeleter streamDeleter = NULL;
NetworkTableMode* NetworkTable::mode = &NetworkTableMode::Server;
int NetworkTable::port = DEFAULT_PORT;
std::string NetworkTable::ipAddress;
NTReentrantSemaphore NetworkTable::STATIC_LOCK;






void NetworkTable::CheckInit(){
	printf("[NT] NetworkTable::CheckInit()...\n");
	{
		NTSynchronized sync(STATIC_LOCK);
		if(staticProvider!=NULL)
			throw new IllegalStateException("Network tables has already been initialized");
	}
}

void NetworkTable::Initialize() {
	CheckInit();
	printf("[NT] NetworkTable::Initialize()...\n");
	staticProvider = new NetworkTableProvider(*(staticNode = mode->CreateNode(ipAddress.c_str(), port, threadManager, streamFactory, streamDeleter, typeManager)));
	printf("[NT] ...NetworkTable::Initialize().\n");
}

void NetworkTable::Shutdown() 
{
	if (staticProvider!=NULL)
	{
		delete staticProvider;
		staticProvider=NULL;
	}
	if (staticNode!=NULL)
	{
		delete staticNode;
		staticNode=NULL;
	}
	if (streamDeleter!=NULL && streamFactory!=NULL)
	{
	        streamDeleter(streamFactory);
	        streamFactory=NULL;
		streamDeleter=NULL;
	}
	if (typeManager!=NULL)
	{
		delete typeManager;
		typeManager=NULL;
	}
}

void NetworkTable::SetTableProvider(NetworkTableProvider* provider){
	CheckInit();
	staticProvider = provider;
}

void NetworkTable::SetClientMode(){
	CheckInit();
	mode = &NetworkTableMode::Client;
}

void NetworkTable::SetServerMode(){
	CheckInit();
	mode = &NetworkTableMode::Server;
}

void NetworkTable::SetTeam(int team){
	char tmp[30];
	sprintf(tmp, "%d.%d.%d.%d\n", 10, team/100, team%100, 2);
	SetIPAddress(tmp);
}

void NetworkTable::SetIPAddress(const char* address){
	CheckInit();
	ipAddress = address;
}

NetworkTable* NetworkTable::GetTable(std::string key) {
	printf("[NT] NetworkTable::GetTable()...\n");
	if(staticProvider==NULL){
		printf("[NT] \tInitializing...\n");
		Initialize();
	}
	std::string tmp(PATH_SEPARATOR);
	tmp+=key;
	printf("[NT] ...Ready to get Table.\n");
	return (NetworkTable*)staticProvider->GetTable(tmp);
}



NetworkTable::NetworkTable(std::string _path, NetworkTableProvider& _provider) :
		path(_path), entryCache(_path), absoluteKeyCache(_path), provider(_provider), node(provider.GetNode()) {
}
NetworkTable::~NetworkTable(){
}

bool NetworkTable::IsConnected() {
	return node.IsConnected();
}

bool NetworkTable::IsServer() {
	return node.IsServer();
}


void NetworkTable::AddConnectionListener(IRemoteConnectionListener* listener, bool immediateNotify) {
	map<IRemoteConnectionListener*, NetworkTableConnectionListenerAdapter*>::iterator itr = connectionListenerMap.find(listener);
	if(itr != connectionListenerMap.end()){
		throw IllegalStateException("Cannot add the same listener twice");
	}
	else{
		NetworkTableConnectionListenerAdapter* adapter = new NetworkTableConnectionListenerAdapter(this, listener);
		connectionListenerMap[listener] = adapter;
		node.AddConnectionListener(adapter, immediateNotify);
	}
}

void NetworkTable::RemoveConnectionListener(IRemoteConnectionListener* listener) {
	map<IRemoteConnectionListener*, NetworkTableConnectionListenerAdapter*>::iterator itr = connectionListenerMap.find(listener);
	if(itr != connectionListenerMap.end()){
		node.RemoveConnectionListener(itr->second);
		delete itr->second;
		connectionListenerMap.erase(itr);
	}
}


void NetworkTable::AddTableListener(ITableListener* listener) {
	AddTableListener(listener, false);
}

void NetworkTable::AddTableListener(ITableListener* listener, bool immediateNotify) {
	std::string tmp(path);
	tmp+=PATH_SEPARATOR;
	NetworkTableListenerAdapter* adapter = new NetworkTableListenerAdapter(tmp, this, listener);
	listenerMap.insert ( pair<ITableListener*,ITableListener*>(listener, adapter) );
	node.AddTableListener(adapter, immediateNotify);
}
void NetworkTable::AddTableListener(std::string key, ITableListener* listener, bool immediateNotify) {
	NetworkTableKeyListenerAdapter* adapter = new NetworkTableKeyListenerAdapter(key, absoluteKeyCache.Get(key), this, listener);
	listenerMap.insert ( pair<ITableListener*,ITableListener*>(listener, adapter) );
	node.AddTableListener(adapter, immediateNotify);
}
void NetworkTable::AddSubTableListener(ITableListener* listener) {
	NetworkTableSubListenerAdapter* adapter = new NetworkTableSubListenerAdapter(path, this, listener);
	listenerMap.insert ( pair<ITableListener*,ITableListener*>(listener, adapter) );
	node.AddTableListener(adapter, true);
}

void NetworkTable::RemoveTableListener(ITableListener* listener) {
	multimap<ITableListener*,ITableListener*>::iterator itr;
	pair<multimap<ITableListener*,ITableListener*>::iterator,multimap<ITableListener*,ITableListener*>::iterator> itrs = listenerMap.equal_range(listener);
	for (itr=itrs.first; itr!=itrs.second; ++itr){
		node.RemoveTableListener(itr->second);
		delete itr->second;
	}
	listenerMap.erase(itrs.first, itrs.second);
}

NetworkTableEntry* NetworkTable::GetEntry(std::string key){
	{ 
		NTSynchronized sync(LOCK);
		return entryCache.Get(key);
	}
}


NetworkTable* NetworkTable::GetSubTable(std::string key) {
	{ 
		NTSynchronized sync(LOCK);
		return (NetworkTable*)provider.GetTable(absoluteKeyCache.Get(key));
	}
}


bool NetworkTable::ContainsKey(std::string key) {
	return node.ContainsKey(absoluteKeyCache.Get(key));
}
	
bool NetworkTable::ContainsSubTable(std::string key){
        std::string subtablePrefix(absoluteKeyCache.Get(key));
        subtablePrefix+=PATH_SEPARATOR;
	std::vector<std::string>* keys = node.GetEntryStore().keys();
	for(unsigned int i = 0; i<keys->size(); ++i){
		if(keys->at(i).compare(0, subtablePrefix.size(), subtablePrefix)==0){
			delete keys;
			return true;
		}
	}
	delete keys;
	return false;
}


void NetworkTable::PutNumber(std::string key, double value) {
	EntryValue eValue;
	eValue.f = value;
	PutValue(key, &DefaultEntryTypes::DOUBLE, eValue);
}


double NetworkTable::GetNumber(std::string key) {
	return node.GetDouble(absoluteKeyCache.Get(key));
}


double NetworkTable::GetNumber(std::string key, double defaultValue) {
	try {
		return node.GetDouble(absoluteKeyCache.Get(key));
	} catch (TableKeyNotDefinedException& e) {
		return defaultValue;
	}
}


void NetworkTable::PutString(std::string key, std::string value) {
	EntryValue eValue;
	eValue.ptr = &value;
	PutValue(key, &DefaultEntryTypes::STRING, eValue);
}


std::string NetworkTable::GetString(std::string key) {
	return node.GetString(absoluteKeyCache.Get(key));
}


std::string NetworkTable::GetString(std::string key, std::string defaultValue) {
	try {
		return node.GetString(absoluteKeyCache.Get(key));
	} catch (TableKeyNotDefinedException& e) {
		return defaultValue;
	}
}


void NetworkTable::PutBoolean(std::string key, bool value) {
	EntryValue eValue;
	eValue.b = value;
	PutValue(key, &DefaultEntryTypes::BOOLEAN, eValue);
}


bool NetworkTable::GetBoolean(std::string key) {
	return node.GetBoolean(absoluteKeyCache.Get(key));
}


bool NetworkTable::GetBoolean(std::string key, bool defaultValue) {
	try {
		return node.GetBoolean(absoluteKeyCache.Get(key));
	} catch (TableKeyNotDefinedException& e) {
		return defaultValue;
	}
}

void NetworkTable::PutValue(std::string key, NetworkTableEntryType* type, EntryValue value){
	NetworkTableEntry* entry = entryCache.Get(key);
	if(entry!=NULL)
		node.PutValue(entry, value);//TODO pass type along or do some kind of type check
	else
		node.PutValue(absoluteKeyCache.Get(key), type, value);
}

void NetworkTable::RetrieveValue(std::string key, ComplexData& externalValue) {
	node.retrieveValue(absoluteKeyCache.Get(key), externalValue);
}


void NetworkTable::PutValue(std::string key, ComplexData& value){
	EntryValue eValue;
	eValue.ptr = &value;
	PutValue(key, &value.GetType(), eValue);
}


EntryValue NetworkTable::GetValue(std::string key) {
	return node.GetValue(absoluteKeyCache.Get(key));
}

EntryValue NetworkTable::GetValue(std::string key, EntryValue defaultValue) {
	try {
		return node.GetValue(absoluteKeyCache.Get(key));
	} catch(TableKeyNotDefinedException& e){
		return defaultValue;
	}
}






//NetworkTableKeyCache
NetworkTableKeyCache::NetworkTableKeyCache(std::string _path):path(_path){}
NetworkTableKeyCache::~NetworkTableKeyCache(){}
std::string NetworkTableKeyCache::Calc(const std::string& key){
  std::string tmp(path);
  tmp+=NetworkTable::PATH_SEPARATOR;
  tmp+=key;
  return tmp;
}
//Entry Cache
EntryCache::EntryCache(std::string& _path):path(_path){}
EntryCache::~EntryCache(){}

NetworkTableEntry* EntryCache::Get(std::string& key){
	return cache[key];
}

