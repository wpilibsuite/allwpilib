#ifndef _NETWORKTABLE_H_
#define _NETWORKTABLE_H_

class NetworkTable;
class NetworkTableKeyCache;
class EntryCache;

#include <map>
#include <string>
#include "tables/ITable.h"
#include "tables/IRemote.h"
#include "networktables2/thread/NTThreadManager.h"
#include "networktables2/NetworkTableEntry.h"
#include "networktables2/util/StringCache.h"
#include "networktables/NetworkTableProvider.h"
#include "networktables/NetworkTableMode.h"
#include "networktables2/thread/DefaultThreadManager.h"
#include "networktables/NetworkTableConnectionListenerAdapter.h"
#include "networktables/NetworkTableListenerAdapter.h"
#include "tables/IRemoteConnectionListener.h"



using namespace std;



class NetworkTableKeyCache: public StringCache{
private:
	const std::string path;

public: 
	NetworkTableKeyCache(std::string path);
	~NetworkTableKeyCache();

	std::string Calc(const std::string& key);
};


class EntryCache {
private:
	map<std::string, NetworkTableEntry*> cache;
	std::string& path;
public:
	EntryCache(std::string& path);
	~EntryCache();
	
	NetworkTableEntry* Get(std::string& key);
};



class NetworkTable : public ITable, IRemote {
	
private:
	static DefaultThreadManager threadManager;
	static NetworkTableProvider* staticProvider;
	static NetworkTableNode* staticNode;
	static void* streamFactory;
	static NetworkTableEntryTypeManager* typeManager;
	static NetworkTableMode* mode;
	static int port;
	static std::string ipAddress;
	static NTReentrantSemaphore STATIC_LOCK;
	
	
	
	std::string path;
	EntryCache entryCache;
	NetworkTableKeyCache absoluteKeyCache;
	NetworkTableProvider& provider;
	NetworkTableNode& node;
	NTReentrantSemaphore LOCK;
	
	
	map<IRemoteConnectionListener*, NetworkTableConnectionListenerAdapter*> connectionListenerMap;
	multimap<ITableListener*, ITableListener*> listenerMap;
	
	static void CheckInit();
	
	NetworkTableEntry* GetEntry(std::string key);
	
public:

	static const char PATH_SEPARATOR_CHAR;
	/**
	 * The path separator for sub-tables and keys
	 * 
	 */
	static const std::string PATH_SEPARATOR;
	/**
	 * The default port that network tables operates on
	 */
	static const int DEFAULT_PORT;
	
	/**
	 * @throws IOException
	 */
	static void Initialize();
	static void Shutdown();

	/**
	 * set the table provider for static network tables methods
	 * This must be called before getTable
	 */
	static void SetTableProvider(NetworkTableProvider* provider);
	
	/**
	 * set that network tables should be a client
	 * This must be called before initalize or GetTable
	 */
	static void SetClientMode();
	
	/**
	 * set that network tables should be a server
	 * This must be called before initalize or GetTable
	 */
	static void SetServerMode();
	
	/**
	 * set the team the robot is configured for (this will set the ip address that network tables will connect to in client mode)
	 * This must be called before initalize or GetTable
	 * @param team the team number
	 */
	static void SetTeam(int team);
	/**
	 * @param address the adress that network tables will connect to in client mode
	 */
	static void SetIPAddress(const char* address);
	/**
	 * Gets the table with the specified key. If the table does not exist, a new table will be created.<br>
	 * This will automatically initialize network tables if it has not been already
	 * 
	 * @param key
	 *            the key name
	 * @return the network table requested
	 */
	static NetworkTable* GetTable(std::string key);
	

	NetworkTable(std::string path, NetworkTableProvider& provider);
	virtual ~NetworkTable();
	
	bool IsConnected();

	bool IsServer();
	
	
	
	
	
	
	
	void AddConnectionListener(IRemoteConnectionListener* listener, bool immediateNotify);

	void RemoveConnectionListener(IRemoteConnectionListener* listener);
	

	void AddTableListener(ITableListener* listener);

	
	void AddTableListener(ITableListener* listener, bool immediateNotify);
	void AddTableListener(std::string key, ITableListener* listener, bool immediateNotify);
	void AddSubTableListener(ITableListener* listener);

	void RemoveTableListener(ITableListener* listener);
	
	

