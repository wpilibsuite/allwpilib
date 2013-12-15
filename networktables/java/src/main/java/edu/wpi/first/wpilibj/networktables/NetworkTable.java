package edu.wpi.first.wpilibj.networktables;

import edu.wpi.first.wpilibj.networktables2.*;
import edu.wpi.first.wpilibj.networktables2.thread.*;
import edu.wpi.first.wpilibj.networktables2.util.*;
import edu.wpi.first.wpilibj.networktables2.util.List;
import edu.wpi.first.wpilibj.tables.*;
import java.io.*;
import java.util.*;

/**
 * 
 * @author Fredric
 * @author mwills
 * 
 */

public class NetworkTable implements ITable, IRemote {
	private static final NTThreadManager threadManager = new DefaultThreadManager();
	
	/**
	 * The path separator for sub-tables and keys
	 * 
	 */
	public static final char PATH_SEPARATOR = '/';
	/**
	 * The default port that network tables operates on
	 */
	public static final int DEFAULT_PORT = 1735;
	
	
	
	
	
	
	
	private static NetworkTableProvider staticProvider = null;
	
	private static NetworkTableMode mode = NetworkTableMode.Server;
	private static int port = DEFAULT_PORT;
	private static String ipAddress = null;
	
	private synchronized static void checkInit(){
		if(staticProvider!=null)
			throw new IllegalStateException("Network tables has already been initialized");
	}
	/**
	 * @throws IOException
	 */
	public synchronized static void initialize() throws IOException {
		checkInit();
		staticProvider = new NetworkTableProvider(mode.createNode(ipAddress, port, threadManager));
	}
        
	/**
	 * set the table provider for static network tables methods
	 * This must be called before initalize or getTable
	 */
	public synchronized static void setTableProvider(NetworkTableProvider provider) {
		checkInit();
		staticProvider = provider;
	}
	/**
	 * set that network tables should be a server
	 * This must be called before initalize or getTable
	 */
	public synchronized static void setServerMode(){
		checkInit();
		mode = NetworkTableMode.Server;
	}
	/**
	 * set that network tables should be a client
	 * This must be called before initalize or getTable
	 */
	public synchronized static void setClientMode(){
		checkInit();
		mode = NetworkTableMode.Client;
	}
	
	/**
	 * set the team the robot is configured for (this will set the ip address that network tables will connect to in client mode)
	 * This must be called before initalize or getTable
	 * @param team the team number
	 */
	public synchronized static void setTeam(int team){
		setIPAddress("10." + (team / 100) + "." + (team % 100) + ".2");
	}
	/**
	 * @param address the adress that network tables will connect to in client mode
	 */
	public synchronized static void setIPAddress(final String address){
		checkInit();
		ipAddress = address;
	}
	/**
	 * Gets the table with the specified key. If the table does not exist, a new table will be created.<br>
	 * This will automatically initialize network tables if it has not been already
	 * 
	 * @param key
	 *            the key name
	 * @return the network table requested
	 */
	public synchronized static NetworkTable getTable(String key) {
		if(staticProvider==null)
			try {
				initialize();
			} catch (IOException e) {
				throw new RuntimeException("NetworkTable could not be initialized: "+e+": "+e.getMessage());
			}
		return (NetworkTable)staticProvider.getTable(PATH_SEPARATOR+key);
	}
	

	
	private final String path;
	private final EntryCache entryCache;
	private final NetworkTableKeyCache absoluteKeyCache;
	private final NetworkTableProvider provider;
	private final NetworkTableNode node;

	NetworkTable(String path, NetworkTableProvider provider) {
		this.path = path;
		entryCache = new EntryCache(path);
		absoluteKeyCache = new NetworkTableKeyCache(path);
		this.provider = provider;
		node = provider.getNode();
	}
	public String toString(){
		return "NetworkTable: "+path;
	}
	
	public boolean isConnected() {
		return node.isConnected();
	}

	public boolean isServer() {
		return node.isServer();
	}
	
	
	static class NetworkTableKeyCache extends StringCache{
		private final String path;

		public NetworkTableKeyCache(String path) {
			this.path = path;
		}

		public String calc(String key) {
			return path + PATH_SEPARATOR + key;
		}
	}
	class EntryCache {
		private final Hashtable cache = new Hashtable();
		private final String path;

		public EntryCache(String path) {
			this.path = path;
		}
		
		public NetworkTableEntry get(final String key){
			NetworkTableEntry cachedValue = (NetworkTableEntry)cache.get(key);
			if(cachedValue==null){
				cachedValue = node.getEntryStore().getEntry(absoluteKeyCache.get(key));
				if(cachedValue!=null)
					cache.put(key, cachedValue);
			}
			return cachedValue;
		}
	}
	
	
	private final Hashtable connectionListenerMap = new Hashtable();
	public void addConnectionListener(IRemoteConnectionListener listener, boolean immediateNotify) {
		NetworkTableConnectionListenerAdapter adapter = (NetworkTableConnectionListenerAdapter)connectionListenerMap.get(listener);
		if(adapter!=null)
			throw new IllegalStateException("Cannot add the same listener twice");
		adapter = new NetworkTableConnectionListenerAdapter(this, listener);
		connectionListenerMap.put(listener, adapter);
		node.addConnectionListener(adapter, immediateNotify);
	}

