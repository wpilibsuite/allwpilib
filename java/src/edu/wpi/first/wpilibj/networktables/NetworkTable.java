package edu.wpi.first.wpilibj.networktables;

import edu.wpi.first.wpilibj.tables.*;
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
  private static String ipAddress = null;
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
    checkInit();
    if (client)
      NetworkTablesJNI.startClient(ipAddress, port);
    else
      NetworkTablesJNI.startServer(persistentFilename, ipAddress, port);
    running = true;
  }

  /**
   * set that network tables should be a server
   * This must be called before initalize or getTable
   */
  public synchronized static void setServerMode() {
    checkInit();
    client = false;
  }

  /**
   * set that network tables should be a client
   * This must be called before initalize or getTable
   */
  public synchronized static void setClientMode() {
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
    checkInit();
    ipAddress = address;
  }

  /**
   * @param port the port number that network tables will connect to in client
   * mode or listen to in server mode
   */
  public synchronized static void setPort(int aport) {
    checkInit();
    port = aport;
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
    addTableListener(listener, false);
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
                                            boolean immediateNotify) {
    List<ListenerBase> adapters = listenerMap.get(listener);
    if (adapters == null) {
      adapters = new ArrayList<ListenerBase>();
      listenerMap.put(listener, adapters);
    }
    TableListenerAdapter adapter =
        new TableListenerAdapter(path.length() + 1, this, listener);
    adapter.uid = NetworkTablesJNI.addEntryListener(path + PATH_SEPARATOR, adapter, immediateNotify);
    adapters.add(adapter);
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
                                            boolean immediateNotify) {
    List<ListenerBase> adapters = listenerMap.get(listener);
    if (adapters == null) {
      adapters = new ArrayList<ListenerBase>();
      listenerMap.put(listener, adapters);
    }
    String fullKey = path + PATH_SEPARATOR + key;
    KeyListenerAdapter adapter =
        new KeyListenerAdapter(key, fullKey, this, listener);
    adapter.uid = NetworkTablesJNI.addEntryListener(fullKey, adapter, immediateNotify);
    adapters.add(adapter);
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

  public synchronized void addSubTableListener(final ITableListener listener) {
    List<ListenerBase> adapters = listenerMap.get(listener);
    if (adapters == null) {
      adapters = new ArrayList<ListenerBase>();
      listenerMap.put(listener, adapters);
    }
    SubListenerAdapter adapter =
        new SubListenerAdapter(path.length() + 1, this, listener);
    adapter.uid = NetworkTablesJNI.addEntryListener(path + PATH_SEPARATOR, adapter, true);
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
    String subtablePrefix = path + key + PATH_SEPARATOR;
    //List keys = node.getEntryStore().keys();
    //for (int i = 0; i < keys.size(); ++i) {
    //  if (((String)keys.get(i)).startsWith(subtablePrefix))
    //    return true;
    //}
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
    NetworkTablesJNI.putDouble(path + PATH_SEPARATOR + key, value);
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
   */
  public void putString(String key, String value) {
    NetworkTablesJNI.putString(path + PATH_SEPARATOR + key, value);
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
   */
  public void putBoolean(String key, boolean value) {
    NetworkTablesJNI.putBoolean(path + PATH_SEPARATOR + key, value);
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
   */
  public void putBooleanArray(String key, boolean[] value) {
    NetworkTablesJNI.putBooleanArray(path + PATH_SEPARATOR + key, value);
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
  public void putBooleanArray(String key, Boolean[] value) {
    putBooleanArray(key, toNative(value));
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
   */
  public void putNumberArray(String key, double[] value) {
    NetworkTablesJNI.putDoubleArray(path + PATH_SEPARATOR + key, value);
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
  public void putNumberArray(String key, Double[] value) {
    putNumberArray(key, toNative(value));
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
   */
  public void putStringArray(String key, String[] value) {
    NetworkTablesJNI.putStringArray(path + PATH_SEPARATOR + key, value);
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
   */
  public void putValue(String key, Object value) {
    if (value instanceof Boolean)
      NetworkTablesJNI.putBoolean(path + PATH_SEPARATOR + key, ((Boolean)value).booleanValue());
    else if (value instanceof Double)
      NetworkTablesJNI.putDouble(path + PATH_SEPARATOR + key, ((Double)value).doubleValue());
    else if (value instanceof String)
      NetworkTablesJNI.putString(path + PATH_SEPARATOR + key, (String)value);
    else if (value instanceof byte[])
      NetworkTablesJNI.putRaw(path + PATH_SEPARATOR + key, (byte[])value);
    else if (value instanceof boolean[])
      NetworkTablesJNI.putBooleanArray(path + PATH_SEPARATOR + key, (boolean[])value);
    else if (value instanceof double[])
      NetworkTablesJNI.putDoubleArray(path + PATH_SEPARATOR + key, (double[])value);
    else if (value instanceof Boolean[])
      NetworkTablesJNI.putBooleanArray(path + PATH_SEPARATOR + key, toNative((Boolean[])value));
    else if (value instanceof Double[])
      NetworkTablesJNI.putDoubleArray(path + PATH_SEPARATOR + key, toNative((Double[])value));
    else if (value instanceof String[])
      NetworkTablesJNI.putStringArray(path + PATH_SEPARATOR + key, (String[])value);
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
   * Sets flags on the specified key in this table. The key can
   * not be null.
   *
   * @param key the key name
   * @param flags the flags to set
   */
  public void setFlags(String key, int flags) {
    NetworkTablesJNI.setEntryFlags(path + PATH_SEPARATOR + key, flags);
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
  * @throws IllegalArgumentException if key is null
  */
  public void putInt(String key, int value) { putNumber(key, value); }

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
