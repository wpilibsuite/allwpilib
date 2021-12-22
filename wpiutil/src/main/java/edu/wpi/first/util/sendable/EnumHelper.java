// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.sendable;

import java.util.Arrays;

public class EnumHelper {
  public static <E extends Enum<?>> E enumFromString(String value, E defaultValue) {
    E[] enumValues = (E[]) defaultValue.getClass().getEnumConstants();
    return Arrays.stream(enumValues)
        .filter(p -> p.name().equals(value))
        .findFirst()
        .orElse(defaultValue);
  }
}
