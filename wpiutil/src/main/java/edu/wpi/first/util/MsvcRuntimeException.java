// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util;

/** Exception thrown due to a bad MSVC Runtime. */
public class MsvcRuntimeException extends RuntimeException {
  private static final long serialVersionUID = -9155939328084105142L;

  private static String generateMessage(
      int foundMajor, int foundMinor, int expectedMajor, int expectedMinor, String runtimePath) {
    String jvmLocation = ProcessHandle.current().info().command().orElse("Unknown");

    StringBuilder builder = new StringBuilder(100);
    builder
        .append("Invalid MSVC Runtime Detected.\n")
        .append(
            String.format(
                "Expected at least %d.%d, but found %d.%d\n",
                expectedMajor, expectedMinor, foundMajor, foundMinor))
        .append(String.format("JVM Location: %s\n", jvmLocation))
        .append(String.format("Runtime DLL Location: %s\n", runtimePath))
        .append("See https://wpilib.org/jvmruntime for more information\n");

    return builder.toString();
  }

  /**
   * Constructs a runtime exception.
   *
   * @param foundMajor found major
   * @param foundMinor found minor
   * @param expectedMajor expected major
   * @param expectedMinor expected minor
   * @param runtimePath path of runtime
   */
  public MsvcRuntimeException(
      int foundMajor, int foundMinor, int expectedMajor, int expectedMinor, String runtimePath) {
    super(generateMessage(foundMajor, foundMinor, expectedMajor, expectedMinor, runtimePath));
  }

  /**
   * Constructs a runtime exception.
   *
   * @param msg message
   */
  public MsvcRuntimeException(String msg) {
    super(msg);
  }
}
