/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpiutil;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.security.DigestInputStream;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.Locale;
import java.util.Scanner;

public final class RuntimeLoader<T> {
  private static String defaultExtractionRoot;

  /**
   * Gets the default extration root location (~/.wpilib/nativecache).
   */
  public static synchronized String getDefaultExtractionRoot() {
    if (defaultExtractionRoot != null) {
      return defaultExtractionRoot;
    }
    String home = System.getProperty("user.home");
    defaultExtractionRoot = Paths.get(home, ".wpilib", "nativecache").toString();
    return defaultExtractionRoot;
  }

  private final String m_libraryName;
  private final Class<T> m_loadClass;
  private final String m_extractionRoot;

  /**
   * Creates a new library loader.
   *
   * <p>Resources loaded on disk from extractionRoot, and from classpath from the
   * passed in class. Library name is the passed in name.
   */
  public RuntimeLoader(String libraryName, String extractionRoot, Class<T> cls) {
    m_libraryName = libraryName;
    m_loadClass = cls;
    m_extractionRoot = extractionRoot;
  }

  private String getLoadErrorMessage(UnsatisfiedLinkError ule) {
    StringBuilder msg = new StringBuilder(512);
    msg.append(m_libraryName)
       .append(" could not be loaded from path or an embedded resource.\n"
               + "\tattempted to load for platform ")
       .append(RuntimeDetector.getPlatformPath())
       .append("\nLast Load Error: \n")
       .append(ule.getMessage())
       .append('\n');
    if (RuntimeDetector.isWindows()) {
      msg.append("A common cause of this error is missing the C++ runtime.\n"
                 + "Download the latest at https://support.microsoft.com/en-us/help/2977003/the-latest-supported-visual-c-downloads\n");
    }
    return msg.toString();
  }

  /**
   * Loads a native library.
   */
  @SuppressWarnings("PMD.PreserveStackTrace")
  public void loadLibrary() throws IOException {
    try {
      // First, try loading path
      System.loadLibrary(m_libraryName);
    } catch (UnsatisfiedLinkError ule) {
      // Then load the hash from the resources
      String hashName = RuntimeDetector.getHashLibraryResource(m_libraryName);
      String resname = RuntimeDetector.getLibraryResource(m_libraryName);
      try (InputStream hashIs = m_loadClass.getResourceAsStream(hashName)) {
        if (hashIs == null) {
          throw new IOException(getLoadErrorMessage(ule));
        }
        try (Scanner scanner = new Scanner(hashIs, StandardCharsets.UTF_8.name())) {
          String hash = scanner.nextLine();
          File jniLibrary = new File(m_extractionRoot, resname + "." + hash);
          try {
            // Try to load from an already extracted hash
            System.load(jniLibrary.getAbsolutePath());
          } catch (UnsatisfiedLinkError ule2) {
            // If extraction failed, extract
            try (InputStream resIs = m_loadClass.getResourceAsStream(resname)) {
              if (resIs == null) {
                throw new IOException(getLoadErrorMessage(ule));
              }
              jniLibrary.getParentFile().mkdirs();
              try (OutputStream os = Files.newOutputStream(jniLibrary.toPath())) {
                byte[] buffer = new byte[0xFFFF]; // 64K copy buffer
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

  /**
   * Load a native library by directly hashing the file.
   */
  @SuppressWarnings({"PMD.NPathComplexity", "PMD.PreserveStackTrace", "PMD.EmptyWhileStmt",
                     "PMD.AvoidThrowingRawExceptionTypes", "PMD.CyclomaticComplexity"})
  public void loadLibraryHashed() throws IOException {
    try {
      // First, try loading path
      System.loadLibrary(m_libraryName);
    } catch (UnsatisfiedLinkError ule) {
      // Then load the hash from the input file
      String resname = RuntimeDetector.getLibraryResource(m_libraryName);
      String hash = null;
      try (InputStream is = m_loadClass.getResourceAsStream(resname)) {
        if (is == null) {
          throw new IOException(getLoadErrorMessage(ule));
        }
        MessageDigest md = null;
        try {
          md = MessageDigest.getInstance("MD5");
        } catch (NoSuchAlgorithmException nsae) {
          throw new RuntimeException("Weird Hash Algorithm?");
        }
        try (DigestInputStream dis = new DigestInputStream(is, md)) {
          // Read the entire buffer once to hash
          byte[] buffer = new byte[0xFFFF];
          while (dis.read(buffer) > -1) {}
          MessageDigest digest = dis.getMessageDigest();
          byte[] digestOutput = digest.digest();
          StringBuilder builder = new StringBuilder();
          for (byte b : digestOutput) {
            builder.append(String.format("%02X", b));
          }
          hash = builder.toString().toLowerCase(Locale.ENGLISH);
        }
      }
      if (hash == null) {
        throw new IOException("Weird Hash?");
      }
      File jniLibrary = new File(m_extractionRoot, resname + "." + hash);
      try {
        // Try to load from an already extracted hash
        System.load(jniLibrary.getAbsolutePath());
      } catch (UnsatisfiedLinkError ule2) {
        // If extraction failed, extract
        try (InputStream resIs = m_loadClass.getResourceAsStream(resname)) {
          if (resIs == null) {
            throw new IOException(getLoadErrorMessage(ule));
          }
          jniLibrary.getParentFile().mkdirs();
          try (OutputStream os = Files.newOutputStream(jniLibrary.toPath())) {
            byte[] buffer = new byte[0xFFFF]; // 64K copy buffer
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
