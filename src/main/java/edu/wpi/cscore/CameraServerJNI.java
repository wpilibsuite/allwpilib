/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.cscore;

import java.io.File;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.function.Consumer;
import org.opencv.core.Core;
import edu.wpi.first.wpiutil.RuntimeDetector;

public class CameraServerJNI {
  static boolean libraryLoaded = false;
  static File jniLibrary = null;
  static boolean cvLibraryLoaded = false;
  static File cvJniLibrary = null;
  static {
    if (!libraryLoaded) {
      try {
        System.loadLibrary("cscore");
      } catch (UnsatisfiedLinkError e) {
        try {
          String resname = RuntimeDetector.getLibraryResource("cscore");
          InputStream is = CameraServerJNI.class.getResourceAsStream(resname);
          if (is != null) {
            // create temporary file
            if (System.getProperty("os.name").startsWith("Windows"))
              jniLibrary = File.createTempFile("CameraServerJNI", ".dll");
            else if (System.getProperty("os.name").startsWith("Mac"))
              jniLibrary = File.createTempFile("libCameraServerJNI", ".dylib");
            else
              jniLibrary = File.createTempFile("libCameraServerJNI", ".so");
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
            System.load(jniLibrary.getAbsolutePath());
          } else {
            System.loadLibrary("cscore");
          }
        } catch (IOException ex) {
          ex.printStackTrace();
          System.exit(1);
        }
      }
      libraryLoaded = true;
    }

    String opencvName = Core.NATIVE_LIBRARY_NAME;
    if (!cvLibraryLoaded) {
      try {

        System.loadLibrary(opencvName);
      } catch (UnsatisfiedLinkError e) {
        try {
          String resname = RuntimeDetector.getLibraryResource(opencvName);
          InputStream is = CameraServerJNI.class.getResourceAsStream(resname);
          if (is != null) {
            // create temporary file
            if (System.getProperty("os.name").startsWith("Windows"))
              cvJniLibrary = File.createTempFile("OpenCVJNI", ".dll");
            else if (System.getProperty("os.name").startsWith("Mac"))
              cvJniLibrary = File.createTempFile("libOpenCVJNI", ".dylib");
            else
              cvJniLibrary = File.createTempFile("libOpenCVJNI", ".so");
            // flag for delete on exit
            cvJniLibrary.deleteOnExit();
            OutputStream os = new FileOutputStream(cvJniLibrary);

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
            System.load(cvJniLibrary.getAbsolutePath());
          } else {
            System.loadLibrary(opencvName);
          }
        } catch (IOException ex) {
          ex.printStackTrace();
          System.exit(1);
        }
      }
      cvLibraryLoaded = true;
    }
  }

  public static void ForceLoad() {}

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
  public static native int createCvSource(String name, int pixelFormat, int width, int height, int fps);

  //
  // Source Functions
  //
  public static native int getSourceKind(int source);
  public static native String getSourceName(int source);
  public static native String getSourceDescription(int source);
  public static native long getSourceLastFrameTime(int source);
  public static native boolean isSourceConnected(int source);
  public static native int getSourceProperty(int source, String name);
  public static native int[] enumerateSourceProperties(int source);
  public static native VideoMode getSourceVideoMode(int source);
  public static native boolean setSourceVideoMode(int source, int pixelFormat, int width, int height, int fps);
  public static native boolean setSourcePixelFormat(int source, int pixelFormat);
  public static native boolean setSourceResolution(int source, int width, int height);
  public static native boolean setSourceFPS(int source, int fps);
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
  public static native String getUsbCameraPath(int source);

  //
  // HttpCamera Source Functions
  //
  public static native int getHttpCameraKind(int source);
  public static native void setHttpCameraUrls(int source, String[] urls);
  public static native String[] getHttpCameraUrls(int source);

  //
  // OpenCV Source Functions
  //
  public static native void putSourceFrame(int source, long imageNativeObj);
  public static native void notifySourceError(int source, String msg);
  public static native void setSourceConnected(int source, boolean connected);
  public static native void setSourceDescription(int source, String description);
  public static native int createSourceProperty(int source, String name, int kind, int minimum, int maximum, int step, int defaultValue, int value);
  public static native void setSourceEnumPropertyChoices(int source, int property, String[] choices);

  //
  // Sink Creation Functions
  //
  public static native int createMjpegServer(String name, String listenAddress, int port);
  public static native int createCvSink(String name);
  //public static native int createCvSinkCallback(String name,
  //                            void (*processFrame)(long time));

  //
  // Sink Functions
  //
  public static native int getSinkKind(int sink);
  public static native String getSinkName(int sink);
  public static native String getSinkDescription(int sink);
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
  // OpenCV Sink Functions
  //
  public static native void setSinkDescription(int sink, String description);
  public static native long grabSinkFrame(int sink, long imageNativeObj);
  public static native long grabSinkFrameTimeout(int sink, long imageNativeObj, double timeout);
  public static native String getSinkError(int sink);
  public static native void setSinkEnabled(int sink, boolean enabled);

  //
  // Listener Functions
  //
  public static native int addListener(Consumer<VideoEvent> listener,
                                       int eventMask, boolean immediateNotify);

  public static native void removeListener(int handle);

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
}