	public void removeConnectionListener(IRemoteConnectionListener listener) {
		NetworkTableConnectionListenerAdapter adapter = (NetworkTableConnectionListenerAdapter)connectionListenerMap.get(listener);
		if(adapter!=null)
			node.removeConnectionListener(adapter);
	}
	

	public void addTableListener(ITableListener listener) {
		addTableListener(listener, false);
	}

	private final Hashtable listenerMap = new Hashtable();
	public void addTableListener(ITableListener listener, boolean immediateNotify) {
            List adapters = (List)listenerMap.get(listener);
            if(adapters==null){
                    adapters = new List();
                    listenerMap.put(listener, adapters);
            }
            NetworkTableListenerAdapter adapter = new NetworkTableListenerAdapter(path+PATH_SEPARATOR, this, listener);
            adapters.add(adapter);
            node.addTableListener(adapter, immediateNotify);
	}
	public void addTableListener(String key, ITableListener listener, boolean immediateNotify) {
		List adapters = (List)listenerMap.get(listener);
		if(adapters==null){
			adapters = new List();
			listenerMap.put(listener, adapters);
		}
		NetworkTableKeyListenerAdapter adapter = new NetworkTableKeyListenerAdapter(key, absoluteKeyCache.get(key), this, listener);
		adapters.add(adapter);
		node.addTableListener(adapter, immediateNotify);
	}
	public void addSubTableListener(final ITableListener listener) {
		List adapters = (List)listenerMap.get(listener);
		if(adapters==null){
			adapters = new List();
			listenerMap.put(listener, adapters);
		}
		NetworkTableSubListenerAdapter adapter = new NetworkTableSubListenerAdapter(path, this, listener);
		adapters.add(adapter);
		node.addTableListener(adapter, true);
	}

	public void removeTableListener(ITableListener listener) {
		List adapters = (List)listenerMap.get(listener);
		if(adapters!=null){
			for(int i = 0; i<adapters.size(); ++i)
				node.removeTableListener((ITableListener) adapters.get(i));
			adapters.clear();
		}
	}
	
	private synchronized NetworkTableEntry getEntry(String key){
		return entryCache.get(key);
	}

	/**
	 * Returns the table at the specified key. If there is no table at the
	 * specified key, it will create a new table
	 * 
	 * @param key
	 *            the key name
	 * @return the networktable to be returned
	 */
	public synchronized ITable getSubTable(String key) {
		return (NetworkTable)provider.getTable(absoluteKeyCache.get(key));
	}

	/**
	 * Checks the table and tells if it contains the specified key
	 * 
	 * @param key
	 *            the key to be checked
	 */
	public boolean containsKey(String key) {
            return node.containsKey(absoluteKeyCache.get(key));
	}
        
	public boolean containsSubTable(String key){
            String subtablePrefix = absoluteKeyCache.get(key)+PATH_SEPARATOR;
            List keys = node.getEntryStore().keys();
            for(int i = 0; i<keys.size(); ++i){
                if(((String)keys.get(i)).startsWith(subtablePrefix))
                    return true;
            }
            return false;
        }

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
	public void putNumber(String key, double value) {
		putValue(key, new Double(value));//TODO cache doubles
	}

	/**
	 * Returns the key that the name maps to.
	 * 
	 * @param key
	 *            the key name
	 * @return the key
	 * @throws TableKeyNotDefinedException
	 *             if the specified key is null
	 */
	public double getNumber(String key) throws TableKeyNotDefinedException {
		return node.getDouble(absoluteKeyCache.get(key));
	}

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
	public double getNumber(String key, double defaultValue) {
		try {
			return node.getDouble(absoluteKeyCache.get(key));
		} catch (TableKeyNotDefinedException e) {
			return defaultValue;
		}
	}

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
	public void putString(String key, String value) {
		putValue(key, value);
	}

	/**
	 * Returns the key that the name maps to.
	 * 
	 * @param key
	 *            the key name
	 * @return the key
	 * @throws TableKeyNotDefinedException
	 *             if the specified key is null
	 */
	public String getString(String key) throws TableKeyNotDefinedException {
		return node.getString(absoluteKeyCache.get(key));
	}

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
	public String getString(String key, String defaultValue) {
		try {
			return node.getString(absoluteKeyCache.get(key));
		} catch (TableKeyNotDefinedException e) {
			return defaultValue;
		}
	}

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
	public void putBoolean(String key, boolean value) {
		putValue(key, value?Boolean.TRUE:Boolean.FALSE);
	}

