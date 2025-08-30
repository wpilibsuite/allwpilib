// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.javacplugin;

import java.io.IOException;
import java.util.List;
import javax.tools.Diagnostic;
import javax.tools.JavaFileObject;

public final class CompileTestOptions {
  public static final int kJavaVersion = 17;
  public static final List<Object> kJavaVersionOptions =
      List.of("-source", kJavaVersion, "-target", kJavaVersion);

  private CompileTestOptions() {
    // Utility class
  }

  public static String getErrorSource(Diagnostic<? extends JavaFileObject> diagnostic) {
    try (var reader = diagnostic.getSource().openReader(true)) {
      int sourceLength = (int) (diagnostic.getEndPosition() - diagnostic.getStartPosition() + 1);
      char[] buf = new char[sourceLength];
      long skipCnt = reader.skip(diagnostic.getPosition());
      if (skipCnt != diagnostic.getPosition()) {
        // Didn't skip to the expected position; bail
        return "<unknown source>";
      }

      int readCnt = reader.read(buf);
      if (readCnt != sourceLength) {
        // Didn't read the expected length of text; bail
        return "<unknown source>";
      }

      return new String(buf);
    } catch (IOException e) {
      return "<unknown source>";
    }
  }
}
