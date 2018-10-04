/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.networktables;

import java.io.IOException;
import java.nio.ByteBuffer;

import edu.wpi.first.wpiutil.RuntimeLoader;

public final class NetworkTablesJNI implements INetworkTablesJNI {
  private static INetworkTablesJNI instance;

  /**
   * Gets the JNI instance for NetworkTables.
   *
   * @return The JNI instance
   */
  public static synchronized INetworkTablesJNI getInstance() {
    if (instance == null) {
      instance = new NetworkTablesJNI();
    }
    return instance;
  }

  private boolean m_libraryLoaded = false;
  private RuntimeLoader<NetworkTablesJNI> m_loader = null;

  private NetworkTablesJNI() {
    if (!m_libraryLoaded) {
      try {
        m_loader = new RuntimeLoader<>("ntcore", RuntimeLoader.getDefaultExtractionRoot(), NetworkTablesJNI.class);
        m_loader.loadLibrary();
      } catch (IOException ex) {
        ex.printStackTrace();
        System.exit(1);
      }
      m_libraryLoaded = true;
    }
  }

  @Override
  public native int getDefaultInstance();
  @Override
  public native int createInstance();
  @Override
  public native void destroyInstance(int inst);
  @Override
  public native int getInstanceFromHandle(int handle);

  @Override
  public native int getEntry(int inst, String key);
  @Override
  public native int[] getEntries(int inst, String prefix, int types);
  @Override
  public native String getEntryName(int entry);
  @Override
  public native long getEntryLastChange(int entry);

  @Override
  public native int getType(int entry);

  @Override
  public native boolean setBoolean(int entry, long time, boolean value, boolean force);
  @Override
  public native boolean setDouble(int entry, long time, double value, boolean force);
  @Override
  public native boolean setString(int entry, long time, String value, boolean force);
  @Override
  public native boolean setRaw(int entry, long time, byte[] value, boolean force);
  @Override
  public native boolean setRaw(int entry, long time, ByteBuffer value, int len, boolean force);
  @Override
  public native boolean setBooleanArray(int entry, long time, boolean[] value, boolean force);
  @Override
  public native boolean setDoubleArray(int entry, long time, double[] value, boolean force);
  @Override
  public native boolean setStringArray(int entry, long time, String[] value, boolean force);

  @Override
  public native NetworkTableValue getValue(int entry);

  @Override
  public native boolean getBoolean(int entry, boolean defaultValue);
  @Override
  public native double getDouble(int entry, double defaultValue);
  @Override
  public native String getString(int entry, String defaultValue);
  @Override
  public native byte[] getRaw(int entry, byte[] defaultValue);
  @Override
  public native boolean[] getBooleanArray(int entry, boolean[] defaultValue);
  @Override
  public native double[] getDoubleArray(int entry, double[] defaultValue);
  @Override
  public native String[] getStringArray(int entry, String[] defaultValue);
  @Override
  public native boolean setDefaultBoolean(int entry, long time, boolean defaultValue);

  @Override
  public native boolean setDefaultDouble(int entry, long time, double defaultValue);
  @Override
  public native boolean setDefaultString(int entry, long time, String defaultValue);
  @Override
  public native boolean setDefaultRaw(int entry, long time, byte[] defaultValue);
  @Override
  public native boolean setDefaultBooleanArray(int entry, long time, boolean[] defaultValue);
  @Override
  public native boolean setDefaultDoubleArray(int entry, long time, double[] defaultValue);
  @Override
  public native boolean setDefaultStringArray(int entry, long time, String[] defaultValue);

  @Override
  public native void setEntryFlags(int entry, int flags);
  @Override
  public native int getEntryFlags(int entry);

  @Override
  public native void deleteEntry(int entry);

  @Override
  public native void deleteAllEntries(int inst);

  @Override
  public native EntryInfo getEntryInfoHandle(NetworkTableInstance inst, int entry);
  @Override
  public native EntryInfo[] getEntryInfo(NetworkTableInstance instObject, int inst, String prefix, int types);

