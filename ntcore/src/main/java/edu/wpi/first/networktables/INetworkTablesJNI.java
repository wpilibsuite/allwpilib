package edu.wpi.first.networktables;

import java.nio.ByteBuffer;

public interface INetworkTablesJNI {
  int getDefaultInstance();
  int createInstance();
  void destroyInstance(int inst);
  int getInstanceFromHandle(int handle);

  int getEntry(int inst, String key);
  int[] getEntries(int inst, String prefix, int types);
  String getEntryName(int entry);
  long getEntryLastChange(int entry);

  int getType(int entry);

  boolean setBoolean(int entry, long time, boolean value, boolean force);
  boolean setDouble(int entry, long time, double value, boolean force);
  boolean setString(int entry, long time, String value, boolean force);
  boolean setRaw(int entry, long time, byte[] value, boolean force);
  boolean setRaw(int entry, long time, ByteBuffer value, int len, boolean force);
  boolean setBooleanArray(int entry, long time, boolean[] value, boolean force);
  boolean setDoubleArray(int entry, long time, double[] value, boolean force);
  boolean setStringArray(int entry, long time, String[] value, boolean force);

  NetworkTableValue getValue(int entry);

  boolean getBoolean(int entry, boolean defaultValue);
  double getDouble(int entry, double defaultValue);
  String getString(int entry, String defaultValue);
  byte[] getRaw(int entry, byte[] defaultValue);
  boolean[] getBooleanArray(int entry, boolean[] defaultValue);
  double[] getDoubleArray(int entry, double[] defaultValue);
  String[] getStringArray(int entry, String[] defaultValue);
  boolean setDefaultBoolean(int entry, long time, boolean defaultValue);

  boolean setDefaultDouble(int entry, long time, double defaultValue);
  boolean setDefaultString(int entry, long time, String defaultValue);
  boolean setDefaultRaw(int entry, long time, byte[] defaultValue);
  boolean setDefaultBooleanArray(int entry, long time, boolean[] defaultValue);
  boolean setDefaultDoubleArray(int entry, long time, double[] defaultValue);
  boolean setDefaultStringArray(int entry, long time, String[] defaultValue);

  void setEntryFlags(int entry, int flags);
  int getEntryFlags(int entry);

  void deleteEntry(int entry);

  void deleteAllEntries(int inst);

  EntryInfo getEntryInfoHandle(NetworkTableInstance inst, int entry);
  EntryInfo[] getEntryInfo(NetworkTableInstance instObject, int inst, String prefix, int types);

  int createEntryListenerPoller(int inst);
  void destroyEntryListenerPoller(int poller);
  int addPolledEntryListener(int poller, String prefix, int flags);
  int addPolledEntryListener(int poller, int entry, int flags);
  EntryNotification[] pollEntryListener(NetworkTableInstance inst, int poller) throws InterruptedException;
  EntryNotification[] pollEntryListenerTimeout(NetworkTableInstance inst, int poller, double timeout) throws InterruptedException;
  void cancelPollEntryListener(int poller);
  void removeEntryListener(int entryListener);
  boolean waitForEntryListenerQueue(int inst, double timeout);

  int createConnectionListenerPoller(int inst);
  void destroyConnectionListenerPoller(int poller);
  int addPolledConnectionListener(int poller, boolean immediateNotify);
  ConnectionNotification[] pollConnectionListener(NetworkTableInstance inst, int poller) throws InterruptedException;
  ConnectionNotification[] pollConnectionListenerTimeout(NetworkTableInstance inst, int poller, double timeout) throws InterruptedException;
  void cancelPollConnectionListener(int poller);
  void removeConnectionListener(int connListener);
  boolean waitForConnectionListenerQueue(int inst, double timeout);

  int createRpcCallPoller(int inst);
  void destroyRpcCallPoller(int poller);
  void createPolledRpc(int entry, byte[] def, int poller);
  RpcAnswer[] pollRpc(NetworkTableInstance inst, int poller) throws InterruptedException;
  RpcAnswer[] pollRpcTimeout(NetworkTableInstance inst, int poller, double timeout) throws InterruptedException;
  void cancelPollRpc(int poller);
  boolean waitForRpcCallQueue(int inst, double timeout);
  boolean postRpcResponse(int entry, int call, byte[] result);
  int callRpc(int entry, byte[] params);
  byte[] getRpcResult(int entry, int call);
  byte[] getRpcResult(int entry, int call, double timeout);
  void cancelRpcResult(int entry, int call);

  byte[] getRpc(int entry, byte[] defaultValue);

  void setNetworkIdentity(int inst, String name);
  int getNetworkMode(int inst);
  void startServer(int inst, String persistFilename, String listenAddress, int port);
  void stopServer(int inst);
  void startClient(int inst);
  void startClient(int inst, String serverName, int port);
  void startClient(int inst, String[] serverNames, int[] ports);
  void startClientTeam(int inst, int team, int port);
  void stopClient(int inst);
  void setServer(int inst, String serverName, int port);
  void setServer(int inst, String[] serverNames, int[] ports);
  void setServerTeam(int inst, int team, int port);
  void startDSClient(int inst, int port);
  void stopDSClient(int inst);
  void setUpdateRate(int inst, double interval);

  void flush(int inst);

  ConnectionInfo[] getConnections(int inst);

  boolean isConnected(int inst);

  void savePersistent(int inst, String filename) throws PersistentException;
  String[] loadPersistent(int inst, String filename) throws PersistentException;  // returns warnings

  void saveEntries(int inst, String filename, String prefix) throws PersistentException;
  String[] loadEntries(int inst, String filename, String prefix) throws PersistentException;  // returns warnings

  long now();

  int createLoggerPoller(int inst);
  void destroyLoggerPoller(int poller);
  int addPolledLogger(int poller, int minLevel, int maxLevel);
  LogMessage[] pollLogger(NetworkTableInstance inst, int poller) throws InterruptedException;
  LogMessage[] pollLoggerTimeout(NetworkTableInstance inst, int poller, double timeout) throws InterruptedException;
  void cancelPollLogger(int poller);
  void removeLogger(int logger);
  boolean waitForLoggerQueue(int inst, double timeout);
}
