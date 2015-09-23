package edu.wpi.first.wpilibj.networktables;

import edu.wpi.first.wpilibj.tables.*;
import edu.wpi.first.wpilibj.networktables2.type.*;
import java.io.*;
import java.util.*;

/**
 * A network table that knows its subtable path.
 * 
 */
public class NetworkTable implements ITable, IRemote {
  /**
   * The path separator for sub-tables and keys
   *
   */
  public static final char PATH_SEPARATOR = '/';
  /**
   * The default port that network tables operates on
   */
  public static final int DEFAULT_PORT = 1735;

  private static boolean client = false;
  private static boolean running = false;
  private static int port = DEFAULT_PORT;
  private static String ipAddress = "";
  private static String persistentFilename = "networktables.ini";

  private synchronized static void checkInit() {
    if (running)
      throw new IllegalStateException(
          "Network tables has already been initialized");
  }

  /**
   * initializes network tables
   */
  public synchronized static void initialize() {
    if (running)
      shutdown();
    if (client)
      NetworkTablesJNI.startClient(ipAddress, port);
    else
      NetworkTablesJNI.startServer(persistentFilename, "", port);
    running = true;
  }

  /**
   * shuts down network tables
   */
  public synchronized static void shutdown() {
    if (!running)
      return;
    if (client)
      NetworkTablesJNI.stopClient();
    else
      NetworkTablesJNI.stopServer();
    running = false;
  }

  /**
   * set that network tables should be a server
   * This must be called before initalize or getTable
   */
  public synchronized static void setServerMode() {
    if (!client)
      return;
    checkInit();
    client = false;
  }

  /**
   * set that network tables should be a client
   * This must be called before initalize or getTable
   */
  public synchronized static void setClientMode() {
    if (client)
      return;
    checkInit();
    client = true;
  }

  /**
   * set the team the robot is configured for (this will set the ip address that
   * network tables will connect to in client mode)
   * This must be called before initalize or getTable
   * @param team the team number
   */
  public synchronized static void setTeam(int team) {
    setIPAddress("10." + (team / 100) + "." + (team % 100) + ".2");
  }

  /**
   * @param address the adress that network tables will connect to in client
   * mode
   */
  public synchronized static void setIPAddress(final String address) {
    if (ipAddress == address)
      return;
    checkInit();
    ipAddress = address;
  }

  /**
   * @param port the port number that network tables will connect to in client
   * mode or listen to in server mode
   */
  public synchronized static void setPort(int aport) {
    if (port == aport)
      return;
    checkInit();
    port = aport;
  }

  /**
   * Sets the persistent filename.
   * @param filename the filename that the network tables server uses for
   * automatic loading and saving of persistent values
   */
  public synchronized static void setPersistentFilename(final String filename) {
    if (persistentFilename == filename)
      return;
    checkInit();
    persistentFilename = filename;
  }

  /**
   * Sets the network identity.
   * This is provided in the connection info on the remote end.
   * @param name identity
   */
  public static void setNetworkIdentity(String name) {
    NetworkTablesJNI.setNetworkIdentity(name);
  }

  public static boolean[] toNative(Boolean[] arr) {
    boolean[] out = new boolean[arr.length];
    for (int i = 0; i < arr.length; i++)
      out[i] = arr[i];
    return out;
  }

  public static double[] toNative(Double[] arr) {
    double[] out = new double[arr.length];
    for (int i = 0; i < arr.length; i++)
      out[i] = arr[i];
    return out;
  }

  public static Boolean[] fromNative(boolean[] arr) {
    Boolean[] out = new Boolean[arr.length];
    for (int i = 0; i < arr.length; i++)
      out[i] = arr[i];
    return out;
  }

  public static Double[] fromNative(double[] arr) {
    Double[] out = new Double[arr.length];
    for (int i = 0; i < arr.length; i++)
      out[i] = arr[i];
    return out;
  }

  /**
   * Gets the table with the specified key. If the table does not exist, a new
   *table will be created.<br>
   * This will automatically initialize network tables if it has not been
   *already
   *
   * @param key
   *            the key name
   * @return the network table requested
   */
  public synchronized static NetworkTable getTable(String key) {
    if (!running)
      initialize();
    if (key.isEmpty())
      return new NetworkTable(key);
    return new NetworkTable(PATH_SEPARATOR + key);
  }

  private final String path;

