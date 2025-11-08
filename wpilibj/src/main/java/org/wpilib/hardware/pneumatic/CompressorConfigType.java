// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import edu.wpi.first.hal.REVPHJNI;

/** Compressor config type. */
public enum CompressorConfigType {
  /** Disabled. */
  Disabled(REVPHJNI.COMPRESSOR_CONFIG_TYPE_DISABLED),
  /** Digital. */
  Digital(REVPHJNI.COMPRESSOR_CONFIG_TYPE_DIGITAL),
  /** Analog. */
  Analog(REVPHJNI.COMPRESSOR_CONFIG_TYPE_ANALOG),
  /** Hybrid. */
  Hybrid(REVPHJNI.COMPRESSOR_CONFIG_TYPE_HYBRID);

  /** CompressorConfigType value. */
  public final int value;

  CompressorConfigType(int value) {
    this.value = value;
  }

  /**
   * Gets a type from an int value.
   *
   * @param value int value
   * @return type
   */
  public static CompressorConfigType fromValue(int value) {
    return switch (value) {
      case REVPHJNI.COMPRESSOR_CONFIG_TYPE_HYBRID -> Hybrid;
      case REVPHJNI.COMPRESSOR_CONFIG_TYPE_ANALOG -> Analog;
      case REVPHJNI.COMPRESSOR_CONFIG_TYPE_DIGITAL -> Digital;
      default -> Disabled;
    };
  }

  /**
   * Returns the CompressorConfigType's value.
   *
   * @return The CompressorConfigType's value.
   */
  public int getValue() {
    return value;
  }
}