	/**
	 * Returns the key that the name maps to.
	 * 
	 * @param key
	 *            the key name
	 * @return the key
	 * @throws TableKeyNotDefinedException
	 *             if the specified key is null
	 */
	public boolean getBoolean(String key) throws TableKeyNotDefinedException {
		return node.getBoolean(absoluteKeyCache.get(key));
	}

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
	public boolean getBoolean(String key, boolean defaultValue) {
		try {
			return node.getBoolean(absoluteKeyCache.get(key));
		} catch (TableKeyNotDefinedException e) {
			return defaultValue;
		}
	}
	

        public void retrieveValue(String key, Object externalValue) {
            node.retrieveValue(absoluteKeyCache.get(key), externalValue);
        }
        
	/**
	 * Maps the specified key to the specified value in this table. The key can
	 * not be null. The value can be retrieved by calling the get method with a
	 * key that is equal to the original key.
	 * 
	 * @param key the key name
	 * @param value the value to be put
	 */
	public void putValue(String key, Object value){
		NetworkTableEntry entry = entryCache.get(key);
		if(entry!=null)
			node.putValue(entry, value);
		else
			node.putValue(absoluteKeyCache.get(key), value);
	}
	
	/**
	 * Returns the key that the name maps to.
	 * NOTE: If the value is a double, it will return a Double object,
	 * not a primitive.  To get the primitive, use getDouble
	 * 
	 * @param key
	 *            the key name
	 * @return the key
	 * @throws TableKeyNotDefinedException
	 *             if the specified key is null
	 */
	public Object getValue(String key) throws TableKeyNotDefinedException {
		return node.getValue(absoluteKeyCache.get(key));
	}
	
	/**
	 * Returns the key that the name maps to. If the key is null, it will return
	 * the default value
	 * NOTE: If the value is a double, it will return a Double object,
	 * not a primitive.  To get the primitive, use getDouble
	 * 
	 * @param key
	 *            the key name
	 * @param defaultValue
	 *            the default value if the key is null
	 * @return the key
	 */
	public Object getValue(String key, Object defaultValue) {
		try {
			return node.getValue(absoluteKeyCache.get(key));
		} catch(TableKeyNotDefinedException e){
			return defaultValue;
		}
	}
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        

     /*
      * Depricated Methods
      */
     /**
      * @deprecated 
     * Maps the specified key to the specified value in this table.
     * The key can not be null.
     * The value can be retrieved by calling the get method with a key that is equal to the original key.
     * @param key the key
     * @param value the value
     * @throws IllegalArgumentException if key is null
     */
    public void putInt(String key, int value) {
        putNumber(key, value);
    }

    /**
     * @deprecated 
     * Returns the value at the specified key.
     * @param key the key
     * @return the value
     * @throws TableKeyNotDefinedException if there is no value mapped to by the key
     * @throws IllegalArgumentException if the value mapped to by the key is not an int
     * @throws IllegalArgumentException if the key is null
     */
    public int getInt(String key) throws TableKeyNotDefinedException{
        return (int) getNumber(key);
    }

    /**
     * @deprecated 
     * Returns the value at the specified key.
     * @param key the key
     * @param defaultValue the value returned if the key is undefined
     * @return the value
     * @throws NetworkTableKeyNotDefined if there is no value mapped to by the key
     * @throws IllegalArgumentException if the value mapped to by the key is not an int
     * @throws IllegalArgumentException if the key is null
     */
    public int getInt(String key, int defaultValue) throws TableKeyNotDefinedException{
        try {
            return (int) getNumber(key);
        } catch (NoSuchElementException ex) {
            return defaultValue;
        }
    }

    /**
     * @deprecated 
     * Maps the specified key to the specified value in this table.
     * The key can not be null.
     * The value can be retrieved by calling the get method with a key that is equal to the original key.
     * @param key the key
     * @param value the value
     * @throws IllegalArgumentException if key is null
     */
    public void putDouble(String key, double value) {
        putNumber(key, value);
    }

    /**
     * @deprecated 
     * Returns the value at the specified key.
     * @param key the key
     * @return the value
     * @throws NoSuchEleNetworkTableKeyNotDefinedmentException if there is no value mapped to by the key
     * @throws IllegalArgumentException if the value mapped to by the key is not a double
     * @throws IllegalArgumentException if the key is null
     */
    public double getDouble(String key) throws TableKeyNotDefinedException{
        return getNumber(key);
    }

    /**
     * @deprecated 
     * Returns the value at the specified key.
     * @param key the key
     * @param defaultValue the value returned if the key is undefined
     * @return the value
     * @throws NoSuchEleNetworkTableKeyNotDefinedmentException if there is no value mapped to by the key
     * @throws IllegalArgumentException if the value mapped to by the key is not a double
     * @throws IllegalArgumentException if the key is null
     */
    public double getDouble(String key, double defaultValue) {
        return getNumber(key, defaultValue);
    }
}
