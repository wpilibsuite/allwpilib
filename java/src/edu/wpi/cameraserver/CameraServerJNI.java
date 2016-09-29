/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.cameraserver;

import java.io.File;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;

public class CameraServerJNI {
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
        //System.out.println("platform: " + resname);
        if (osname.startsWith("Windows"))
          resname += "cameraserver.dll";
        else if (osname.startsWith("Mac"))
          resname += "libcameraserver.dylib";
        else
          resname += "libcameraserver.so";
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
          try {
            System.load(jniLibrary.getAbsolutePath());
          } catch (UnsatisfiedLinkError e) {
            System.loadLibrary("cameraserver");
          }
        } else {
          System.loadLibrary("cameraserver");
        }
      } catch (IOException ex) {
        ex.printStackTrace();
        System.exit(1);
      }
      libraryLoaded = true;
    }
  }

  //
  // Property Functions
  //
  public static native int getPropertyType(int property);
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
  public static native int createUSBSourceDev(String name, int dev);
  public static native int createUSBSourcePath(String name, String path);
  public static native int createHTTPSource(String name, String url);
  public static native int createCvSource(String name, int pixelFormat, int width, int height, int fps);

  //
  // Source Functions
  //
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
  public static native int copySource(int source);
  public static native void releaseSource(int source);

  //
  // OpenCV Source Functions
  //
  //public static native void putSourceFrame(int source, CvMat image);
  public static native void notifySourceError(int source, String msg);
  public static native void setSourceConnected(int source, boolean connected);
  public static native int createSourceProperty(int source, String name, int type);
  //public static native int createSourcePropertyCallback(int source, String name,
  //                                    int type,
  //                                    void (*onChange)(String name,
  //                                                     int property));
  public static native void removeSourceProperty(int source, int property);
  public static native void removeSourcePropertyByName(int source, String name);

  //
  // Sink Creation Functions
  //
  public static native int createHTTPSink(String name, String listenAddress, int port);
  public static native int createCvSink(String name);
  //public static native int createCvSinkCallback(String name,
  //                            void (*processFrame)(long time));

  //
  // Sink Functions
  //
  public static native String getSinkName(int sink);
  public static native String getSinkDescription(int sink);
  public static native void setSinkSource(int sink, int source);
  public static native int getSinkSourceProperty(int sink, String name);
  public static native int getSinkSource(int sink);
  public static native int copySink(int sink);
  public static native void releaseSink(int sink);

  //
  // OpenCV Sink Functions
  //
  //public static native int grabSinkFrame(int sink, CvMat image);
  public static native String getSinkError(int sink);
  public static native void setSinkEnabled(int sink, boolean enabled);

  //
  // Listener Functions
  //
  //public static native int addSourceListener(void (*callback)(String name, int source,
  //                                          int event),
  //                         int eventMask);

  public static native void removeSourceListener(int handle);

  //public static native int addSinkListener(void (*callback)(String name, int sink, int event),
  //                       int eventMask);

  public static native void removeSinkListener(int handle);

  //
  // Utility Functions
  //
  public static native USBCameraInfo[] enumerateUSBCameras();

  public static native int[] enumerateSources();

  public static native int[] enumerateSinks();
}