  @Override
  public native int createEntryListenerPoller(int inst);
  @Override
  public native void destroyEntryListenerPoller(int poller);
  @Override
  public native int addPolledEntryListener(int poller, String prefix, int flags);
  @Override
  public native int addPolledEntryListener(int poller, int entry, int flags);
  @Override
  public native EntryNotification[] pollEntryListener(NetworkTableInstance inst, int poller) throws InterruptedException;
  @Override
  public native EntryNotification[] pollEntryListenerTimeout(NetworkTableInstance inst, int poller, double timeout) throws InterruptedException;
  @Override
  public native void cancelPollEntryListener(int poller);
  @Override
  public native void removeEntryListener(int entryListener);
  @Override
  public native boolean waitForEntryListenerQueue(int inst, double timeout);

  @Override
  public native int createConnectionListenerPoller(int inst);
  @Override
  public native void destroyConnectionListenerPoller(int poller);
  @Override
  public native int addPolledConnectionListener(int poller, boolean immediateNotify);
  @Override
  public native ConnectionNotification[] pollConnectionListener(NetworkTableInstance inst, int poller) throws InterruptedException;
  @Override
  public native ConnectionNotification[] pollConnectionListenerTimeout(NetworkTableInstance inst, int poller, double timeout) throws InterruptedException;
  @Override
  public native void cancelPollConnectionListener(int poller);
  @Override
  public native void removeConnectionListener(int connListener);
  @Override
  public native boolean waitForConnectionListenerQueue(int inst, double timeout);

  @Override
  public native int createRpcCallPoller(int inst);
  @Override
  public native void destroyRpcCallPoller(int poller);
  @Override
  public native void createPolledRpc(int entry, byte[] def, int poller);
  @Override
  public native RpcAnswer[] pollRpc(NetworkTableInstance inst, int poller) throws InterruptedException;
  @Override
  public native RpcAnswer[] pollRpcTimeout(NetworkTableInstance inst, int poller, double timeout) throws InterruptedException;
  @Override
  public native void cancelPollRpc(int poller);
  @Override
  public native boolean waitForRpcCallQueue(int inst, double timeout);
  @Override
  public native boolean postRpcResponse(int entry, int call, byte[] result);
  @Override
  public native int callRpc(int entry, byte[] params);
  @Override
  public native byte[] getRpcResult(int entry, int call);
  @Override
  public native byte[] getRpcResult(int entry, int call, double timeout);
  @Override
  public native void cancelRpcResult(int entry, int call);

  @Override
  public native byte[] getRpc(int entry, byte[] defaultValue);

  @Override
  public native void setNetworkIdentity(int inst, String name);
  @Override
  public native int getNetworkMode(int inst);
  @Override
  public native void startServer(int inst, String persistFilename, String listenAddress, int port);
  @Override
  public native void stopServer(int inst);
  @Override
  public native void startClient(int inst);
  @Override
  public native void startClient(int inst, String serverName, int port);
  @Override
  public native void startClient(int inst, String[] serverNames, int[] ports);
  @Override
  public native void startClientTeam(int inst, int team, int port);
  @Override
  public native void stopClient(int inst);
  @Override
  public native void setServer(int inst, String serverName, int port);
  @Override
  public native void setServer(int inst, String[] serverNames, int[] ports);
  @Override
  public native void setServerTeam(int inst, int team, int port);
  @Override
  public native void startDSClient(int inst, int port);
  @Override
  public native void stopDSClient(int inst);
  @Override
  public native void setUpdateRate(int inst, double interval);

  @Override
  public native void flush(int inst);

  @Override
  public native ConnectionInfo[] getConnections(int inst);

  @Override
  public native boolean isConnected(int inst);

  @Override
  public native void savePersistent(int inst, String filename) throws PersistentException;
  @Override
  public native String[] loadPersistent(int inst, String filename) throws PersistentException;  // returns warnings

  @Override
  public native void saveEntries(int inst, String filename, String prefix) throws PersistentException;
  @Override
  public native String[] loadEntries(int inst, String filename, String prefix) throws PersistentException;  // returns warnings

  @Override
  public native long now();

  @Override
  public native int createLoggerPoller(int inst);
  @Override
  public native void destroyLoggerPoller(int poller);
  @Override
  public native int addPolledLogger(int poller, int minLevel, int maxLevel);
  @Override
  public native LogMessage[] pollLogger(NetworkTableInstance inst, int poller) throws InterruptedException;
  @Override
  public native LogMessage[] pollLoggerTimeout(NetworkTableInstance inst, int poller, double timeout) throws InterruptedException;
  @Override
  public native void cancelPollLogger(int poller);
  @Override
  public native void removeLogger(int logger);
  @Override
  public native boolean waitForLoggerQueue(int inst, double timeout);
}
