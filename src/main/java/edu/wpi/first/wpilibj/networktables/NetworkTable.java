/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2017-2018. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.networktables;

import edu.wpi.first.networktables.ConnectionInfo;
import edu.wpi.first.networktables.ConnectionNotification;
import edu.wpi.first.networktables.EntryInfo;
import edu.wpi.first.networktables.EntryNotification;
import edu.wpi.first.networktables.NetworkTableEntry;
import edu.wpi.first.networktables.NetworkTableInstance;
import edu.wpi.first.networktables.NetworkTableType;
import edu.wpi.first.networktables.NetworkTableValue;
import edu.wpi.first.networktables.NetworkTablesJNI;
import edu.wpi.first.networktables.PersistentException;
import edu.wpi.first.wpilibj.tables.IRemote;
import edu.wpi.first.wpilibj.tables.IRemoteConnectionListener;
import edu.wpi.first.wpilibj.tables.ITable;
import edu.wpi.first.wpilibj.tables.ITableListener;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Objects;
import java.util.Set;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentMap;
import java.util.function.Consumer;

/**
 * A network table that knows its subtable path.
 * @deprecated Use {@link edu.wpi.first.networktables.NetworkTable} instead.
 */
@Deprecated
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
  private static boolean enableDS = true;
  private static boolean running = false;
  private static int port = DEFAULT_PORT;
  private static String persistentFilename = "networktables.ini";

  private synchronized static void checkInit() {
    if (running)
      throw new IllegalStateException(
          "Network tables has already been initialized");
  }

  /**
   * initializes network tables
   * @deprecated Use {@link NetworkTableInstance#startServer()} or
   * {@link NetworkTableInstance#startClient()} instead.
   */
  @Deprecated
  public synchronized static void initialize() {
    if (running)
      shutdown();
    NetworkTableInstance inst = NetworkTableInstance.getDefault();
    if (client) {
      inst.startClient();
      if (enableDS)
        inst.startDSClient(port);
    } else
      inst.startServer(persistentFilename, "", port);
    running = true;
  }

  /**
   * shuts down network tables
   * @deprecated Use {@link NetworkTableInstance#stopServer()} or
   * {@link NetworkTableInstance#stopClient()} instead.
   */
  @Deprecated
  public synchronized static void shutdown() {
    if (!running)
      return;
    NetworkTableInstance inst = NetworkTableInstance.getDefault();
    if (client) {
      inst.stopDSClient();
      inst.stopClient();
    } else
      inst.stopServer();
    running = false;
  }

  /**
   * set that network tables should be a server
   * This must be called before initialize or getTable
   * @deprecated Use {@link NetworkTableInstance#startServer()} instead.
   */
  @Deprecated
  public synchronized static void setServerMode() {
    if (!client)
      return;
    checkInit();
    client = false;
  }

  /**
   * set that network tables should be a client
   * This must be called before initialize or getTable
   * @deprecated Use {@link NetworkTableInstance#startClient()} instead.
   */
  @Deprecated
  public synchronized static void setClientMode() {
    if (client)
      return;
    checkInit();
    client = true;
  }

  /**
   * set the team the robot is configured for (this will set the mdns address that
   * network tables will connect to in client mode)
   * This must be called before initialize or getTable
   * @param team the team number
   * @deprecated Use {@link NetworkTableInstance#setServerTeam(int)} or
   * {@link NetworkTableInstance#startClientTeam(int)} instead.
   */
  @Deprecated
  public synchronized static void setTeam(int team) {
    NetworkTableInstance inst = NetworkTableInstance.getDefault();
    inst.setServerTeam(team, port);
    if (enableDS)
      inst.startDSClient(port);
  }

  /**
   * @param address the address that network tables will connect to in client
   * mode
   * @deprecated Use {@link NetworkTableInstance#setServer(String)} or
   * {@link NetworkTableInstance#startClient(String)} instead.
   */
  @Deprecated
  public synchronized static void setIPAddress(final String address) {
    String[] addresses = new String[1];
    addresses[0] = address;
    setIPAddress(addresses);
  }

  /**
   * @param addresses the adresses that network tables will connect to in
   * client mode (in round robin order)
   * @deprecated Use {@link NetworkTableInstance#setServer(String[])} or
   * {@link NetworkTableInstance#startClient(String[])} instead.
   */
  @Deprecated
  public synchronized static void setIPAddress(final String[] addresses) {
    NetworkTableInstance inst = NetworkTableInstance.getDefault();
    inst.setServer(addresses, port);

    // Stop the DS client if we're explicitly connecting to localhost
    if (addresses.length > 0 &&
        (addresses[0].equals("localhost") || addresses[0].equals("127.0.0.1")))
      inst.stopDSClient();
    else if (enableDS)
      inst.startDSClient(port);
  }

  /**
   * Set the port number that network tables will connect to in client
   * mode or listen to in server mode.
   * @param aport the port number
   * @deprecated Use the appropriate parameters to
   * {@link NetworkTableInstance#setServer(String, int)},
   * {@link NetworkTableInstance#startClient(String, int)},
   * {@link NetworkTableInstance#startServer(String, String, int)}, and
   * {@link NetworkTableInstance#startDSClient(int)} instead.
   */
  @Deprecated
  public synchronized static void setPort(int aport) {
    if (port == aport)
      return;
    checkInit();
    port = aport;
  }

  /**
   * Enable requesting the server address from the Driver Station.
   * @param enabled whether to enable the connection to the local DS
   * @deprecated Use {@link NetworkTableInstance#startDSClient()} and
   * {@link NetworkTableInstance#stopDSClient()} instead.
   */
  @Deprecated
  public synchronized static void setDSClientEnabled(boolean enabled) {
    NetworkTableInstance inst = NetworkTableInstance.getDefault();
    enableDS = enabled;
    if (enableDS)
      inst.startDSClient(port);
    else
      inst.stopDSClient();
  }

  /**
   * Sets the persistent filename.
   * @param filename the filename that the network tables server uses for
   * automatic loading and saving of persistent values
   * @deprecated Use the appropriate parameter to
   * {@link NetworkTableInstance#startServer()} instead.
   */
  @Deprecated
  public synchronized static void setPersistentFilename(final String filename) {
    if (persistentFilename.equals(filename))
      return;
    checkInit();
    persistentFilename = filename;
  }

  /**
   * Sets the network identity.
   * This is provided in the connection info on the remote end.
   * @param name identity
   * @deprecated Use {@link NetworkTableInstance#setNetworkIdentity(String)}
   * instead.
   */
  @Deprecated
  public static void setNetworkIdentity(String name) {
    NetworkTableInstance.getDefault().setNetworkIdentity(name);
  }

  public static boolean[] toNative(Boolean[] arr) {
    boolean[] out = new boolean[arr.length];
    for (int i = 0; i < arr.length; i++)
      out[i] = arr[i];
    return out;
  }

  public static double[] toNative(Number[] arr) {
    double[] out = new double[arr.length];
    for (int i = 0; i < arr.length; i++)
      out[i] = arr[i].doubleValue();
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
   * table will be created.<br>
   * This will automatically initialize network tables if it has not been
   * already
   *
   * @deprecated Use {@link NetworkTableInstance#getTable(String)} instead.
   *
   * @param key   the key name
   * @return the network table requested
   */
  @Deprecated
  public synchronized static NetworkTable getTable(String key) {
    if (!running)
      initialize();
    String theKey;
    if (key.isEmpty() || key.equals("/")) {
      theKey = "";
    } else if (key.charAt(0) == NetworkTable.PATH_SEPARATOR) {
      theKey = key;
    } else {
      theKey = NetworkTable.PATH_SEPARATOR + key;
    }
    return new NetworkTable(NetworkTableInstance.getDefault(), theKey);
  }

  private final String path;
  private final String pathWithSep;
  private final NetworkTableInstance inst;

  NetworkTable(NetworkTableInstance inst, String path) {
    this.path = path;
    this.pathWithSep = path + PATH_SEPARATOR;
    this.inst = inst;
  }

  public String toString() { return "NetworkTable: " + path; }

  private final ConcurrentMap<String, NetworkTableEntry> entries = new ConcurrentHashMap<String, NetworkTableEntry>();

  /**
   * Gets the entry for a subkey.
   * @param key the key name
   * @return Network table entry.
   */
  private NetworkTableEntry getEntry(String key) {
    NetworkTableEntry entry = entries.get(key);
    if (entry == null) {
      entry = inst.getEntry(pathWithSep + key);
      entries.putIfAbsent(key, entry);
    }
    return entry;
  }

  /**
   * Gets the current network connections.
   * @return An array of connection information.
   * @deprecated Use {@link NetworkTableInstance#getConnections()} instead.
   */
  @Deprecated
  public static ConnectionInfo[] connections() {
    return NetworkTableInstance.getDefault().getConnections();
  }

  /**
   * Determine whether or not a network connection is active.
   * @return True if connected, false if not connected.
   * @deprecated Use {@link NetworkTableInstance#isConnected()} instead.
   */
  @Deprecated
  public boolean isConnected() {
    return inst.isConnected();
  }

  /**
   * Determine whether NetworkTables is operating as a server or as a client.
   * @return True if operating as a server, false otherwise.
   * @deprecated Use {@link NetworkTableInstance#getNetworkMode()} instead.
   */
  @Deprecated
  public boolean isServer() {
    return (inst.getNetworkMode() & NetworkTableInstance.kNetModeServer) != 0;
  }

  /* Backwards compatibility shims for IRemoteConnectionListener */
  private static class ConnectionListenerAdapter implements Consumer<ConnectionNotification> {
    public int uid;
    private final IRemote targetSource;
    private final IRemoteConnectionListener targetListener;

    public ConnectionListenerAdapter(IRemote targetSource, IRemoteConnectionListener targetListener) {
      this.targetSource = targetSource;
      this.targetListener = targetListener;
    }

    @Override
    public void accept(ConnectionNotification event) {
      if (event.connected)
        targetListener.connectedEx(targetSource, event.conn);
      else
        targetListener.disconnectedEx(targetSource, event.conn);
    }
  }

  private static final HashMap<IRemoteConnectionListener,ConnectionListenerAdapter> globalConnectionListenerMap = new HashMap<IRemoteConnectionListener,ConnectionListenerAdapter>();

  private static IRemote staticRemote = new IRemote() {
    public void addConnectionListener(IRemoteConnectionListener listener, boolean immediateNotify) {
      NetworkTable.addGlobalConnectionListener(listener, immediateNotify);
    }
    public void removeConnectionListener(IRemoteConnectionListener listener) {
      NetworkTable.removeGlobalConnectionListener(listener);
    }
    public boolean isConnected() {
      ConnectionInfo[] conns = NetworkTableInstance.getDefault().getConnections();
      return conns.length > 0;
    }
    public boolean isServer() {
      return (NetworkTableInstance.getDefault().getNetworkMode() & NetworkTableInstance.kNetModeServer) != 0;
    }
  };

  private final HashMap<IRemoteConnectionListener,ConnectionListenerAdapter> connectionListenerMap = new HashMap<IRemoteConnectionListener,ConnectionListenerAdapter>();

  /**
   * Add a connection listener.
   * @param listener connection listener
   * @param immediateNotify call listener immediately for all existing connections
   * @deprecated Use {@link NetworkTableInstance#addConnectionListener(Consumer, boolean)} instead.
   */
  @Deprecated
  public static synchronized void addGlobalConnectionListener(IRemoteConnectionListener listener, boolean immediateNotify) {
    ConnectionListenerAdapter adapter = new ConnectionListenerAdapter(staticRemote, listener);
    if (globalConnectionListenerMap.putIfAbsent(listener, adapter) != null) {
      throw new IllegalStateException("Cannot add the same listener twice");
    }
    adapter.uid = NetworkTableInstance.getDefault().addConnectionListener(adapter, immediateNotify);
  }

  /**
   * Remove a connection listener.
   * @param listener connection listener
   * @deprecated Use {@link NetworkTableInstance#removeConnectionListener(int)} instead.
   */
  @Deprecated
  public static synchronized void removeGlobalConnectionListener(IRemoteConnectionListener listener) {
    ConnectionListenerAdapter adapter = globalConnectionListenerMap.remove(listener);
    if (adapter != null) {
      NetworkTableInstance.getDefault().removeConnectionListener(adapter.uid);
    }
  }

  /**
   * Add a connection listener.
   * @param listener connection listener
   * @param immediateNotify call listener immediately for all existing connections
   * @deprecated Use {@link NetworkTableInstance#addConnectionListener(Consumer, boolean)} instead.
   */
  @Deprecated
  public synchronized void addConnectionListener(IRemoteConnectionListener listener,
                                                 boolean immediateNotify) {
    ConnectionListenerAdapter adapter = new ConnectionListenerAdapter(this, listener);
    if (connectionListenerMap.putIfAbsent(listener, adapter) != null) {
      throw new IllegalStateException("Cannot add the same listener twice");
    }
    adapter.uid = inst.addConnectionListener(adapter, immediateNotify);
  }

  /**
   * Remove a connection listener.
   * @param listener connection listener
   * @deprecated Use {@link NetworkTableInstance#removeConnectionListener(int)} instead.
   */
  @Deprecated
  public synchronized void removeConnectionListener(IRemoteConnectionListener listener) {
    ConnectionListenerAdapter adapter = connectionListenerMap.get(listener);
    if (adapter != null && connectionListenerMap.remove(listener, adapter)) {
      inst.removeConnectionListener(adapter.uid);
    }
  }

  /**
   * {@inheritDoc}
   * @deprecated Use {@link edu.wpi.first.networktables.NetworkTable#addEntryListener(TableEntryListener, int)} instead
   * (with flags value of NOTIFY_NEW | NOTIFY_UPDATE).
   */
  @Override
  @Deprecated
  public void addTableListener(ITableListener listener) {
    addTableListenerEx(listener, NOTIFY_NEW | NOTIFY_UPDATE);
  }

  /**
   * {@inheritDoc}
   * @deprecated Use {@link edu.wpi.first.networktables.NetworkTable#addEntryListener(TableEntryListener, int)} instead
   * (with flags value of NOTIFY_NEW | NOTIFY_UPDATE | NOTIFY_IMMEDIATE).
   */
  @Override
  @Deprecated
  public void addTableListener(ITableListener listener,
                               boolean immediateNotify) {
    int flags = NOTIFY_NEW | NOTIFY_UPDATE;
    if (immediateNotify)
      flags |= NOTIFY_IMMEDIATE;
    addTableListenerEx(listener, flags);
  }

  /* Base class for listeners; stores uid to implement remove functions */
  private static class ListenerBase {
    public int uid;
  }

  private class OldTableListenerAdapter extends ListenerBase implements Consumer<EntryNotification> {
    private final int prefixLen;
    private final ITable targetSource;
    private final ITableListener targetListener;

    public OldTableListenerAdapter(int prefixLen, ITable targetSource, ITableListener targetListener) {
      this.prefixLen = prefixLen;
      this.targetSource = targetSource;
      this.targetListener = targetListener;
    }

    @Override
    public void accept(EntryNotification event) {
      String relativeKey = event.name.substring(prefixLen);
      if (relativeKey.indexOf(PATH_SEPARATOR) != -1)
        return;
      targetListener.valueChangedEx(targetSource, relativeKey, event.value.getValue(), event.flags);
    }
  }

  private final HashMap<ITableListener,List<ListenerBase>> oldListenerMap = new HashMap<ITableListener,List<ListenerBase>>();

  /**
   * {@inheritDoc}
   * @deprecated Use {@link edu.wpi.first.networktables.NetworkTable#addEntryListener(TableEntryListener, int)} instead.
   */
  @Override
  @Deprecated
  public synchronized void addTableListenerEx(ITableListener listener,
                                              int flags) {
    List<ListenerBase> adapters = oldListenerMap.get(listener);
    if (adapters == null) {
      adapters = new ArrayList<ListenerBase>();
      oldListenerMap.put(listener, adapters);
    }
    OldTableListenerAdapter adapter =
        new OldTableListenerAdapter(path.length() + 1, this, listener);
    adapter.uid = inst.addEntryListener(pathWithSep, adapter, flags);
    adapters.add(adapter);
  }

  /**
   * {@inheritDoc}
   * @deprecated Use {@link edu.wpi.first.networktables.NetworkTable#addEntryListener(String, TableEntryListener, int)}
   * or {@link NetworkTableEntry#addListener(Consumer, int)} instead.
   */
  @Override
  @Deprecated
  public void addTableListener(String key, ITableListener listener,
                               boolean immediateNotify) {
    int flags = NOTIFY_NEW | NOTIFY_UPDATE;
    if (immediateNotify)
      flags |= NOTIFY_IMMEDIATE;
    addTableListenerEx(key, listener, flags);
  }

  private class OldKeyListenerAdapter extends ListenerBase implements Consumer<EntryNotification> {
    private final String relativeKey;
    private final ITable targetSource;
    private final ITableListener targetListener;

    public OldKeyListenerAdapter(String relativeKey, ITable targetSource, ITableListener targetListener) {
      this.relativeKey = relativeKey;
      this.targetSource = targetSource;
      this.targetListener = targetListener;
    }

    @Override
    public void accept(EntryNotification event) {
      targetListener.valueChangedEx(targetSource, relativeKey, event.value.getValue(), event.flags);
    }
  }

  /**
   * {@inheritDoc}
   * @deprecated Use {@link edu.wpi.first.networktables.NetworkTable#addEntryListener(String, TableEntryListener, int)}
   * or {@link NetworkTableEntry#addListener(Consumer, int)} instead.
   */
  @Override
  @Deprecated
  public synchronized void addTableListenerEx(String key,
                                              ITableListener listener,
                                              int flags) {
    List<ListenerBase> adapters = oldListenerMap.get(listener);
    if (adapters == null) {
      adapters = new ArrayList<ListenerBase>();
      oldListenerMap.put(listener, adapters);
    }
    OldKeyListenerAdapter adapter = new OldKeyListenerAdapter(key, this, listener);
    adapter.uid = inst.addEntryListener(getEntry(key), adapter, flags);
    adapters.add(adapter);
  }

  /**
   * {@inheritDoc}
   * @deprecated Use {@link edu.wpi.first.networktables.NetworkTable#addSubTableListener(TableListener, boolean)}
   * instead.
   */
  @Override
  @Deprecated
  public void addSubTableListener(final ITableListener listener) {
    addSubTableListener(listener, false);
  }

  private class OldSubListenerAdapter extends ListenerBase implements Consumer<EntryNotification> {
    private final int prefixLen;
    private final ITable targetSource;
    private final ITableListener targetListener;
    private final Set<String> notifiedTables = new HashSet<String>();

    public OldSubListenerAdapter(int prefixLen, ITable targetSource, ITableListener targetListener) {
      this.prefixLen = prefixLen;
      this.targetSource = targetSource;
      this.targetListener = targetListener;
    }

    @Override
    public void accept(EntryNotification event) {
      String relativeKey = event.name.substring(prefixLen);
      int endSubTable = relativeKey.indexOf(PATH_SEPARATOR);
      if (endSubTable == -1)
        return;
      String subTableKey = relativeKey.substring(0, endSubTable);
      if (notifiedTables.contains(subTableKey))
        return;
      notifiedTables.add(subTableKey);
      targetListener.valueChangedEx(targetSource, subTableKey, targetSource.getSubTable(subTableKey), event.flags);
    }
  }

  /**
   * {@inheritDoc}
   * @deprecated Use {@link edu.wpi.first.networktables.NetworkTable#addSubTableListener(TableListener, boolean)}
   * instead.
   */
  @Override
  @Deprecated
  public synchronized void addSubTableListener(final ITableListener listener,
                                               boolean localNotify) {
    List<ListenerBase> adapters = oldListenerMap.get(listener);
    if (adapters == null) {
      adapters = new ArrayList<ListenerBase>();
      oldListenerMap.put(listener, adapters);
    }
    OldSubListenerAdapter adapter =
        new OldSubListenerAdapter(path.length() + 1, this, listener);
    int flags = NOTIFY_NEW | NOTIFY_IMMEDIATE;
    if (localNotify)
      flags |= NOTIFY_LOCAL;
    adapter.uid = inst.addEntryListener(pathWithSep, adapter, flags);
    adapters.add(adapter);
  }

  /**
   * {@inheritDoc}
   * @deprecated Use {@link edu.wpi.first.networktables.NetworkTable#removeTableListener(int)} instead.
   */
  @Override
  @Deprecated
  public synchronized void removeTableListener(ITableListener listener) {
    List<ListenerBase> adapters = oldListenerMap.remove(listener);
    if (adapters != null) {
      for (ListenerBase adapter : adapters)
        inst.removeEntryListener(adapter.uid);
    }
  }

  /**
   * {@inheritDoc}
   */
  @Override
  public ITable getSubTable(String key) {
    return new NetworkTable(inst, pathWithSep + key);
  }

  /**
   * {@inheritDoc}
   */
  @Override
  public boolean containsKey(String key) {
    return getEntry(key).exists();
  }

  public boolean containsSubTable(String key) {
    int[] handles = NetworkTablesJNI.getEntries(inst.getHandle(), pathWithSep + key + PATH_SEPARATOR, 0);
    return handles.length != 0;
  }

  /**
   * @param types bitmask of types; 0 is treated as a "don't care".
   * @return keys currently in the table
   */
  public Set<String> getKeys(int types) {
    Set<String> keys = new HashSet<String>();
    int prefixLen = path.length() + 1;
    for (EntryInfo info : inst.getEntryInfo(pathWithSep, types)) {
      String relativeKey = info.name.substring(prefixLen);
      if (relativeKey.indexOf(PATH_SEPARATOR) != -1)
        continue;
      keys.add(relativeKey);
      // populate entries as we go
      if (entries.get(relativeKey) == null) {
        entries.putIfAbsent(relativeKey, new NetworkTableEntry(inst, info.entry));
      }
    }
    return keys;
  }

  /**
   * {@inheritDoc}
   */
  @Override
  public Set<String> getKeys() {
    return getKeys(0);
  }

  /**
   * {@inheritDoc}
   */
  @Override
  public Set<String> getSubTables() {
    Set<String> keys = new HashSet<String>();
    int prefixLen = path.length() + 1;
    for (EntryInfo info : inst.getEntryInfo(pathWithSep, 0)) {
      String relativeKey = info.name.substring(prefixLen);
      int endSubTable = relativeKey.indexOf(PATH_SEPARATOR);
      if (endSubTable == -1)
        continue;
      keys.add(relativeKey.substring(0, endSubTable));
    }
    return keys;
  }

  /**
   * {@inheritDoc}
   */
  @Override
  public boolean putNumber(String key, double value) {
    return getEntry(key).setNumber(value);
  }

  /**
   * {@inheritDoc}
   */
  public boolean setDefaultNumber(String key, double defaultValue) {
    return getEntry(key).setDefaultDouble(defaultValue);
  }

  /**
   * {@inheritDoc}
   */
  @Override
  public double getNumber(String key, double defaultValue) {
    return getEntry(key).getDouble(defaultValue);
  }

  /**
   * {@inheritDoc}
   */
  @Override
  public boolean putString(String key, String value) {
    return getEntry(key).setString(value);
  }

  /**
   * {@inheritDoc}
   */
  public boolean setDefaultString(String key, String defaultValue) {
    return getEntry(key).setDefaultString(defaultValue);
  }

  /**
   * {@inheritDoc}
   */
  @Override
  public String getString(String key, String defaultValue) {
    return getEntry(key).getString(defaultValue);
  }

  /**
   * {@inheritDoc}
   */
  @Override
  public boolean putBoolean(String key, boolean value) {
    return getEntry(key).setBoolean(value);
  }

  /**
   * {@inheritDoc}
   */
  public boolean setDefaultBoolean(String key, boolean defaultValue) {
    return getEntry(key).setDefaultBoolean(defaultValue);
  }

  /**
   * {@inheritDoc}
   */
  @Override
  public boolean getBoolean(String key, boolean defaultValue) {
    return getEntry(key).getBoolean(defaultValue);
  }

  /**
   * {@inheritDoc}
   */
  @Override
  public boolean putBooleanArray(String key, boolean[] value) {
    return getEntry(key).setBooleanArray(value);
  }

  /**
   * {@inheritDoc}
   */
  @Override
  public boolean putBooleanArray(String key, Boolean[] value) {
    return getEntry(key).setBooleanArray(value);
  }

  /**
   * {@inheritDoc}
   */
  public boolean setDefaultBooleanArray(String key, boolean[] defaultValue) {
    return getEntry(key).setDefaultBooleanArray(defaultValue);
  }

  /**
   * {@inheritDoc}
   */
  public boolean setDefaultBooleanArray(String key, Boolean[] defaultValue) {
    return getEntry(key).setDefaultBooleanArray(defaultValue);
  }

  /**
   * {@inheritDoc}
   */
  @Override
  public boolean[] getBooleanArray(String key, boolean[] defaultValue) {
    return getEntry(key).getBooleanArray(defaultValue);
  }

  /**
   * {@inheritDoc}
   */
  @Override
  public Boolean[] getBooleanArray(String key, Boolean[] defaultValue) {
    return getEntry(key).getBooleanArray(defaultValue);
  }

  /**
   * {@inheritDoc}
   */
  @Override
  public boolean putNumberArray(String key, double[] value) {
    return getEntry(key).setDoubleArray(value);
  }

  /**
   * {@inheritDoc}
   */
  @Override
  public boolean putNumberArray(String key, Double[] value) {
    return getEntry(key).setNumberArray(value);
  }

  /**
   * {@inheritDoc}
   */
  public boolean setDefaultNumberArray(String key, double[] defaultValue) {
    return getEntry(key).setDefaultDoubleArray(defaultValue);
  }

  /**
   * {@inheritDoc}
   */
  public boolean setDefaultNumberArray(String key, Double[] defaultValue) {
    return getEntry(key).setDefaultNumberArray(defaultValue);
  }

  /**
   * {@inheritDoc}
   */
  @Override
  public double[] getNumberArray(String key, double[] defaultValue) {
    return getEntry(key).getDoubleArray(defaultValue);
  }

  /**
   * {@inheritDoc}
   */
  @Override
  public Double[] getNumberArray(String key, Double[] defaultValue) {
    return getEntry(key).getDoubleArray(defaultValue);
  }

  /**
   * {@inheritDoc}
   */
  @Override
  public boolean putStringArray(String key, String[] value) {
    return getEntry(key).setStringArray(value);
  }

  /**
   * {@inheritDoc}
   */
  public boolean setDefaultStringArray(String key, String[] defaultValue) {
    return getEntry(key).setDefaultStringArray(defaultValue);
  }

  /**
   * {@inheritDoc}
   */
  @Override
  public String[] getStringArray(String key, String[] defaultValue) {
    return getEntry(key).getStringArray(defaultValue);
  }

  /**
   * {@inheritDoc}
   */
  @Override
  public boolean putRaw(String key, byte[] value) {
    return getEntry(key).setRaw(value);
  }

  /**
   * {@inheritDoc}
   */
  public boolean setDefaultRaw(String key, byte[] defaultValue) {
    return getEntry(key).setDefaultRaw(defaultValue);
  }

  /**
   * {@inheritDoc}
   */
  @Override
  public boolean putRaw(String key, ByteBuffer value, int len) {
    return getEntry(key).setRaw(value, len);
  }

  /**
   * {@inheritDoc}
   */
  @Override
  public byte[] getRaw(String key, byte[] defaultValue) {
    return getEntry(key).getRaw(defaultValue);
  }

  /**
   * Put a value in the table
   * @param key the key to be assigned to
   * @param value the value that will be assigned
   * @return False if the table key already exists with a different type
   */
  public boolean putValue(String key, NetworkTableValue value) {
    return getEntry(key).setValue(value);
  }

  /**
   * Sets the current value in the table if it does not exist.
   * @param key the key
   * @param defaultValue the default value to set if key doens't exist.
   * @return False if the table key exists with a different type
   */
  public boolean setDefaultValue(String key, NetworkTableValue defaultValue) {
    return getEntry(key).setDefaultValue(defaultValue);
  }

  /**
   * Gets the value associated with a key as a NetworkTableValue object.
   * @param key the key of the value to look up
   * @return the value associated with the given key
   */
  public NetworkTableValue getValue(String key) {
    return getEntry(key).getValue();
  }

  /**
   * {@inheritDoc}
   * @deprecated Use {@link edu.wpi.first.networktables.NetworkTableEntry#setValue(Object)}
   * instead, e.g. `NetworkTable.getEntry(key).setValue(NetworkTableEntry.makeBoolean(false));`
   * or `NetworkTable.getEntry(key).setValue(new Boolean(false));`
   */
  @Deprecated
  public boolean putValue(String key, Object value) throws IllegalArgumentException {
    if (value instanceof Boolean)
      return putBoolean(key, ((Boolean)value).booleanValue());
    else if (value instanceof Number)
      return putDouble(key, ((Number)value).doubleValue());
    else if (value instanceof String)
      return putString(key, (String)value);
    else if (value instanceof byte[])
      return putRaw(key, (byte[])value);
    else if (value instanceof boolean[])
      return putBooleanArray(key, (boolean[])value);
    else if (value instanceof double[])
      return putNumberArray(key, (double[])value);
    else if (value instanceof Boolean[])
      return putBooleanArray(key, toNative((Boolean[])value));
    else if (value instanceof Number[])
      return putNumberArray(key, toNative((Number[])value));
    else if (value instanceof String[])
      return putStringArray(key, (String[])value);
    else if (value instanceof NetworkTableValue)
      return getEntry(key).setValue((NetworkTableValue)value);
    else
      throw new IllegalArgumentException("Value of type " + value.getClass().getName() + " cannot be put into a table");
  }

  /**
   * {@inheritDoc}
   * @deprecated Use {@link edu.wpi.first.networktables.NetworkTableEntry#getValue()}
   * instead, e.g. `NetworkTable.getEntry(key).getValue();`
   */
  @Override
  @Deprecated
  public Object getValue(String key, Object defaultValue) {
    NetworkTableValue value = getValue(key);
    if (value.getType() == NetworkTableType.kUnassigned) {
      return defaultValue;
    }
    return value.getValue();
  }

  /** The persistent flag value. */
  public static final int PERSISTENT = 1;

  /**
   * {@inheritDoc}
   */
  @Override
  public void setPersistent(String key) {
    getEntry(key).setPersistent();
  }

  /**
   * {@inheritDoc}
   */
  @Override
  public void clearPersistent(String key) {
    getEntry(key).clearPersistent();
  }

  /**
   * {@inheritDoc}
   */
  @Override
  public boolean isPersistent(String key) {
    return getEntry(key).isPersistent();
  }

  /**
   * {@inheritDoc}
   */
  @Override
  public void setFlags(String key, int flags) {
    getEntry(key).setFlags(flags);
  }

  /**
   * {@inheritDoc}
   */
  @Override
  public void clearFlags(String key, int flags) {
    getEntry(key).clearFlags(flags);
  }

  /**
   * {@inheritDoc}
   */
  @Override
  public int getFlags(String key) {
    return getEntry(key).getFlags();
  }

  /**
   * {@inheritDoc}
   */
  @Override
  public void delete(String key) {
    getEntry(key).delete();
  }

  /**
   * Deletes ALL keys in ALL subtables.  Use with caution!
   * @deprecated Use {@link NetworkTableInstance#deleteAllEntries()} instead.
   */
  @Deprecated
  public static void globalDeleteAll() {
    NetworkTableInstance.getDefault().deleteAllEntries();
  }

  /**
   * Flushes all updated values immediately to the network.
   * Note: This is rate-limited to protect the network from flooding.
   * This is primarily useful for synchronizing network updates with
   * user code.
   * @deprecated Use {@link NetworkTableInstance#flush()} instead.
   */
  @Deprecated
  public static void flush() {
    NetworkTableInstance.getDefault().flush();
  }

  /**
   * Set the periodic update rate.
   *
   * @param interval update interval in seconds (range 0.01 to 1.0)
   * @deprecated Use {@link NetworkTableInstance#setUpdateRate(double)}
   * instead.
   */
  @Deprecated
  public static void setUpdateRate(double interval) {
    NetworkTableInstance.getDefault().setUpdateRate(interval);
  }

  /**
   * Saves persistent keys to a file.  The server does this automatically.
   *
   * @param filename file name
   * @throws PersistentException if error saving file
   * @deprecated Use {@link NetworkTableInstance#savePersistent(String)}
   * instead.
   */
  @Deprecated
  public static void savePersistent(String filename) throws PersistentException {
    NetworkTableInstance.getDefault().savePersistent(filename);
  }

  /**
   * Loads persistent keys from a file.  The server does this automatically.
   *
   * @param filename file name
   * @return List of warnings (errors result in an exception instead)
   * @throws PersistentException if error reading file
   * @deprecated Use {@link NetworkTableInstance#loadPersistent(String)}
   * instead.
   */
  @Deprecated
  public static String[] loadPersistent(String filename) throws PersistentException {
    return NetworkTableInstance.getDefault().loadPersistent(filename);
  }

  /*
   * Deprecated Methods
   */

  /**
   * {@inheritDoc}
   * @deprecated Use {@link #putNumber(String, double)} instead.
   */
  @Override
  @Deprecated
  public boolean putDouble(String key, double value) {
    return putNumber(key, value);
  }

  /**
   * {@inheritDoc}
   * @deprecated Use {@link #getNumber(String, double)} instead.
   */
  @Override
  @Deprecated
  public double getDouble(String key, double defaultValue) {
    return getNumber(key, defaultValue);
  }

  /**
   * {@inheritDoc}
   */
  @Override
  public String getPath() {
    return path;
  }

  @Override
  public boolean equals(Object o) {
    if (o == this) {
      return true;
    }
    if (!(o instanceof NetworkTable)) {
      return false;
    }
    NetworkTable other = (NetworkTable) o;
    return inst.equals(other.inst) && path.equals(other.path);
  }

  @Override
  public int hashCode() {
    return Objects.hash(inst, path);
  }
}
