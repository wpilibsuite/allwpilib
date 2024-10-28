// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util;

public class MsvcRuntimeException extends RuntimeException {
    private static final long serialVersionUID = -9155939328084105142L;

    private static String generateMessage(int foundMajor, int foundMinor, int expectedMajor, int expectedMinor,
            String runtimePath) {
        String jvmLocation = ProcessHandle.current().info().command().orElse("Unknown");

        StringBuffer builder = new StringBuffer();
        builder.append("Invalid MSVC Runtime Detected.\n");
        builder.append(String.format("Expected at least %d.%d, but found %d.%d\n", expectedMajor, expectedMinor,
                foundMajor, foundMinor));
        builder.append(String.format("JVM Location: %s\n", jvmLocation));
        builder.append(String.format("Runtime DLL Location: %s\n", runtimePath));
        builder.append(String.format("See https://wpilib.org/jvmruntime for more information\n", runtimePath));

        return builder.toString();
    }

    public MsvcRuntimeException(int foundMajor, int foundMinor, int expectedMajor, int expectedMinor,
            String runtimePath) {
        super(generateMessage(foundMajor, foundMinor, expectedMajor, expectedMinor, runtimePath));
    }

    public MsvcRuntimeException(String msg) {
        super(msg);
    }
}
