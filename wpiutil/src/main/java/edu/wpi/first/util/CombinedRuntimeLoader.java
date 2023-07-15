// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util;

import com.fasterxml.jackson.core.type.TypeReference;
import com.fasterxml.jackson.databind.ObjectMapper;
import java.io.File;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Objects;

public final class CombinedRuntimeLoader {
  private CombinedRuntimeLoader() {}

  private static String extractionDirectory;

  public static synchronized String getExtractionDirectory() {
    return extractionDirectory;
  }

  private static synchronized void setExtractionDirectory(String directory) {
    extractionDirectory = directory;
  }

  public static native String setDllDirectory(String directory);

  private static String getLoadErrorMessage(String libraryName, UnsatisfiedLinkError ule) {
    StringBuilder msg = new StringBuilder(512);
    msg.append(libraryName)
        .append(" could not be loaded from path\n" + "\tattempted to load for platform ")
        .append(RuntimeDetector.getPlatformPath())
        .append("\nLast Load Error: \n")
        .append(ule.getMessage())
        .append('\n');
    if (RuntimeDetector.isWindows()) {
      msg.append(
          "A common cause of this error is missing the C++ runtime.\n"
              + "Download the latest at https://support.microsoft.com/en-us/help/2977003/the-latest-supported-visual-c-downloads\n");
    }
    return msg.toString();
  }

  /**
   * Extract a list of native libraries.
   *
   * @param <T> The class where the resources would be located
   * @param clazz The actual class object
   * @param resourceName The resource name on the classpath to use for file lookup
   * @return List of all libraries that were extracted
   * @throws IOException Thrown if resource not found or file could not be extracted
   */
  @SuppressWarnings("unchecked")
  public static <T> List<String> extractLibraries(Class<T> clazz, String resourceName)
      throws IOException {
    TypeReference<HashMap<String, Object>> typeRef =
        new TypeReference<HashMap<String, Object>>() {};
    ObjectMapper mapper = new ObjectMapper();
    Map<String, Object> map;
    try (var stream = clazz.getResourceAsStream(resourceName)) {
      map = mapper.readValue(stream, typeRef);
    }

    var platformPath = Paths.get(RuntimeDetector.getPlatformPath());
    var platform = platformPath.getName(0).toString();
    var arch = platformPath.getName(1).toString();

    var platformMap = (Map<String, List<String>>) map.get(platform);

    var fileList = platformMap.get(arch);

    var extractionPathString = getExtractionDirectory();

    if (extractionPathString == null) {
      String hash = (String) map.get("hash");

      var defaultExtractionRoot = RuntimeLoader.getDefaultExtractionRoot();
      var extractionPath = Paths.get(defaultExtractionRoot, platform, arch, hash);
      extractionPathString = extractionPath.toString();

      setExtractionDirectory(extractionPathString);
    }

    List<String> extractedFiles = new ArrayList<>();

    byte[] buffer = new byte[0x10000]; // 64K copy buffer

    for (var file : fileList) {
      try (var stream = clazz.getResourceAsStream(file)) {
        Objects.requireNonNull(stream);

        var outputFile = Paths.get(extractionPathString, new File(file).getName());
        extractedFiles.add(outputFile.toString());
        if (outputFile.toFile().exists()) {
          continue;
        }
        var parent = outputFile.getParent();
        if (parent == null) {
          throw new IOException("Output file has no parent");
        }
        parent.toFile().mkdirs();

        try (var os = Files.newOutputStream(outputFile)) {
          int readBytes;
          while ((readBytes = stream.read(buffer)) != -1) { // NOPMD
            os.write(buffer, 0, readBytes);
          }
        }
      }
    }

    return extractedFiles;
  }

  /**
   * Load a single library from a list of extracted files.
   *
   * @param libraryName The library name to load
   * @param extractedFiles The extracted files to search
   * @throws IOException If library was not found
   */
  public static void loadLibrary(String libraryName, List<String> extractedFiles)
      throws IOException {
    String currentPath = null;
    String oldDllDirectory = null;
    try {
      if (RuntimeDetector.isWindows()) {
        var extractionPathString = getExtractionDirectory();
        oldDllDirectory = setDllDirectory(extractionPathString);
      }
      for (var extractedFile : extractedFiles) {
        if (extractedFile.contains(libraryName)) {
          // Load it
          currentPath = extractedFile;
          System.load(extractedFile);
          return;
        }
      }
      throw new IOException("Could not find library " + libraryName);
    } catch (UnsatisfiedLinkError ule) {
      throw new IOException(getLoadErrorMessage(currentPath, ule));
    } finally {
      if (oldDllDirectory != null) {
        setDllDirectory(oldDllDirectory);
      }
    }
  }

  /**
   * Load a list of native libraries out of a single directory.
   *
   * @param <T> The class where the resources would be located
   * @param clazz The actual class object
   * @param librariesToLoad List of libraries to load
   * @throws IOException Throws an IOException if not found
   */
  public static <T> void loadLibraries(Class<T> clazz, String... librariesToLoad)
      throws IOException {
    // Extract everything

    var extractedFiles = extractLibraries(clazz, "/ResourceInformation.json");

    String currentPath = "";

    try {
      if (RuntimeDetector.isWindows()) {
        var extractionPathString = getExtractionDirectory();
        // Load windows, set dll directory
        currentPath = Paths.get(extractionPathString, "WindowsLoaderHelper.dll").toString();
        System.load(currentPath);
      }
    } catch (UnsatisfiedLinkError ule) {
      throw new IOException(getLoadErrorMessage(currentPath, ule));
    }

    for (var library : librariesToLoad) {
      loadLibrary(library, extractedFiles);
    }
  }
}
