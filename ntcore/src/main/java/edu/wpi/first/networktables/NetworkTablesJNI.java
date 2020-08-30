/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.networktables;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.concurrent.atomic.AtomicBoolean;

import edu.wpi.first.wpiutil.RuntimeLoader;

public final class NetworkTablesJNI {
  static boolean libraryLoaded = false;
  static RuntimeLoader<NetworkTablesJNI> loader = null;

  public static class Helper {
    private static AtomicBoolean extractOnStaticLoad = new AtomicBoolean(true);

    public static boolean getExtractOnStaticLoad() {
      return extractOnStaticLoad.get();
    }

    public static void setExtractOnStaticLoad(boolean load) {
      extractOnStaticLoad.set(load);
    }
  }

  static {
    if (Helper.getExtractOnStaticLoad()) {
      try {
        loader = new RuntimeLoader<>("ntcorejni", RuntimeLoader.getDefaultExtractionRoot(), NetworkTablesJNI.class);
        loader.loadLibrary();
      } catch (IOException ex) {
        ex.printStackTrace();
        System.exit(1);
      }
      libraryLoaded = true;
    }
  }

  /**
   * Force load the library.
   */
  public static synchronized void forceLoad() throws IOException {
    if (libraryLoaded) {
      return;
    }
    loader = new RuntimeLoader<>("ntcorejni", RuntimeLoader.getDefaultExtractionRoot(), NetworkTablesJNI.class);
    loader.loadLibrary();
    libraryLoaded = true;
  }

  public static native int getDefaultInstance();
  public static native int createInstance();
  public static native void destroyInstance(int inst);
  public static native int getInstanceFromHandle(int handle);

  public static native int getEntry(int inst, String key);
  public static native int[] getEntries(int inst, String prefix, int types);
  public static native String getEntryName(int entry);
  public static native long getEntryLastChange(int entry);

  public static native int getType(int entry);

  public static native boolean setBoolean(int entry, long time, boolean value, boolean force);
  public static native boolean setDouble(int entry, long time, double value, boolean force);
  public static native boolean setString(int entry, long time, String value, boolean force);
  public static native boolean setRaw(int entry, long time, byte[] value, boolean force);
  public static native boolean setRaw(int entry, long time, ByteBuffer value, int len, boolean force);
  public static native boolean setBooleanArray(int entry, long time, boolean[] value, boolean force);
  public static native boolean setDoubleArray(int entry, long time, double[] value, boolean force);
  public static native boolean setStringArray(int entry, long time, String[] value, boolean force);

  public static native NetworkTableValue getValue(int entry);

  public static native boolean getBoolean(int entry, boolean defaultValue);
  public static native double getDouble(int entry, double defaultValue);
  public static native String getString(int entry, String defaultValue);
  public static native byte[] getRaw(int entry, byte[] defaultValue);
  public static native boolean[] getBooleanArray(int entry, boolean[] defaultValue);
  public static native double[] getDoubleArray(int entry, double[] defaultValue);
  public static native String[] getStringArray(int entry, String[] defaultValue);
  public static native boolean setDefaultBoolean(int entry, long time, boolean defaultValue);

  public static native boolean setDefaultDouble(int entry, long time, double defaultValue);
  public static native boolean setDefaultString(int entry, long time, String defaultValue);
  public static native boolean setDefaultRaw(int entry, long time, byte[] defaultValue);
  public static native boolean setDefaultBooleanArray(int entry, long time, boolean[] defaultValue);
  public static native boolean setDefaultDoubleArray(int entry, long time, double[] defaultValue);
  public static native boolean setDefaultStringArray(int entry, long time, String[] defaultValue);

  public static native void setEntryFlags(int entry, int flags);
  public static native int getEntryFlags(int entry);

  public static native void deleteEntry(int entry);

  public static native void deleteAllEntries(int inst);

  public static native EntryInfo getEntryInfoHandle(NetworkTableInstance inst, int entry);
  public static native EntryInfo[] getEntryInfo(NetworkTableInstance instObject, int inst, String prefix, int types);

