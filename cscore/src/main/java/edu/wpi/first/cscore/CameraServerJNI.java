// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.cscore;

import edu.wpi.first.cscore.raw.RawFrame;
import edu.wpi.first.util.RuntimeLoader;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.function.Consumer;

public class CameraServerJNI {
  static boolean libraryLoaded = false;

  static RuntimeLoader<CameraServerJNI> loader = null;

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
        loader =
            new RuntimeLoader<>(
                "cscorejni", RuntimeLoader.getDefaultExtractionRoot(), CameraServerJNI.class);
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
   *
   * @throws IOException if library load failed
   */
  public static synchronized void forceLoad() throws IOException {
    if (libraryLoaded) {
      return;
    }
    loader =
        new RuntimeLoader<>(
            "cscorejni", RuntimeLoader.getDefaultExtractionRoot(), CameraServerJNI.class);
    loader.loadLibrary();
    libraryLoaded = true;
  }

  //
  // Property Functions
  //
  public static native int getPropertyKind(int property);

  public static native String getPropertyName(int property);

  public static native int getProperty(int property);

  public static native void setProperty(int property, int value);

  public static native int getPropertyMin(int property);

  public static native int getPropertyMax(int property);

  public static native int getPropertyStep(int property);

  public static native int getPropertyDefault(int property);

  public static native String getStringProperty(int property);

  public static native void setStringProperty(int property, String value);

  public static native String[] getEnumPropertyChoices(int property);

  //
  // Source Creation Functions
  //
  public static native int createUsbCameraDev(String name, int dev);

  public static native int createUsbCameraPath(String name, String path);

  public static native int createHttpCamera(String name, String url, int kind);

  public static native int createHttpCameraMulti(String name, String[] urls, int kind);

  public static native int createRawSource(
      String name, int pixelFormat, int width, int height, int fps);

  //
  // Source Functions
  //
  public static native int getSourceKind(int source);

  public static native String getSourceName(int source);

  public static native String getSourceDescription(int source);

  public static native long getSourceLastFrameTime(int source);

  public static native void setSourceConnectionStrategy(int source, int strategy);

  public static native boolean isSourceConnected(int source);

  public static native boolean isSourceEnabled(int source);

  public static native int getSourceProperty(int source, String name);

  public static native int[] enumerateSourceProperties(int source);

  public static native VideoMode getSourceVideoMode(int source);

  public static native boolean setSourceVideoMode(
      int source, int pixelFormat, int width, int height, int fps);

  public static native boolean setSourcePixelFormat(int source, int pixelFormat);

  public static native boolean setSourceResolution(int source, int width, int height);

  public static native boolean setSourceFPS(int source, int fps);

  public static native boolean setSourceConfigJson(int source, String config);

  public static native String getSourceConfigJson(int source);

  public static native VideoMode[] enumerateSourceVideoModes(int source);

  public static native int[] enumerateSourceSinks(int source);

  public static native int copySource(int source);

  public static native void releaseSource(int source);

  //
  // Camera Source Common Property Fuctions
  //
  public static native void setCameraBrightness(int source, int brightness);

  public static native int getCameraBrightness(int source);

  public static native void setCameraWhiteBalanceAuto(int source);

  public static native void setCameraWhiteBalanceHoldCurrent(int source);

  public static native void setCameraWhiteBalanceManual(int source, int value);

  public static native void setCameraExposureAuto(int source);

  public static native void setCameraExposureHoldCurrent(int source);

  public static native void setCameraExposureManual(int source, int value);

  //
  // UsbCamera Source Functions
  //
  public static native void setUsbCameraPath(int source, String path);

  public static native String getUsbCameraPath(int source);

  public static native UsbCameraInfo getUsbCameraInfo(int source);

  //
  // HttpCamera Source Functions
  //
  public static native int getHttpCameraKind(int source);

  public static native void setHttpCameraUrls(int source, String[] urls);

  public static native String[] getHttpCameraUrls(int source);

  //
  // Image Source Functions
  //
  public static native void putRawSourceFrameBB(
      int source, ByteBuffer data, int width, int height, int pixelFormat, int totalData);

  public static native void putRawSourceFrame(
      int source, long data, int width, int height, int pixelFormat, int totalData);

  public static void putRawSourceFrame(int source, RawFrame raw) {
    putRawSourceFrame(
        source,
        raw.getDataPtr(),
        raw.getWidth(),
        raw.getHeight(),
        raw.getPixelFormat(),
        raw.getTotalData());
  }

  public static native void notifySourceError(int source, String msg);

  public static native void setSourceConnected(int source, boolean connected);

