/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpiutil;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.Scanner;

public final class RuntimeLoader<T> {
  private static String defaultExtractionRoot;
  public synchronized static String getDefaultExtractionRoot() {
    if (defaultExtractionRoot != null) {
      return defaultExtractionRoot;
    }
    String home = System.getProperty("user.home");
    defaultExtractionRoot = Paths.get(home, ".wpilib", "nativecache").toString();
    return defaultExtractionRoot;
  }

  private File jniLibrary; // NOPMD
  private final String libraryName;
  private final Class<T> loadClass;
  private final String extractionRoot;



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
          throw new IOException(hashName + " Resource not found"); // NOPMD
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
                throw new IOException(resname + " Resource not found"); // NOPMD
              }
              jniLibrary.getParentFile().mkdirs();
              Files.newOutputStream(jniLibrary.toPath());
              try (OutputStream os = Files.newOutputStream(jniLibrary.toPath())) {
                byte[] buffer = new byte[1024];
                int readBytes;
                while ((readBytes = resIs.read(buffer)) != -1) { // NOPMD
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