  NetworkTable(String path) {
    this.path = path;
  }
  public String toString() { return "NetworkTable: " + path; }

  public static ConnectionInfo[] connections() {
    return NetworkTablesJNI.getConnections();
  }

  public boolean isConnected() {
    ConnectionInfo[] conns = NetworkTablesJNI.getConnections();
    return conns.length > 0;
  }

  public boolean isServer() {
    return !client;
  }

  private class ListenerBase {
    public int uid;
  }

  private class ConnectionListenerAdapter extends ListenerBase implements NetworkTablesJNI.ConnectionListenerFunction {
    private final IRemote targetSource;
    private final IRemoteConnectionListener targetListener;

    public ConnectionListenerAdapter(IRemote targetSource, IRemoteConnectionListener targetListener) {
      this.targetSource = targetSource;
      this.targetListener = targetListener;
    }

    public void apply(int uid, boolean connected, ConnectionInfo conn) {
      if (connected)
        targetListener.connected(targetSource);
      else
        targetListener.disconnected(targetSource);
    }
  }

  private final Hashtable<IRemoteConnectionListener,ConnectionListenerAdapter> connectionListenerMap = new Hashtable<IRemoteConnectionListener,ConnectionListenerAdapter>();
  public synchronized void addConnectionListener(IRemoteConnectionListener listener,
                                                 boolean immediateNotify) {
    ConnectionListenerAdapter adapter = connectionListenerMap.get(listener);
    if (adapter != null)
      throw new IllegalStateException("Cannot add the same listener twice");
    adapter = new ConnectionListenerAdapter(this, listener);
    adapter.uid = NetworkTablesJNI.addConnectionListener(adapter, immediateNotify);
    connectionListenerMap.put(listener, adapter);
  }

  public synchronized void removeConnectionListener(IRemoteConnectionListener listener) {
    ConnectionListenerAdapter adapter = connectionListenerMap.get(listener);
    if (adapter != null) {
      NetworkTablesJNI.removeConnectionListener(adapter.uid);
      connectionListenerMap.remove(listener);
    }
  }

  public void addTableListener(ITableListener listener) {
    addTableListener(listener, false, false);
  }

  public void addTableListener(ITableListener listener,
                               boolean immediateNotify) {
    addTableListener(listener, immediateNotify, false);
  }

  private class TableListenerAdapter extends ListenerBase implements NetworkTablesJNI.EntryListenerFunction {
    private final int prefixLen;
    private final ITable targetSource;
    private final ITableListener targetListener;

    public TableListenerAdapter(int prefixLen, ITable targetSource, ITableListener targetListener) {
      this.prefixLen = prefixLen;
      this.targetSource = targetSource;
      this.targetListener = targetListener;
    }

    public void apply(int uid, String key, Object value, boolean isNew) {
      String relativeKey = key.substring(prefixLen);
      if (relativeKey.indexOf(PATH_SEPARATOR) != -1)
        return;
      targetListener.valueChanged(targetSource, relativeKey, value, isNew);
    }
  }

  private final Hashtable<ITableListener,List<ListenerBase>> listenerMap = new Hashtable<ITableListener,List<ListenerBase>>();
  public synchronized void addTableListener(ITableListener listener,
                                            boolean immediateNotify,
                                            boolean localNotify) {
    List<ListenerBase> adapters = listenerMap.get(listener);
    if (adapters == null) {
      adapters = new ArrayList<ListenerBase>();
      listenerMap.put(listener, adapters);
    }
    TableListenerAdapter adapter =
        new TableListenerAdapter(path.length() + 1, this, listener);
    adapter.uid = NetworkTablesJNI.addEntryListener(path + PATH_SEPARATOR, adapter, immediateNotify, localNotify);
    adapters.add(adapter);
  }

  public void addTableListener(String key, ITableListener listener,
                               boolean immediateNotify) {
    addTableListener(key, listener, immediateNotify, false);
  }

  private class KeyListenerAdapter extends ListenerBase implements NetworkTablesJNI.EntryListenerFunction {
    private final String relativeKey;
    private final String fullKey;
    private final ITable targetSource;
    private final ITableListener targetListener;

    public KeyListenerAdapter(String relativeKey, String fullKey, ITable targetSource, ITableListener targetListener) {
      this.relativeKey = relativeKey;
      this.fullKey = fullKey;
      this.targetSource = targetSource;
      this.targetListener = targetListener;
    }

