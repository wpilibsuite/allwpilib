// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.sendable;

import java.util.Arrays;

/**
 * Helper class for various enum functions.
 */
@SuppressWarnings("PMD.AbstractClassWithoutAbstractMethod")
public abstract class EnumHelper {
  /**
   * Casts a string to an enum, defaulting if the string does not match a valid enum value.
   *
   * @param value The string to cast to the enum.
   * @param defaultValue Default value to use if the cast fails.
   * @param <E> Type of the enum.
   * @return Casted enum value.
   */
  public static <E extends Enum<?>> E enumFromString(String value, E defaultValue) {
    E[] enumValues = (E[]) defaultValue.getClass().getEnumConstants();
    return Arrays.stream(enumValues)
        .filter(p -> p.name().equals(value))
        .findFirst()
        .orElse(defaultValue);
  }
}
