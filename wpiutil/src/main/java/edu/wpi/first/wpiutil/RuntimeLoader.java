package edu.wpi.first.wpiutil;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.nio.file.Paths;
import java.util.Scanner;

public final class RuntimeLoader<T> {
  private static String defaultExtractionRoot = null;
  public synchronized static String getDefaultExtractionRoot() {
    if (defaultExtractionRoot != null) {
      return defaultExtractionRoot;
    }
    String home = System.getProperty("user.home");
    defaultExtractionRoot = Paths.get(home, ".wpilib", "nativecache").toString();
    return defaultExtractionRoot;
  }

  private File jniLibrary = null;
  private String libraryName;
  private Class<T> loadClass;
  private String extractionRoot;



  public RuntimeLoader(String libraryName, String extractionRoot, Class<T> cls) {
    this.libraryName = libraryName;
    loadClass = cls;
    this.extractionRoot = extractionRoot;
  }

  public void LoadLibrary() throws IOException {
    try {
      // First, try loading path
      System.loadLibrary(libraryName);
      return;
    } catch (UnsatisfiedLinkError ule) {
      // Then load the hash from the resources
      String hashName = RuntimeDetector.getHashLibraryResource(libraryName);
      String resname = RuntimeDetector.getLibraryResource(libraryName);
      try (InputStream hashIs = loadClass.getResourceAsStream(hashName)) {
        if (hashIs == null) {
          throw new IOException(hashName + " Resource not found");
        }
        try (Scanner scanner = new Scanner(hashIs)) {
          String hash = scanner.nextLine();
          jniLibrary = new File(extractionRoot, resname + "." + hash);
          try {
            // Try to load from an already extracted hash
            System.load(jniLibrary.getAbsolutePath());
          } catch (UnsatisfiedLinkError ule2) {
            // If extraction failed, extract
            try (InputStream resIs = loadClass.getResourceAsStream(resname)) {
              if (resIs == null) {
                throw new IOException(resname + " Resource not found");
              }
              jniLibrary.getParentFile().mkdirs();
              try (OutputStream os = new FileOutputStream(jniLibrary)) {
                byte[] buffer = new byte[1024];
                int readBytes;
                while ((readBytes = resIs.read(buffer)) != -1) {
                  os.write(buffer, 0, readBytes);
                }
              }
              System.load(jniLibrary.getAbsolutePath());
            }
          }
        }
      }
    }
  }
}