    public void apply(int uid, String key, Object value, boolean isNew) {
      if (!key.equals(fullKey))
        return;
      targetListener.valueChanged(targetSource, relativeKey, value, isNew);
    }
  }

  public synchronized void addTableListener(String key, ITableListener listener,
                                            boolean immediateNotify,
                                            boolean localNotify) {
    List<ListenerBase> adapters = listenerMap.get(listener);
    if (adapters == null) {
      adapters = new ArrayList<ListenerBase>();
      listenerMap.put(listener, adapters);
    }
    String fullKey = path + PATH_SEPARATOR + key;
    KeyListenerAdapter adapter =
        new KeyListenerAdapter(key, fullKey, this, listener);
    adapter.uid = NetworkTablesJNI.addEntryListener(fullKey, adapter, immediateNotify, localNotify);
    adapters.add(adapter);
  }

  public void addSubTableListener(final ITableListener listener) {
    addSubTableListener(listener, false);
  }

  private class SubListenerAdapter extends ListenerBase implements NetworkTablesJNI.EntryListenerFunction {
    private final int prefixLen;
    private final ITable targetSource;
    private final ITableListener targetListener;
    private final Set<String> notifiedTables = new HashSet<String>();

    public SubListenerAdapter(int prefixLen, ITable targetSource, ITableListener targetListener) {
      this.prefixLen = prefixLen;
      this.targetSource = targetSource;
      this.targetListener = targetListener;
    }

    public void apply(int uid, String key, Object value, boolean isNew) {
      String relativeKey = key.substring(prefixLen);
      int endSubTable = relativeKey.indexOf(PATH_SEPARATOR);
      if (endSubTable == -1)
        return;
      String subTableKey = relativeKey.substring(0, endSubTable);
      if (notifiedTables.contains(subTableKey))
        return;
      notifiedTables.add(subTableKey);
      targetListener.valueChanged(targetSource, subTableKey, targetSource.getSubTable(subTableKey), true);
    }
  }

  public synchronized void addSubTableListener(final ITableListener listener,
                                               boolean localNotify) {
    List<ListenerBase> adapters = listenerMap.get(listener);
    if (adapters == null) {
      adapters = new ArrayList<ListenerBase>();
      listenerMap.put(listener, adapters);
    }
    SubListenerAdapter adapter =
        new SubListenerAdapter(path.length() + 1, this, listener);
    adapter.uid = NetworkTablesJNI.addEntryListener(path + PATH_SEPARATOR, adapter, true, localNotify);
    adapters.add(adapter);
  }

  public synchronized void removeTableListener(ITableListener listener) {
    List<ListenerBase> adapters = listenerMap.get(listener);
    if (adapters != null) {
      for (int i = 0; i < adapters.size(); ++i)
        NetworkTablesJNI.removeEntryListener(adapters.get(i).uid);
      adapters.clear();
    }
  }

  /**
   * Returns the table at the specified key. If there is no table at the
   * specified key, it will create a new table
   *
   * @param key
   *            the key name
   * @return the networktable to be returned
   */
  public ITable getSubTable(String key) {
    return new NetworkTable(path + PATH_SEPARATOR + key);
  }

  /**
   * Checks the table and tells if it contains the specified key
   *
   * @param key
   *            the key to be checked
   */
  public boolean containsKey(String key) {
    return NetworkTablesJNI.containsKey(path + PATH_SEPARATOR + key);
  }

  public boolean containsSubTable(String key) {
    EntryInfo[] entries = NetworkTablesJNI.getEntries(path + PATH_SEPARATOR + key + PATH_SEPARATOR, 0);
    return entries.length != 0;
  }

  /**
   * @param types bitmask of types; 0 is treated as a "don't care".
   * @return keys currently in the table
   */
  public Set<String> getKeys(int types) {
    Set<String> keys = new HashSet<String>();
    int prefixLen = path.length() + 1;
    for (EntryInfo entry : NetworkTablesJNI.getEntries(path + PATH_SEPARATOR, types)) {
      String relativeKey = entry.name.substring(prefixLen);
      if (relativeKey.indexOf(PATH_SEPARATOR) != -1)
        continue;
      keys.add(relativeKey);
    }
    return keys;
  }

  /**
   * @return keys currently in the table
   */
  public Set<String> getKeys() {
    return getKeys(0);
  }

