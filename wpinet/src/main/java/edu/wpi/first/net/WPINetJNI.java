// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.net;

import edu.wpi.first.util.RuntimeLoader;
import java.io.IOException;
import java.util.concurrent.atomic.AtomicBoolean;

/** WPINet JNI. */
public class WPINetJNI {
  static boolean libraryLoaded = false;

  /** Sets whether JNI should be loaded in the static block. */
  public static class Helper {
    private static AtomicBoolean extractOnStaticLoad = new AtomicBoolean(true);

    /**
     * Returns true if the JNI should be loaded in the static block.
     *
     * @return True if the JNI should be loaded in the static block.
     */
    public static boolean getExtractOnStaticLoad() {
      return extractOnStaticLoad.get();
    }

    /**
     * Sets whether the JNI should be loaded in the static block.
     *
     * @param load Whether the JNI should be loaded in the static block.
     */
    public static void setExtractOnStaticLoad(boolean load) {
      extractOnStaticLoad.set(load);
    }

    /** Utility class. */
    private Helper() {}
  }

  static {
    if (Helper.getExtractOnStaticLoad()) {
      try {
        RuntimeLoader.loadLibrary("wpinetjni");
      } catch (Exception ex) {
        ex.printStackTrace();
        System.exit(1);
      }
      libraryLoaded = true;
    }
  }

  /**
   * Force load the library.
   *
   * @throws IOException if the library failed to load
   */
  public static synchronized void forceLoad() throws IOException {
    if (libraryLoaded) {
      return;
    }
    RuntimeLoader.loadLibrary("wpinetjni");
    libraryLoaded = true;
  }

  /**
   * Forward a local TCP port to a remote host and port. Note that local ports less than 1024 won't
   * work as a normal user.
   *
   * @param port local port number
   * @param remoteHost remote IP address / DNS name
   * @param remotePort remote port number
   */
  public static native void addPortForwarder(int port, String remoteHost, int remotePort);

  /**
   * Stop TCP forwarding on a port.
   *
   * @param port local port number
   */
  public static native void removePortForwarder(int port);

  /**
   * Create a web server at the given port. Note that local ports less than 1024 won't work as a
   * normal user.
   *
   * @param port local port number
   * @param path local path to document root
   */
  public static native void startWebServer(int port, String path);

  /**
   * Stop web server running at the given port.
   *
   * @param port local port number
   */
  public static native void stopWebServer(int port);

  /**
   * Creates a MulticastServiceAnnouncer.
   *
   * @param serviceName service name
   * @param serviceType service type
   * @param port port
   * @param keys keys
   * @param values values
   * @return MulticastServiceAnnouncer handle.
   */
  public static native int createMulticastServiceAnnouncer(
      String serviceName, String serviceType, int port, String[] keys, String[] values);

  /**
   * Frees a MulticastServiceAnnouncer.
   *
   * @param handle MulticastServiceAnnouncer handle.
   */
  public static native void freeMulticastServiceAnnouncer(int handle);

  /**
   * Starts MulticastServiceAnnouncer.
   *
   * @param handle MulticastServiceAnnouncer handle.
   */
  public static native void startMulticastServiceAnnouncer(int handle);

  /**
   * Stops MulticastServiceAnnouncer.
   *
   * @param handle MulticastServiceAnnouncer handle.
   */
  public static native void stopMulticastServiceAnnouncer(int handle);

  /**
   * Returns true if MulticastServiceAnnouncer has an implementation.
   *
   * @param handle MulticastServiceAnnouncer handle.
   * @return True if MulticastServiceAnnouncer has an implementation.
   */
  public static native boolean getMulticastServiceAnnouncerHasImplementation(int handle);

  /**
   * Creates a MulticastServiceResolver.
   *
   * @param serviceType Service type.
   * @return MulticastServiceResolver handle.
   */
  public static native int createMulticastServiceResolver(String serviceType);

  /**
   * Frees MulticastServiceResolver.
   *
   * @param handle MulticastServiceResolver handle.
   */
  public static native void freeMulticastServiceResolver(int handle);

  /**
   * Starts MulticastServiceResolver.
   *
   * @param handle MulticastServiceResolver handle.
   */
  public static native void startMulticastServiceResolver(int handle);

  /**
   * Stops MulticastServiceResolver.
   *
   * @param handle MulticastServiceResolver handle.
   */
  public static native void stopMulticastServiceResolver(int handle);

  /**
   * Returns true if MulticastServiceResolver has an implementation.
   *
   * @param handle MulticastServiceResolver handle.
   * @return True if MulticastServiceResolver has an implementation.
   */
  public static native boolean getMulticastServiceResolverHasImplementation(int handle);

  /**
   * Returns event handle for MulticastServiceResolver.
   *
   * @param handle MulticastServiceResolver handle.
   * @return Event handle for MulticastServiceResolver.
   */
  public static native int getMulticastServiceResolverEventHandle(int handle);

  /**
   * Returns service data for MulticastServiceResolver.
   *
   * @param handle MulticastServiceResolver handle.
   * @return Service data for MulticastServiceResolver.
   */
  public static native ServiceData[] getMulticastServiceResolverData(int handle);

  /** Utility class. */
  private WPINetJNI() {}
}
