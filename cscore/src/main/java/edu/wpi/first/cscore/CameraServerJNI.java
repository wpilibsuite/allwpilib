// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.cscore;

import edu.wpi.first.util.RawFrame;
import edu.wpi.first.util.RuntimeLoader;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.function.Consumer;

/** CameraServer JNI. */
public class CameraServerJNI {
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
        RuntimeLoader.loadLibrary("cscorejni");
      } catch (IOException ex) {
        ex.printStackTrace();
        System.exit(1);
      }
      libraryLoaded = true;
    }
  }

  /**
   * Force load the library.
   *
   * @throws IOException if library load failed
   */
  public static synchronized void forceLoad() throws IOException {
    if (libraryLoaded) {
      return;
    }
    RuntimeLoader.loadLibrary("cscorejni");
    libraryLoaded = true;
  }

  //
  // Property Functions
  //

  /**
   * Returns property kind.
   *
   * @param property Property handle.
   * @return Property kind.
   */
  public static native int getPropertyKind(int property);

  /**
   * Returns property name.
   *
   * @param property Property handle.
   * @return Property name.
   */
  public static native String getPropertyName(int property);

  /**
   * Returns property value.
   *
   * @param property Property handle.
   * @return Property value.
   */
  public static native int getProperty(int property);

  /**
   * Sets property value.
   *
   * @param property Property handle.
   * @param value Property value.
   */
  public static native void setProperty(int property, int value);

  /**
   * Returns property minimum.
   *
   * @param property Property handle.
   * @return Property minimum.
   */
  public static native int getPropertyMin(int property);

  /**
   * Returns property maximum.
   *
   * @param property Property handle.
   * @return Property maximum.
   */
  public static native int getPropertyMax(int property);

  /**
   * Returns property step.
   *
   * @param property Property handle.
   * @return Property step.
   */
  public static native int getPropertyStep(int property);

  /**
   * Returns property default value.
   *
   * @param property Property handle.
   * @return Property default value.
   */
  public static native int getPropertyDefault(int property);

  /**
   * Returns property value as a string.
   *
   * @param property Property handle.
   * @return Property value as a string.
   */
  public static native String getStringProperty(int property);

  /**
   * Sets property value to a string.
   *
   * @param property Property handle.
   * @param value Property value string.
   */
  public static native void setStringProperty(int property, String value);

  /**
   * Returns enum of possible property value strings.
   *
   * @param property Property handle.
   * @return Enum of possible property value strings.
   */
  public static native String[] getEnumPropertyChoices(int property);

  //
  // Source Creation Functions
  //

  /**
   * Creates a new USB camera by device.
   *
   * @param name USB camera name.
   * @param dev USB camera device number.
   * @return USB camera handle.
   */
  public static native int createUsbCameraDev(String name, int dev);

  /**
   * Creates a new USB camera by path.
   *
   * @param name USB camera name.
   * @param path USB camera path.
   * @return USB camera handle.
   */
  public static native int createUsbCameraPath(String name, String path);

  /**
   * Creates an HTTP camera.
   *
   * @param name HTTP camera name.
   * @param url HTTP camera stream URL.
   * @param kind HTTP camera kind.
   * @return HTTP camera handle.
   */
  public static native int createHttpCamera(String name, String url, int kind);

  /**
   * Creates an HTTP camera from multiple URLs.
   *
   * @param name HTTP camera name.
   * @param urls HTTP camera stream URLs.
   * @param kind HTTP camera kind.
   * @return HTTP camera handle.
   */
  public static native int createHttpCameraMulti(String name, String[] urls, int kind);

  /**
   * Creates a raw source.
   *
   * @param name Source name.
   * @param isCv true for a Cv source.
   * @param pixelFormat Pixel format.
   * @param width Image width.
   * @param height Image height.
   * @param fps Source frames per second.
   * @return Raw source handle.
   */
  public static native int createRawSource(
      String name, boolean isCv, int pixelFormat, int width, int height, int fps);

  //
  // Source Functions
  //

  /**
   * Returns source kind.
   *
   * @param source Source handle.
   * @return Source kind.
   */
  public static native int getSourceKind(int source);

  /**
   * Returns source name.
   *
   * @param source Source handle.
   * @return Source name.
   */
  public static native String getSourceName(int source);

  /**
   * Returns source description.
   *
   * @param source Source handle.
   * @return Source description.
   */
  public static native String getSourceDescription(int source);

  /**
   * Returns source's last frame time.
   *
   * @param source Source handle.
   * @return Source's last frame time.
   */
  public static native long getSourceLastFrameTime(int source);

  /**
   * Sets source connection strategy.
   *
   * @param source Source handle.
   * @param strategy Connection strategy.
   */
  public static native void setSourceConnectionStrategy(int source, int strategy);

  /**
   * Returns true if source is connected.
   *
   * @param source Source handle.
   * @return True if source is connected.
   */
  public static native boolean isSourceConnected(int source);

  /**
   * Returns true if source is enabled.
   *
   * @param source Source handle.
   * @return True if source is enabled.
   */
  public static native boolean isSourceEnabled(int source);

  /**
   * Returns source property.
   *
   * @param source Source handle.
   * @param name Source property name.
   * @return Source property.
   */
  public static native int getSourceProperty(int source, String name);

  /**
   * Returns list of source property handles.
   *
   * @param source Source handle.
   * @return List of source property handles.
   */
  public static native int[] enumerateSourceProperties(int source);

  /**
   * Returns source video mode.
   *
   * @param source Source handle.
   * @return Source video mode.
   */
  public static native VideoMode getSourceVideoMode(int source);

  /**
   * Sets source video mode.
   *
   * @param source Source handle.
   * @param pixelFormat Pixel format.
   * @param width Image width.
   * @param height Image height.
   * @param fps Source frames per second.
   * @return True if set succeeded.
   */
  public static native boolean setSourceVideoMode(
      int source, int pixelFormat, int width, int height, int fps);

  /**
   * Sets source pixel format.
   *
   * @param source Source handle.
   * @param pixelFormat Source pixel format.
   * @return True if set succeeded.
   */
  public static native boolean setSourcePixelFormat(int source, int pixelFormat);

  /**
   * Sets source resolution.
   *
   * @param source Source handle.
   * @param width Image width.
   * @param height Image height.
   * @return True if set succeeded.
   */
  public static native boolean setSourceResolution(int source, int width, int height);

  /**
   * Sets source FPS.
   *
   * @param source Source handle.
   * @param fps Source frames per second.
   * @return True if set succeeded.
   */
  public static native boolean setSourceFPS(int source, int fps);

  /**
   * Sets source configuration JSON.
   *
   * @param source Source handle.
   * @param config Configuration JSON.
   * @return True if set succeeded.
   */
  public static native boolean setSourceConfigJson(int source, String config);

  /**
   * Returns source configuration JSON.
   *
   * @param source Source handle.
   * @return Source configuration JSON.
   */
  public static native String getSourceConfigJson(int source);

  /**
   * Returns list of source's supported video modes.
   *
   * @param source Source handle.
   * @return List of source's supported video modes.
   */
  public static native VideoMode[] enumerateSourceVideoModes(int source);

  /**
   * Returns list of source sinks.
   *
   * @param source Source handle.
   * @return List of source sinks.
   */
  public static native int[] enumerateSourceSinks(int source);

  /**
   * Copies source.
   *
   * @param source Source handle.
   * @return New source handle.
   */
  public static native int copySource(int source);

  /**
   * Releases source.
   *
   * @param source Source handle.
   */
  public static native void releaseSource(int source);

  //
  // Camera Source Common Property Functions
  //

  /**
   * Sets camera brightness.
   *
   * @param source Source handle.
   * @param brightness Brightness.
   */
  public static native void setCameraBrightness(int source, int brightness);

  /**
   * Returns camera brightness.
   *
   * @param source Source handle.
   * @return Camera brightness.
   */
  public static native int getCameraBrightness(int source);

  /**
   * Sets camera white balance to auto.
   *
   * @param source Source handle.
   */
  public static native void setCameraWhiteBalanceAuto(int source);

  /**
   * Sets camera white balance to "hold current".
   *
   * @param source Source handle.
   */
  public static native void setCameraWhiteBalanceHoldCurrent(int source);

  /**
   * Sets camera white balance to the given value.
   *
   * @param source Source handle.
   * @param value White balance.
   */
  public static native void setCameraWhiteBalanceManual(int source, int value);

  /**
   * Sets camera exposure to auto.
   *
   * @param source Source handle.
   */
  public static native void setCameraExposureAuto(int source);

  /**
   * Sets camera exposure to "hold current".
   *
   * @param source Source handle.
   */
  public static native void setCameraExposureHoldCurrent(int source);

  /**
   * Sets camera exposure to the given value.
   *
   * @param source Source handle.
   * @param value Exposure.
   */
  public static native void setCameraExposureManual(int source, int value);

  //
  // UsbCamera Source Functions
  //

  /**
   * Sets USB camera path.
   *
   * @param source Source handle.
   * @param path USB camera path.
   */
  public static native void setUsbCameraPath(int source, String path);

  /**
   * Returns USB camera path.
   *
   * @param source Source handle.
   * @return USB camera path.
   */
  public static native String getUsbCameraPath(int source);

  /**
   * Returns USB camera info.
   *
   * @param source Source handle.
   * @return USB camera info.
   */
  public static native UsbCameraInfo getUsbCameraInfo(int source);

  //
  // HttpCamera Source Functions
  //

  /**
   * Returns HTTP camera kind.
   *
   * @param source Source handle.
   * @return HTTP camera kind.
   */
  public static native int getHttpCameraKind(int source);

  /**
   * Sets HTTP camera URLs.
   *
   * @param source Source handle.
   * @param urls HTTP camera URLs.
   */
  public static native void setHttpCameraUrls(int source, String[] urls);

  /**
   * Returns HTTP camera URLs.
   *
   * @param source Source handle.
   * @return HTTP camera URLs.
   */
  public static native String[] getHttpCameraUrls(int source);

  //
  // Image Source Functions
  //

  /**
   * Puts raw frame into source.
   *
   * @param source Source handle.
   * @param frame Frame handle.
   */
  public static native void putRawSourceFrame(int source, long frame);

  /**
   * Puts raw frame into source.
   *
   * @param source Source handle.
   * @param data Frame byte buffer.
   * @param size Frame size.
   * @param width Frame width.
   * @param height Frame height.
   * @param stride Frame stride.
   * @param pixelFormat Frame pixel format.
   */
  public static native void putRawSourceFrameBB(
      int source, ByteBuffer data, int size, int width, int height, int stride, int pixelFormat);

  /**
   * Puts raw frame into source.
   *
   * @param source Source handle.
   * @param data Frame handle.
   * @param size Frame size.
   * @param width Frame width.
   * @param height Frame height.
   * @param stride Frame stride.
   * @param pixelFormat Frame pixel format.
   */
  public static native void putRawSourceFrameData(
      int source, long data, int size, int width, int height, int stride, int pixelFormat);

  /**
   * Notify source error.
   *
   * @param source Source handle.
   * @param msg Error message.
   */
  public static native void notifySourceError(int source, String msg);

  /**
   * Sets whether source is connected.
   *
   * @param source Source handle.
   * @param connected True if source is connected.
   */
  public static native void setSourceConnected(int source, boolean connected);

  /**
   * Sets source description.
   *
   * @param source Source handle.
   * @param description Source description.
   */
  public static native void setSourceDescription(int source, String description);

  /**
   * Creates a source property.
   *
   * @param source Source handle.
   * @param name Property name.
   * @param kind Property kind.
   * @param minimum Property minimum.
   * @param maximum Property maximum.
   * @param step Property step.
   * @param defaultValue Property default value.
   * @param value Property value.
   * @return Source property handle.
   */
  public static native int createSourceProperty(
      int source,
      String name,
      int kind,
      int minimum,
      int maximum,
      int step,
      int defaultValue,
      int value);

  /**
   * Sets list of possible property values.
   *
   * @param source Source handle.
   * @param property Property handle.
   * @param choices List of possible property values.
   */
  public static native void setSourceEnumPropertyChoices(
      int source, int property, String[] choices);

  //
  // Sink Creation Functions
  //

  /**
   * Creates an MJPEG server.
   *
   * @param name MJPEG server name.
   * @param listenAddress IP address at which server should listen.
   * @param port Port on which server should listen.
   * @return MJPEG server handle.
   */
  public static native int createMjpegServer(String name, String listenAddress, int port);

  /**
   * Creates a raw sink.
   *
   * @param name Sink name.
   * @param isCv true for a Cv source.
   * @return Raw sink handle.
   */
  public static native int createRawSink(String name, boolean isCv);

  //
  // Sink Functions
  //

  /**
   * Returns sink kind.
   *
   * @param sink Sink handle.
   * @return Sink kind.
   */
  public static native int getSinkKind(int sink);

  /**
   * Returns sink name.
   *
   * @param sink Sink handle.
   * @return Sink name.
   */
  public static native String getSinkName(int sink);

  /**
   * Returns sink description.
   *
   * @param sink Sink handle.
   * @return Sink description.
   */
  public static native String getSinkDescription(int sink);

  /**
   * Returns sink property.
   *
   * @param sink Sink handle.
   * @param name Property name.
   * @return Sink property handle.
   */
  public static native int getSinkProperty(int sink, String name);

  /**
   * Returns list of sink property handles.
   *
   * @param sink Sink handle.
   * @return List of sink property handles.
   */
  public static native int[] enumerateSinkProperties(int sink);

  /**
   * Sets sink configuration JSON.
   *
   * @param sink Sink handle.
   * @param config Configuration JSON.
   * @return True if set succeeded.
   */
  public static native boolean setSinkConfigJson(int sink, String config);

  /**
   * Returns sink configuration JSON.
   *
   * @param sink Sink handle.
   * @return Sink configuration JSON.
   */
  public static native String getSinkConfigJson(int sink);

  /**
   * Sets sink source.
   *
   * @param sink Sink handle.
   * @param source Source handle.
   */
  public static native void setSinkSource(int sink, int source);

  /**
   * Returns sink source property.
   *
   * @param sink Sink handle.
   * @param name Property name.
   * @return Sink source property handle.
   */
  public static native int getSinkSourceProperty(int sink, String name);

  /**
   * Returns sink source.
   *
   * @param sink Sink handle.
   * @return Sink source handle.
   */
  public static native int getSinkSource(int sink);

  /**
   * Copies sink.
   *
   * @param sink Sink handle.
   * @return New sink handle.
   */
  public static native int copySink(int sink);

  /**
   * Releases sink.
   *
   * @param sink Sink handle.
   */
  public static native void releaseSink(int sink);

  //
  // MjpegServer Sink Functions
  //

  /**
   * Returns MJPEG server listen address.
   *
   * @param sink Sink handle.
   * @return MJPEG server listen address.
   */
  public static native String getMjpegServerListenAddress(int sink);

  /**
   * Returns MJPEG server port.
   *
   * @param sink Sink handle.
   * @return MJPEG server port.
   */
  public static native int getMjpegServerPort(int sink);

  //
  // Image Sink Functions
  //

  /**
   * Sets sink description.
   *
   * @param sink Sink handle.
   * @param description Sink description.
   */
  public static native void setSinkDescription(int sink, String description);

  /**
   * Returns raw sink frame.
   *
   * @param sink Sink handle.
   * @param frame Raw frame.
   * @param nativeObj Native object.
   * @return Raw sink frame handle.
   */
  public static native long grabRawSinkFrame(int sink, RawFrame frame, long nativeObj);

  /**
   * Returns raw sink frame timeout.
   *
   * @param sink Sink handle.
   * @param frame Raw frame.
   * @param nativeObj Native object.
   * @param timeout Timeout in seconds.
   * @return Raw sink frame timeout.
   */
  public static native long grabRawSinkFrameTimeout(
      int sink, RawFrame frame, long nativeObj, double timeout);

  /**
   * Returns sink error message.
   *
   * @param sink Sink handle.
   * @return Sink error message.
   */
  public static native String getSinkError(int sink);

  /**
   * Sets sink enable.
   *
   * @param sink Sink handle.
   * @param enabled True if sink should be enabled.
   */
  public static native void setSinkEnabled(int sink, boolean enabled);

  //
  // Listener Functions
  //

  /**
   * Adds listener.
   *
   * @param listener Video event callback.
   * @param eventMask Event mask.
   * @param immediateNotify True to immediately notify on event.
   * @return Listener handle.
   */
  public static native int addListener(
      Consumer<VideoEvent> listener, int eventMask, boolean immediateNotify);

  /**
   * Removes listener.
   *
   * @param handle Listener handle.
   */
  public static native void removeListener(int handle);

  /**
   * Creates listener poller.
   *
   * @return Listener poller handle.
   */
  public static native int createListenerPoller();

  /**
   * Destroys listener poller.
   *
   * @param poller Listener poller handle.
   */
  public static native void destroyListenerPoller(int poller);

  /**
   * Add polled listener.
   *
   * @param poller Poller handle.
   * @param eventMask Event mask.
   * @param immediateNotify True to immediately notify on event.
   * @return Polled listener handle.
   */
  public static native int addPolledListener(int poller, int eventMask, boolean immediateNotify);

  /**
   * Polls listener.
   *
   * @param poller Poller handle.
   * @return List of video events.
   * @throws InterruptedException if polling was interrupted.
   */
  public static native VideoEvent[] pollListener(int poller) throws InterruptedException;

  /**
   * Polls listener with timeout.
   *
   * @param poller Poller handle.
   * @param timeout Timeout in seconds.
   * @return List of video events.
   * @throws InterruptedException if polling was interrupted.
   */
  public static native VideoEvent[] pollListenerTimeout(int poller, double timeout)
      throws InterruptedException;

  /**
   * Cancels poll listener.
   *
   * @param poller Poller handle.
   */
  public static native void cancelPollListener(int poller);

  //
  // Telemetry Functions
  //

  /** Telemetry kind. */
  public enum TelemetryKind {
    /** kSourceBytesReceived. */
    kSourceBytesReceived(1),
    /** kSourceFramesReceived. */
    kSourceFramesReceived(2);

    private final int value;

    TelemetryKind(int value) {
      this.value = value;
    }

    /**
     * Returns telemetry kind value.
     *
     * @return Telemetry kind value.
     */
    public int getValue() {
      return value;
    }
  }

  /**
   * Sets telemetry period.
   *
   * @param seconds Telemetry period in seconds.
   */
  public static native void setTelemetryPeriod(double seconds);

  /**
   * Returns telemetry elapsed time.
   *
   * @return Telemetry elapsed time.
   */
  public static native double getTelemetryElapsedTime();

  /**
   * Returns telemetry value.
   *
   * @param handle Telemetry handle.
   * @param kind Telemetry kind.
   * @return Telemetry value.
   */
  public static native long getTelemetryValue(int handle, int kind);

  /**
   * Returns telemetry value.
   *
   * @param handle Telemetry handle.
   * @param kind Telemetry kind.
   * @return Telemetry value.
   */
  public static long getTelemetryValue(int handle, TelemetryKind kind) {
    return getTelemetryValue(handle, kind.getValue());
  }

  /**
   * Returns telemetry average value.
   *
   * @param handle Telemetry handle.
   * @param kind Telemetry kind.
   * @return Telemetry average value.
   */
  public static native double getTelemetryAverageValue(int handle, int kind);

  /**
   * Returns telemetry average value.
   *
   * @param handle Telemetry handle.
   * @param kind Telemetry kind.
   * @return Telemetry average value.
   */
  public static double getTelemetryAverageValue(int handle, TelemetryKind kind) {
    return getTelemetryAverageValue(handle, kind.getValue());
  }

  //
  // Logging Functions
  //

  /** Logger functional interface. */
  @FunctionalInterface
  public interface LoggerFunction {
    /**
     * Log a string.
     *
     * @param level Log level.
     * @param file Log file.
     * @param line Line number.
     * @param msg Log message.
     */
    void apply(int level, String file, int line, String msg);
  }

  /**
   * Sets logger.
   *
   * @param func Logger function.
   * @param minLevel Minimum logging level.
   */
  public static native void setLogger(LoggerFunction func, int minLevel);

  //
  // Utility Functions
  //

  /**
   * Returns list of USB cameras.
   *
   * @return List of USB cameras.
   */
  public static native UsbCameraInfo[] enumerateUsbCameras();

  /**
   * Returns list of sources.
   *
   * @return List of sources.
   */
  public static native int[] enumerateSources();

  /**
   * Returns list of sinks.
   *
   * @return List of sinks.
   */
  public static native int[] enumerateSinks();

  /**
   * Returns hostname.
   *
   * @return Hostname.
   */
  public static native String getHostname();

  /**
   * Returns list of network interfaces.
   *
   * @return List of network interfaces.
   */
  public static native String[] getNetworkInterfaces();

  /** Runs main run loop. */
  public static native void runMainRunLoop();

  /**
   * Runs main run loop with timeout.
   *
   * @param timeoutSeconds Timeout in seconds.
   * @return 3 on timeout, 2 on signal, 1 on other.
   */
  public static native int runMainRunLoopTimeout(double timeoutSeconds);

  /** Stops main run loop. */
  public static native void stopMainRunLoop();

  /** Utility class. */
  private CameraServerJNI() {}
}