  public static native int createEntryListenerPoller(int inst);
  public static native void destroyEntryListenerPoller(int poller);
  public static native int addPolledEntryListener(int poller, String prefix, int flags);
  public static native int addPolledEntryListener(int poller, int entry, int flags);
  public static native EntryNotification[] pollEntryListener(NetworkTableInstance inst, int poller) throws InterruptedException;
  public static native EntryNotification[] pollEntryListenerTimeout(NetworkTableInstance inst, int poller, double timeout) throws InterruptedException;
  public static native void cancelPollEntryListener(int poller);
  public static native void removeEntryListener(int entryListener);
  public static native boolean waitForEntryListenerQueue(int inst, double timeout);

  public static native int createConnectionListenerPoller(int inst);
  public static native void destroyConnectionListenerPoller(int poller);
  public static native int addPolledConnectionListener(int poller, boolean immediateNotify);
  public static native ConnectionNotification[] pollConnectionListener(NetworkTableInstance inst, int poller) throws InterruptedException;
  public static native ConnectionNotification[] pollConnectionListenerTimeout(NetworkTableInstance inst, int poller, double timeout) throws InterruptedException;
  public static native void cancelPollConnectionListener(int poller);
  public static native void removeConnectionListener(int connListener);
  public static native boolean waitForConnectionListenerQueue(int inst, double timeout);

  public static native int createRpcCallPoller(int inst);
  public static native void destroyRpcCallPoller(int poller);
  public static native void createPolledRpc(int entry, byte[] def, int poller);
  public static native RpcAnswer[] pollRpc(NetworkTableInstance inst, int poller) throws InterruptedException;
  public static native RpcAnswer[] pollRpcTimeout(NetworkTableInstance inst, int poller, double timeout) throws InterruptedException;
  public static native void cancelPollRpc(int poller);
  public static native boolean waitForRpcCallQueue(int inst, double timeout);
  public static native boolean postRpcResponse(int entry, int call, byte[] result);
  public static native int callRpc(int entry, byte[] params);
  public static native byte[] getRpcResult(int entry, int call);
  public static native byte[] getRpcResult(int entry, int call, double timeout);
  public static native void cancelRpcResult(int entry, int call);

  public static native byte[] getRpc(int entry, byte[] defaultValue);

  public static native void setNetworkIdentity(int inst, String name);
  public static native int getNetworkMode(int inst);
  public static native void startLocal(int inst);
  public static native void stopLocal(int inst);
  public static native void startServer(int inst, String persistFilename, String listenAddress, int port);
  public static native void stopServer(int inst);
  public static native void startClient(int inst);
  public static native void startClient(int inst, String serverName, int port);
  public static native void startClient(int inst, String[] serverNames, int[] ports);
  public static native void startClientTeam(int inst, int team, int port);
  public static native void stopClient(int inst);
  public static native void setServer(int inst, String serverName, int port);
  public static native void setServer(int inst, String[] serverNames, int[] ports);
  public static native void setServerTeam(int inst, int team, int port);
  public static native void startDSClient(int inst, int port);
  public static native void stopDSClient(int inst);
  public static native void setUpdateRate(int inst, double interval);

  public static native void flush(int inst);

  public static native ConnectionInfo[] getConnections(int inst);

  public static native boolean isConnected(int inst);

  public static native void savePersistent(int inst, String filename) throws PersistentException;
  public static native String[] loadPersistent(int inst, String filename) throws PersistentException;  // returns warnings

  public static native void saveEntries(int inst, String filename, String prefix) throws PersistentException;
  public static native String[] loadEntries(int inst, String filename, String prefix) throws PersistentException;  // returns warnings

  public static native long now();

  public static native int createLoggerPoller(int inst);
  public static native void destroyLoggerPoller(int poller);
  public static native int addPolledLogger(int poller, int minLevel, int maxLevel);
  public static native LogMessage[] pollLogger(NetworkTableInstance inst, int poller) throws InterruptedException;
  public static native LogMessage[] pollLoggerTimeout(NetworkTableInstance inst, int poller, double timeout) throws InterruptedException;
  public static native void cancelPollLogger(int poller);
  public static native void removeLogger(int logger);
  public static native boolean waitForLoggerQueue(int inst, double timeout);
}
