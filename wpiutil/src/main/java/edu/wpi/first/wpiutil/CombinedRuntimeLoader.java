/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpiutil;

import java.io.File;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Objects;

import com.fasterxml.jackson.core.type.TypeReference;
import com.fasterxml.jackson.databind.ObjectMapper;

public final class CombinedRuntimeLoader {
  private CombinedRuntimeLoader() {
  }

  public static native boolean addDllSearchDirectory(String directory);

  private static String getLoadErrorMessage(String libraryName, UnsatisfiedLinkError ule) {
    StringBuilder msg = new StringBuilder(512);
    msg.append(libraryName)
       .append(" could not be loaded from path\n"
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
   * Load a list of native libraries out of a single directory.
   * 
   * @param <T> The class where the resources would be located
   * @param clazz The actual class object
   * @param librariesToLoad List of libraries to load
   * @throws IOException Throws an IOException if not found
   */
  @SuppressWarnings({"PMD.AvoidInstantiatingObjectsInLoops", "PMD.UnnecessaryCastRule",
      "PMD.PreserveStackTrace", "PMD.CyclomaticComplexity"})
  public static <T> void loadLibraries(Class<T> clazz, String... librariesToLoad) 
      throws IOException {
    // Extract everything
    TypeReference<HashMap<String, Object>> typeRef = new TypeReference<HashMap<String, Object>>() {
    };
    ObjectMapper mapper = new ObjectMapper();
    Map<String, Object> map;
    try (var stream = clazz.getResourceAsStream("/ResourceInformation.json")) {
      map = mapper.readValue(stream, typeRef);
    }

    String hash = (String) map.get("hash");
    var platformPath = Paths.get(RuntimeDetector.getPlatformPath());
    var platform = platformPath.getName(0).toString();
    var arch = platformPath.getName(1).toString();

    var platformMap = (Map<String, List<String>>) map.get(platform);

    var fileList = platformMap.get(arch);

    var defaultExtractionRoot = RuntimeLoader.getDefaultExtractionRoot();
    var extractionPath = Paths.get(defaultExtractionRoot, platform, arch, hash);
    var extractionPathString = extractionPath.toString();

    List<String> extractedFiles = new ArrayList<>();

    byte[] buffer = new byte[0xFFFF]; // 64K copy buffer

    for (var file : fileList) {
      try (var stream = clazz.getResourceAsStream(file)) {
        Objects.requireNonNull(stream);
        
        var outputFile = Paths.get(extractionPathString, new File(file).getName());
        extractedFiles.add(outputFile.toString());
        if (outputFile.toFile().exists()) {
          continue;
        }
        outputFile.getParent().toFile().mkdirs();

        try (var os = Files.newOutputStream(outputFile)) {
          int readBytes;
          while ((readBytes = stream.read(buffer)) != -1) { // NOPMD
            os.write(buffer, 0, readBytes);
          }
        }
      }
    }

    String currentPath = "";

    try {
      if (RuntimeDetector.isWindows()) {
        // Load windows, set dll directory
        currentPath = Paths.get(extractionPathString, "WindowsLoaderHelper.dll").toString();
        System.load(currentPath);
        addDllSearchDirectory(extractionPathString);
      }

      for (var library : librariesToLoad) {
        for (var extractedFile : extractedFiles) {
          if (extractedFile.contains(library)) {
            // Load it
            currentPath = extractedFile;
            System.load(extractedFile);
          }
        }
      }
    } catch (UnsatisfiedLinkError ule) {
      throw new IOException(getLoadErrorMessage(currentPath, ule));
    }
  }
}
