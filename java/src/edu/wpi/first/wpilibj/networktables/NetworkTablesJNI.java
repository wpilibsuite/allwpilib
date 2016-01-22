package edu.wpi.first.wpilibj.networktables;

import edu.wpi.first.wpilibj.tables.*;

import java.io.File;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;

public class NetworkTablesJNI {
  static boolean libraryLoaded = false;
  static File jniLibrary = null;
  static {
    if (!libraryLoaded) {
      try {
        String osname = System.getProperty("os.name");
        String resname;
        if (osname.startsWith("Windows"))
          resname = "/Windows/" + System.getProperty("os.arch") + "/";
        else
          resname = "/" + osname + "/" + System.getProperty("os.arch") + "/";
        System.out.println("platform: " + resname);
        if (osname.startsWith("Windows"))
          resname += "ntcore.dll";
        else if (osname.startsWith("Mac"))
          resname += "libntcore.dylib";
        else
          resname += "libntcore.so";
        InputStream is = NetworkTablesJNI.class.getResourceAsStream(resname);
        if (is != null) {
          // create temporary file
          if (System.getProperty("os.name").startsWith("Windows"))
            jniLibrary = File.createTempFile("NetworkTablesJNI", ".dll");
          else if (System.getProperty("os.name").startsWith("Mac"))
            jniLibrary = File.createTempFile("libNetworkTablesJNI", ".dylib");
          else
            jniLibrary = File.createTempFile("libNetworkTablesJNI", ".so");
          // flag for delete on exit
          jniLibrary.deleteOnExit();
          OutputStream os = new FileOutputStream(jniLibrary);

          byte[] buffer = new byte[1024];
          int readBytes;
          try {
            while ((readBytes = is.read(buffer)) != -1) {
              os.write(buffer, 0, readBytes);
            }
          } finally {
            os.close();
            is.close();
          }
          try {
            System.load(jniLibrary.getAbsolutePath());
          } catch (UnsatisfiedLinkError e) {
            System.loadLibrary("ntcore");
          }
        } else {
          System.loadLibrary("ntcore");
        }
      } catch (IOException ex) {
        ex.printStackTrace();
        System.exit(1);
      }
      libraryLoaded = true;
    }
  }

  public static native boolean containsKey(String key);
  public static native int getType(String key);

  public static native boolean putBoolean(String key, boolean value);
  public static native boolean putDouble(String key, double value);
  public static native boolean putString(String key, String value);
  public static native boolean putRaw(String key, byte[] value);
  public static native boolean putRaw(String key, ByteBuffer value, int len);
  public static native boolean putBooleanArray(String key, boolean[] value);
  public static native boolean putDoubleArray(String key, double[] value);
  public static native boolean putStringArray(String key, String[] value);

  public static native void forcePutBoolean(String key, boolean value);
  public static native void forcePutDouble(String key, double value);
  public static native void forcePutString(String key, String value);
  public static native void forcePutRaw(String key, byte[] value);
  public static native void forcePutRaw(String key, ByteBuffer value, int len);
  public static native void forcePutBooleanArray(String key, boolean[] value);
  public static native void forcePutDoubleArray(String key, double[] value);
  public static native void forcePutStringArray(String key, String[] value);

  public static native Object getValue(String key) throws TableKeyNotDefinedException;
  public static native boolean getBoolean(String key) throws TableKeyNotDefinedException;
  public static native double getDouble(String key) throws TableKeyNotDefinedException;
  public static native String getString(String key) throws TableKeyNotDefinedException;
  public static native byte[] getRaw(String key) throws TableKeyNotDefinedException;
  public static native boolean[] getBooleanArray(String key) throws TableKeyNotDefinedException;
  public static native double[] getDoubleArray(String key) throws TableKeyNotDefinedException;
  public static native String[] getStringArray(String key) throws TableKeyNotDefinedException;

  public static native Object getValue(String key, Object defaultValue);
  public static native boolean getBoolean(String key, boolean defaultValue);
  public static native double getDouble(String key, double defaultValue);
  public static native String getString(String key, String defaultValue);
  public static native byte[] getRaw(String key, byte[] defaultValue);
  public static native boolean[] getBooleanArray(String key, boolean[] defaultValue);
  public static native double[] getDoubleArray(String key, double[] defaultValue);
  public static native String[] getStringArray(String key, String[] defaultValue);

  public static native void setEntryFlags(String key, int flags);
  public static native int getEntryFlags(String key);

  public static native void deleteEntry(String key);
  public static native void deleteAllEntries();

  public static native EntryInfo[] getEntries(String prefix, int types);

  public static native void flush();

  public interface EntryListenerFunction {
    void apply(int uid, String key, Object value, int flags);
  }
  public static native int addEntryListener(String prefix, EntryListenerFunction listener, int flags);
  public static native void removeEntryListener(int entryListenerUid);

  public interface ConnectionListenerFunction {
    void apply(int uid, boolean connected, ConnectionInfo conn);
  }
  public static native int addConnectionListener(ConnectionListenerFunction listener, boolean immediateNotify);
  public static native void removeConnectionListener(int connListenerUid);

  // public static native void createRpc(String key, byte[] def, IRpc rpc);
  // public static native void createRpc(String key, ByteBuffer def, int def_len, IRpc rpc);
  public static native byte[] getRpc(String key) throws TableKeyNotDefinedException;
  public static native byte[] getRpc(String key, byte[] defaultValue);
  public static native int callRpc(String key, byte[] params);
  public static native int callRpc(String key, ByteBuffer params, int params_len);
  // public static native byte[] getRpcResultBlocking(int callUid);
  // public static native byte[] getRpcResultNonblocking(int callUid) throws RpcNoResponseException;

  public static native void setNetworkIdentity(String name);
  public static native void startServer(String persistFilename, String listenAddress, int port);
  public static native void stopServer();
  public static native void startClient(String serverName, int port);
  public static native void stopClient();
  public static native void setUpdateRate(double interval);

  public static native ConnectionInfo[] getConnections();

  public static native void savePersistent(String filename) throws PersistentException;
  public static native String[] loadPersistent(String filename) throws PersistentException;  // returns warnings

  public static native long now();

  public interface LoggerFunction {
    void apply(int level, String file, int line, String msg);
  }
  public static native void setLogger(LoggerFunction func, int minLevel);
}
