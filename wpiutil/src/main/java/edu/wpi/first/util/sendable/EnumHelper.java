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
   * Casts an integer to its corresponding enum value, defaulting if the ordinal
   * is out-of-bounds.
   *
   * @param ordinal The ordinal index of the enum value.
   * @param defaultValue Default value to use if index is invalid.
   * @param <E> Type of the enum.
   * @return Enum value.
   */
  public static <E extends Enum<?>> E enumFromOrdinal(int ordinal, E defaultValue) {
    E[] enumValues = (E[]) defaultValue.getClass().getEnumConstants();
    try {
      return enumValues[ordinal];
    } catch (Exception e) {
      return defaultValue;
    }
  }
}
