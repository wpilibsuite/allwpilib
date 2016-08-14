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

}
