// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import edu.wpi.first.hal.REVPHJNI;

public enum CompressorConfigType {
  Disabled(REVPHJNI.COMPRESSOR_CONFIG_TYPE_DISABLED),
  Digital(REVPHJNI.COMPRESSOR_CONFIG_TYPE_DIGITAL),
  Analog(REVPHJNI.COMPRESSOR_CONFIG_TYPE_ANALOG),
  Hybrid(REVPHJNI.COMPRESSOR_CONFIG_TYPE_HYBRID);

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
    switch (value) {
      case REVPHJNI.COMPRESSOR_CONFIG_TYPE_HYBRID:
        return Hybrid;
      case REVPHJNI.COMPRESSOR_CONFIG_TYPE_ANALOG:
        return Analog;
      case REVPHJNI.COMPRESSOR_CONFIG_TYPE_DIGITAL:
        return Digital;
      default:
        return Disabled;
    }
  }

  public int getValue() {
    return value;
  }
}