  /**
   * @return subtables currently in the table
   */
  public Set<String> getSubTables() {
    Set<String> keys = new HashSet<String>();
    int prefixLen = path.length() + 1;
    for (EntryInfo entry : NetworkTablesJNI.getEntries(path + PATH_SEPARATOR, 0)) {
      String relativeKey = entry.name.substring(prefixLen);
      int endSubTable = relativeKey.indexOf(PATH_SEPARATOR);
      if (endSubTable == -1)
        continue;
      keys.add(relativeKey.substring(0, endSubTable));
    }
    return keys;
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
   * @return False if the table key already exists with a different type
   */
  public boolean putNumber(String key, double value) {
    return NetworkTablesJNI.putDouble(path + PATH_SEPARATOR + key, value);
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
    return NetworkTablesJNI.getDouble(path + PATH_SEPARATOR + key);
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
    return NetworkTablesJNI.getDouble(path + PATH_SEPARATOR + key, defaultValue);
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
   * @return False if the table key already exists with a different type
   */
  public boolean putString(String key, String value) {
    return NetworkTablesJNI.putString(path + PATH_SEPARATOR + key, value);
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
    return NetworkTablesJNI.getString(path + PATH_SEPARATOR + key);
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
    return NetworkTablesJNI.getString(path + PATH_SEPARATOR + key, defaultValue);
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
   * @return False if the table key already exists with a different type
   */
  public boolean putBoolean(String key, boolean value) {
    return NetworkTablesJNI.putBoolean(path + PATH_SEPARATOR + key, value);
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
    return NetworkTablesJNI.getBoolean(path + PATH_SEPARATOR + key);
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
    return NetworkTablesJNI.getBoolean(path + PATH_SEPARATOR + key, defaultValue);
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
   * @return False if the table key already exists with a different type
   */
  public boolean putBooleanArray(String key, boolean[] value) {
    return NetworkTablesJNI.putBooleanArray(path + PATH_SEPARATOR + key, value);
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
   * @return False if the table key already exists with a different type
   */
  public boolean putBooleanArray(String key, Boolean[] value) {
    return putBooleanArray(key, toNative(value));
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
  public boolean[] getBooleanArray(String key) throws TableKeyNotDefinedException {
    return NetworkTablesJNI.getBooleanArray(path + PATH_SEPARATOR + key);
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
  public boolean[] getBooleanArray(String key, boolean[] defaultValue) {
    return NetworkTablesJNI.getBooleanArray(path + PATH_SEPARATOR + key, defaultValue);
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
  public Boolean[] getBooleanArray(String key, Boolean[] defaultValue) {
    try {
      return fromNative(getBooleanArray(key));
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
   * @return False if the table key already exists with a different type
   */
  public boolean putNumberArray(String key, double[] value) {
    return NetworkTablesJNI.putDoubleArray(path + PATH_SEPARATOR + key, value);
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
   * @return False if the table key already exists with a different type
   */
  public boolean putNumberArray(String key, Double[] value) {
    return putNumberArray(key, toNative(value));
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
  public double[] getNumberArray(String key) throws TableKeyNotDefinedException {
    return NetworkTablesJNI.getDoubleArray(path + PATH_SEPARATOR + key);
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
  public double[] getNumberArray(String key, double[] defaultValue) {
    return NetworkTablesJNI.getDoubleArray(path + PATH_SEPARATOR + key, defaultValue);
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
  public Double[] getNumberArray(String key, Double[] defaultValue) {
    try {
      return fromNative(getNumberArray(key));
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
   * @return False if the table key already exists with a different type
   */
  public boolean putStringArray(String key, String[] value) {
    return NetworkTablesJNI.putStringArray(path + PATH_SEPARATOR + key, value);
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
  public String[] getStringArray(String key) throws TableKeyNotDefinedException {
    return NetworkTablesJNI.getStringArray(path + PATH_SEPARATOR + key);
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
  public String[] getStringArray(String key, String[] defaultValue) {
    return NetworkTablesJNI.getStringArray(path + PATH_SEPARATOR + key, defaultValue);
  }

  /**
   * Maps the specified key to the specified value in this table. The key can
   * not be null. The value can be retrieved by calling the get method with a
   * key that is equal to the original key.
   *
   * @param key the key name
   * @param value the value to be put
   * @return False if the table key already exists with a different type
   * @throws IllegalArgumentException when the value is not supported by the
   * table
   */
  public boolean putValue(String key, Object value) throws IllegalArgumentException {
    if (value instanceof Boolean)
      return NetworkTablesJNI.putBoolean(path + PATH_SEPARATOR + key, ((Boolean)value).booleanValue());
    else if (value instanceof Double)
      return NetworkTablesJNI.putDouble(path + PATH_SEPARATOR + key, ((Double)value).doubleValue());
    else if (value instanceof String)
      return NetworkTablesJNI.putString(path + PATH_SEPARATOR + key, (String)value);
    else if (value instanceof byte[])
      return NetworkTablesJNI.putRaw(path + PATH_SEPARATOR + key, (byte[])value);
    else if (value instanceof boolean[])
      return NetworkTablesJNI.putBooleanArray(path + PATH_SEPARATOR + key, (boolean[])value);
    else if (value instanceof double[])
      return NetworkTablesJNI.putDoubleArray(path + PATH_SEPARATOR + key, (double[])value);
    else if (value instanceof Boolean[])
      return NetworkTablesJNI.putBooleanArray(path + PATH_SEPARATOR + key, toNative((Boolean[])value));
    else if (value instanceof Double[])
      return NetworkTablesJNI.putDoubleArray(path + PATH_SEPARATOR + key, toNative((Double[])value));
    else if (value instanceof String[])
      return NetworkTablesJNI.putStringArray(path + PATH_SEPARATOR + key, (String[])value);
    else if (value instanceof BooleanArray)
      return NetworkTablesJNI.putBooleanArray(path + PATH_SEPARATOR + key, toNative((Boolean[])((ArrayData)value).getDataArray()));
    else if (value instanceof NumberArray)
      return NetworkTablesJNI.putDoubleArray(path + PATH_SEPARATOR + key, toNative((Double[])((ArrayData)value).getDataArray()));
    else if (value instanceof StringArray)
      return NetworkTablesJNI.putStringArray(path + PATH_SEPARATOR + key, (String[])((ArrayData)value).getDataArray());
    else
      throw new IllegalArgumentException(key);
  }

  public void retrieveValue(String key, Object externalData) throws TableKeyNotDefinedException {
    Object value = getValue(key);
    if (value instanceof boolean[] && externalData instanceof BooleanArray)
      ((ArrayData)externalData).setDataArray(fromNative((boolean[])value));
    else if (value instanceof double[] && externalData instanceof NumberArray)
      ((ArrayData)externalData).setDataArray(fromNative((double[])value));
    else if (value instanceof String[] && externalData instanceof StringArray)
      ((ArrayData)externalData).setDataArray((String[])value);
    else
      throw new TableKeyNotDefinedException(key);
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
    return NetworkTablesJNI.getValue(path + PATH_SEPARATOR + key);
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
    return NetworkTablesJNI.getValue(path + PATH_SEPARATOR + key, defaultValue);
  }

  /** The persistent flag value. */
  public static final int PERSISTENT = 1;

  /**
   * Makes a key's value persistent through program restarts.
   * The key cannot be null.
   *
   * @param key the key name
   */
  public void setPersistent(String key) {
    setFlags(key, PERSISTENT);
  }

  /**
   * Stop making a key's value persistent through program restarts.
   * The key cannot be null.
   *
   * @param key the key name
   */
  public void clearPersistent(String key) {
    clearFlags(key, PERSISTENT);
  }

  /**
   * Returns whether the value is persistent through program restarts.
   * The key cannot be null.
   *
   * @param key the key name
   * @return True if the value is persistent.
   */
  public boolean isPersistent(String key) {
    return (getFlags(key) & PERSISTENT) != 0;
  }

  /**
   * Sets flags on the specified key in this table. The key can
   * not be null.
   *
   * @param key the key name
   * @param flags the flags to set (bitmask)
   */
  public void setFlags(String key, int flags) {
    NetworkTablesJNI.setEntryFlags(path + PATH_SEPARATOR + key, getFlags(key) | flags);
  }

  /**
   * Clears flags on the specified key in this table. The key can
   * not be null.
   *
   * @param key the key name
   * @param flags the flags to clear (bitmask)
   */
  public void clearFlags(String key, int flags) {
    NetworkTablesJNI.setEntryFlags(path + PATH_SEPARATOR + key, getFlags(key) & ~flags);
  }

  /**
   * Returns the flags for the specified key.
   *
   * @param key
   *            the key name
   * @return the flags, or 0 if the key is not defined
   */
  public int getFlags(String key) {
    return NetworkTablesJNI.getEntryFlags(path + PATH_SEPARATOR + key);
  }

  /**
   * Deletes the specified key in this table. The key can
   * not be null.
   *
   * @param key the key name
   */
  public void delete(String key) {
    NetworkTablesJNI.deleteEntry(path + PATH_SEPARATOR + key);
  }

  /**
   * Deletes ALL keys in ALL subtables.  Use with caution!
   */
  public static void globalDeleteAll() {
    NetworkTablesJNI.deleteAllEntries();
  }

  /**
   * Flushes all updated values immediately to the network.
   * Note: This is rate-limited to protect the network from flooding.
   * This is primarily useful for synchronizing network updates with
   * user code.
   */
  public static void flush() {
    NetworkTablesJNI.flush();
  }

  /**
   * Set the periodic update rate.
   *
   * @param interval update interval in seconds (range 0.1 to 1.0)
   */
  public static void setUpdateRate(double interval) {
    NetworkTablesJNI.setUpdateRate(interval);
  }

  /**
   * Saves persistent keys to a file.  The server does this automatically.
   *
   * @param filename file name
   * @throws PersistentException if error saving file
   */
  public static void savePersistent(String filename) throws PersistentException {
    NetworkTablesJNI.savePersistent(filename);
  }

  /**
   * Loads persistent keys from a file.  The server does this automatically.
   *
   * @param filename file name
   * @return List of warnings (errors result in an exception instead)
   * @throws PersistentException if error reading file
   */
  public static String[] loadPersistent(String filename) throws PersistentException {
    return NetworkTablesJNI.loadPersistent(filename);
  }

  /*
   * Deprecated Methods
   */

  /**
   * @deprecated
   * Maps the specified key to the specified value in this table.
   * The key can not be null.
   * The value can be retrieved by calling the get method with a key that is
   * equal to the original key.
   * @param key the key
   * @param value the value
   * @return False if the table key already exists with a different type
   * @throws IllegalArgumentException if key is null
   */
  public boolean putInt(String key, int value) {
    return putNumber(key, value);
  }

  /**
   * @deprecated
   * Returns the value at the specified key.
   * @param key the key
   * @return the value
   * @throws TableKeyNotDefinedException if there is no value mapped to by the
   * key
   * @throws IllegalArgumentException if the value mapped to by the key is not
   * an int
   * @throws IllegalArgumentException if the key is null
   */
  public int getInt(String key) throws TableKeyNotDefinedException {
    return (int)getNumber(key);
  }

  /**
   * @deprecated
   * Returns the value at the specified key.
   * @param key the key
   * @param defaultValue the value returned if the key is undefined
   * @return the value
   * @throws NetworkTableKeyNotDefined if there is no value mapped to by the key
   * @throws IllegalArgumentException if the value mapped to by the key is not
   * an int
   * @throws IllegalArgumentException if the key is null
   */
  public int getInt(String key, int defaultValue) throws TableKeyNotDefinedException {
    try {
      return (int)getNumber(key);
    } catch (NoSuchElementException ex) {
      return defaultValue;
    }
  }

  /**
   * @deprecated
   * Maps the specified key to the specified value in this table.
   * The key can not be null.
   * The value can be retrieved by calling the get method with a key that is
   * equal to the original key.
   * @param key the key
   * @param value the value
   * @return False if the table key already exists with a different type
   * @throws IllegalArgumentException if key is null
   */
  public boolean putDouble(String key, double value) {
    return putNumber(key, value);
  }

  /**
   * @deprecated
   * Returns the value at the specified key.
   * @param key the key
   * @return the value
   * @throws NoSuchEleNetworkTableKeyNotDefinedmentException if there is no
   * value mapped to by the key
   * @throws IllegalArgumentException if the value mapped to by the key is not a
   * double
   * @throws IllegalArgumentException if the key is null
   */
  public double getDouble(String key) throws TableKeyNotDefinedException {
    return getNumber(key);
  }

  /**
   * @deprecated
   * Returns the value at the specified key.
   * @param key the key
   * @param defaultValue the value returned if the key is undefined
   * @return the value
   * @throws NoSuchEleNetworkTableKeyNotDefinedmentException if there is no
   * value mapped to by the key
   * @throws IllegalArgumentException if the value mapped to by the key is not a
   * double
   * @throws IllegalArgumentException if the key is null
   */
  public double getDouble(String key, double defaultValue) {
    return getNumber(key, defaultValue);
  }
}