	/**
	 * Returns the table at the specified key. If there is no table at the
	 * specified key, it will create a new table
	 * 
	 * @param key
	 *            the key name
	 * @return the networktable to be returned
	 */
	NetworkTable* GetSubTable(std::string key);


	/**
	 * Checks the table and tells if it contains the specified key
	 * 
	 * @param key
	 *            the key to be checked
	 */
	bool ContainsKey(std::string key);
        
	bool ContainsSubTable(std::string key);

	/**
	 * Maps the specified key to the specified value in this table. The key can
	 * not be null. The value can be retrieved by calling the get method with a
	 * key that is equal to the original key.
	 * 
	 * @param key
	 *            the key
	 * @param value
	 *            the value
	 */
	void PutNumber(std::string key, double value);

	/**
	 * Returns the key that the name maps to.
	 * 
	 * @param key
	 *            the key name
	 * @return the key
	 * @throws TableKeyNotDefinedException
	 *             if the specified key is null
	 */
	double GetNumber(std::string key);

	/**
	 * Returns the key that the name maps to. If the key is null, it will return
	 * the default value
	 * 
	 * @param key
	 *            the key name
	 * @param defaultValue
	 *            the default value if the key is null
	 * @return the key
	 */
	double GetNumber(std::string key, double defaultValue);

	/**
	 * Maps the specified key to the specified value in this table. The key can
	 * not be null. The value can be retrieved by calling the get method with a
	 * key that is equal to the original key.
	 * 
	 * @param key
	 *            the key
	 * @param value
	 *            the value
	 */
	void PutString(std::string key, std::string value);

	/**
	 * Returns the key that the name maps to.
	 * 
	 * @param key
	 *            the key name
	 * @return the key
	 * @throws TableKeyNotDefinedException
	 *             if the specified key is null
	 */
	std::string GetString(std::string key);

	/**
	 * Returns the key that the name maps to. If the key is null, it will return
	 * the default value
	 * 
	 * @param key
	 *            the key name
	 * @param defaultValue
	 *            the default value if the key is null
	 * @return the key
	 */
	std::string GetString(std::string key, std::string defaultValue);

	/**
	 * Maps the specified key to the specified value in this table. The key can
	 * not be null. The value can be retrieved by calling the get method with a
	 * key that is equal to the original key.
	 * 
	 * @param key
	 *            the key
	 * @param value
	 *            the value
	 */
	void PutBoolean(std::string key, bool value);

	/**
	 * Returns the key that the name maps to.
	 * 
	 * @param key
	 *            the key name
	 * @return the key
	 * @throws TableKeyNotDefinedException
	 *             if the specified key is null
	 */
	bool GetBoolean(std::string key);

	/**
	 * Returns the key that the name maps to. If the key is null, it will return
	 * the default value
	 * 
	 * @param key
	 *            the key name
	 * @param defaultValue
	 *            the default value if the key is null
	 * @return the key
	 */
	bool GetBoolean(std::string key, bool defaultValue);
	

	void PutValue(std::string key, NetworkTableEntryType* type, EntryValue value);
	

    void RetrieveValue(std::string key, ComplexData& externalValue);
        
	/**
	 * Maps the specified key to the specified value in this table. The key can
	 * not be null. The value can be retrieved by calling the get method with a
	 * key that is equal to the original key.
	 * 
	 * @param key the key name
	 * @param value the value to be put
	 */
	void PutValue(std::string key, ComplexData& value);
	
	/**
	 * Returns the key that the name maps to.
	 * NOTE: If the value is a double, it will return a Double object,
	 * not a primitive.  To get the primitive, use GetDouble
	 * 
	 * @param key
	 *            the key name
	 * @return the key
	 * @throws TableKeyNotDefinedException
	 *             if the specified key is null
	 */
	EntryValue GetValue(std::string key);
	
	/**
	 * Returns the key that the name maps to. If the key is null, it will return
	 * the default value
	 * NOTE: If the value is a double, it will return a Double object,
	 * not a primitive.  To get the primitive, use GetDouble
	 * 
	 * @param key
	 *            the key name
	 * @param defaultValue
	 *            the default value if the key is null
	 * @return the key
	 */
	 EntryValue GetValue(std::string key, EntryValue defaultValue);

};

#endif