  public static native void setSourceDescription(int source, String description);

  public static native int createSourceProperty(
      int source,
      String name,
      int kind,
      int minimum,
      int maximum,
      int step,
      int defaultValue,
      int value);

  public static native void setSourceEnumPropertyChoices(
      int source, int property, String[] choices);

  //
  // Sink Creation Functions
  //
  public static native int createMjpegServer(String name, String listenAddress, int port);

  public static native int createRawSink(String name);

  //
  // Sink Functions
  //
  public static native int getSinkKind(int sink);

  public static native String getSinkName(int sink);

  public static native String getSinkDescription(int sink);

  public static native int getSinkProperty(int sink, String name);

  public static native int[] enumerateSinkProperties(int sink);

  public static native boolean setSinkConfigJson(int sink, String config);

  public static native String getSinkConfigJson(int sink);

  public static native void setSinkSource(int sink, int source);

  public static native int getSinkSourceProperty(int sink, String name);

  public static native int getSinkSource(int sink);

  public static native int copySink(int sink);

  public static native void releaseSink(int sink);

  //
  // MjpegServer Sink Functions
  //
  public static native String getMjpegServerListenAddress(int sink);

  public static native int getMjpegServerPort(int sink);

  //
  // Image Sink Functions
  //
  public static native void setSinkDescription(int sink, String description);

  private static native long grabRawSinkFrameImpl(
      int sink,
      RawFrame rawFrame,
      long rawFramePtr,
      ByteBuffer byteBuffer,
      int width,
      int height,
      int pixelFormat);

  private static native long grabRawSinkFrameTimeoutImpl(
      int sink,
      RawFrame rawFrame,
      long rawFramePtr,
      ByteBuffer byteBuffer,
      int width,
      int height,
      int pixelFormat,
      double timeout);

  public static long grabSinkFrame(int sink, RawFrame rawFrame) {
    return grabRawSinkFrameImpl(
        sink,
        rawFrame,
        rawFrame.getFramePtr(),
        rawFrame.getDataByteBuffer(),
        rawFrame.getWidth(),
        rawFrame.getHeight(),
        rawFrame.getPixelFormat());
  }

  public static long grabSinkFrameTimeout(int sink, RawFrame rawFrame, double timeout) {
    return grabRawSinkFrameTimeoutImpl(
        sink,
        rawFrame,
        rawFrame.getFramePtr(),
        rawFrame.getDataByteBuffer(),
        rawFrame.getWidth(),
        rawFrame.getHeight(),
        rawFrame.getPixelFormat(),
        timeout);
  }

  public static native String getSinkError(int sink);

  public static native void setSinkEnabled(int sink, boolean enabled);

  //
  // Listener Functions
  //
  public static native int addListener(
      Consumer<VideoEvent> listener, int eventMask, boolean immediateNotify);

  public static native void removeListener(int handle);

  public static native int createListenerPoller();

  public static native void destroyListenerPoller(int poller);

  public static native int addPolledListener(int poller, int eventMask, boolean immediateNotify);

  public static native VideoEvent[] pollListener(int poller) throws InterruptedException;

  public static native VideoEvent[] pollListenerTimeout(int poller, double timeout)
      throws InterruptedException;

  public static native void cancelPollListener(int poller);

  //
  // Telemetry Functions
  //
  public enum TelemetryKind {
    kSourceBytesReceived(1),
    kSourceFramesReceived(2);

    private final int value;

    TelemetryKind(int value) {
      this.value = value;
    }

    public int getValue() {
      return value;
    }
  }

  public static native void setTelemetryPeriod(double seconds);

  public static native double getTelemetryElapsedTime();

  public static native long getTelemetryValue(int handle, int kind);

  public static long getTelemetryValue(int handle, TelemetryKind kind) {
    return getTelemetryValue(handle, kind.getValue());
  }

  public static native double getTelemetryAverageValue(int handle, int kind);

  public static double getTelemetryAverageValue(int handle, TelemetryKind kind) {
    return getTelemetryAverageValue(handle, kind.getValue());
  }

  //
  // Logging Functions
  //
  @FunctionalInterface
  public interface LoggerFunction {
    void apply(int level, String file, int line, String msg);
  }

  public static native void setLogger(LoggerFunction func, int minLevel);

  //
  // Utility Functions
  //
  public static native UsbCameraInfo[] enumerateUsbCameras();

  public static native int[] enumerateSources();

  public static native int[] enumerateSinks();

  public static native String getHostname();

  public static native String[] getNetworkInterfaces();

  public static native long allocateRawFrame();

  public static native void freeRawFrame(long frame);
}
