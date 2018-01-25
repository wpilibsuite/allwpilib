/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.hal;

import edu.wpi.first.wpiutil.RuntimeDetector;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

/**
 * Base class for all JNI wrappers.
 */
public class JNIWrapper {
  static boolean libraryLoaded = false;
  static File jniLibrary = null;

  static {
    if (!libraryLoaded) {
      String jniFileName = "wpilibJNI";
      try {
        System.loadLibrary(jniFileName);
      } catch (UnsatisfiedLinkError ule) {
        try {
          String resname = RuntimeDetector.getLibraryResource(jniFileName);
          InputStream is = JNIWrapper.class.getResourceAsStream(resname);
          if (is != null) {
            // create temporary file
            if (System.getProperty("os.name").startsWith("Windows")) {
              jniLibrary = File.createTempFile(jniFileName, ".dll");
            } else if (System.getProperty("os.name").startsWith("Mac")) {
              jniLibrary = File.createTempFile(jniFileName, ".dylib");
            } else {
              jniLibrary = File.createTempFile(jniFileName, ".so");
            }
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
            System.loadLibrary(jniFileName);
          }
        } catch (IOException ex) {
          ex.printStackTrace();
          System.exit(1);
        }
      }
      libraryLoaded = true;
    }
  }

  public static native int getPortWithModule(byte module, byte channel);

  public static native int getPort(byte channel);
}
