/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.hal;

import java.io.File;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.FileOutputStream;

//
// base class for all JNI wrappers
//
public class JNIWrapper {
  static boolean libraryLoaded = false;
  static File jniLibrary = null;
  static {
    try {
      if (!libraryLoaded) {
        InputStream is = JNIWrapper.class.getResourceAsStream("/linux-arm/libwpilibJavaJNI.so");
        if (is != null) {
          // create temporary file
          jniLibrary = File.createTempFile("libwpilibJavaJNI", ".so");
          // flag for delete on exit
          jniLibrary.deleteOnExit();

          byte[] buffer = new byte[1024];

          int readBytes;


          OutputStream os = new FileOutputStream(jniLibrary);

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
          System.loadLibrary("wpilibJavaJNI");
        }

        libraryLoaded = true;
      }

    } catch (Exception ex) {
      ex.printStackTrace();
      System.exit(1);
    }
  }

  public static native long getPortWithModule(byte module, byte pin);

  public static native long getPort(byte pin);

  public static native void freePort(long port_pointer);
}
