// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.NoSuchFileException;
import java.nio.file.Paths;

/**
 * Loads classes by name. Can be used at any time, but is most commonly used to preload classes at
 * the start of the program to avoid unpredictable delays due to lazy classloading later in program
 * execution.
 */
public final class ClassPreloader {
  private ClassPreloader() {}

  /**
   * Loads classes from an iterable.
   *
   * @param classNames iterable of class names
   * @return Number of classes loaded.
   */
  public static int preload(Iterable<String> classNames) {
    int count = 0;
    for (String i : classNames) {
      try {
        Class.forName(i);
        count++;
      } catch (ClassNotFoundException e) {
        System.out.println("Could not preload " + i);
      }
    }
    return count;
  }

  /**
   * Loads classes.
   *
   * @param classNames array of class names
   * @return Number of classes loaded.
   */
  public static int preload(String... classNames) {
    int count = 0;
    for (String i : classNames) {
      try {
        Class.forName(i);
        count++;
      } catch (ClassNotFoundException e) {
        System.out.println("Could not preload " + i);
      }
    }
    return count;
  }

  /**
   * Loads classes from a buffered reader. The input is expected to be one class name per line.
   * Blank lines and lines starting with a semicolon are ignored.
   *
   * @param reader Reader
   * @return Number of classes loaded.
   */
  public static int preload(BufferedReader reader) {
    int count = 0;
    try {
      String line = reader.readLine();
      while (line != null) {
        if (!line.isEmpty() && !line.startsWith(";")) {
          try {
            Class.forName(line);
            count++;
          } catch (ClassNotFoundException e) {
            System.out.println("Could not preload " + line);
          }
        }
        line = reader.readLine();
      }
    } catch (IOException e) {
      System.out.println("Error when reading preload file: " + e);
    }
    return count;
  }

  /**
   * Loads classes from an input stream. The input is expected to be one class name per line. Blank
   * lines and lines starting with a semicolon are ignored.
   *
   * @param stream input stream
   * @return Number of classes loaded.
   */
  public static int preload(InputStream stream) {
    return preload(new BufferedReader(new InputStreamReader(stream, StandardCharsets.UTF_8)));
  }

  /**
   * Loads classes from a file. The input is expected to be one class name per line. Blank lines and
   * lines starting with a semicolon are ignored.
   *
   * @param filename filename
   * @return Number of classes loaded.
   */
  public static int preloadFile(String filename) {
    try (BufferedReader reader =
        Files.newBufferedReader(Paths.get(filename), StandardCharsets.UTF_8)) {
      return preload(reader);
    } catch (NoSuchFileException e) {
      System.out.println("Could not open preload file " + filename + ": " + e);
    } catch (IOException e) {
      System.out.println("Could not close preload file " + filename + ": " + e);
    }
    return 0;
  }
}
