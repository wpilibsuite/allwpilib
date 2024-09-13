// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

/** Power distribution version. */
@SuppressWarnings("MemberName")
public class PowerDistributionVersion {
  /** Firmware major version number. */
  public final int firmwareMajor;

  /** Firmware minor version number. */
  public final int firmwareMinor;

  /** Firmware fix version number. */
  public final int firmwareFix;

  /** Hardware minor version number. */
  public final int hardwareMinor;

  /** Hardware major version number. */
  public final int hardwareMajor;

  /** Unique ID. */
  public final int uniqueId;

  /**
   * Constructs a power distribution version (Called from the HAL).
   *
   * @param firmwareMajor firmware major
   * @param firmwareMinor firmware minor
   * @param firmwareFix firmware fix
   * @param hardwareMinor hardware minor
   * @param hardwareMajor hardware major
   * @param uniqueId unique id
   */
  public PowerDistributionVersion(
      int firmwareMajor,
      int firmwareMinor,
      int firmwareFix,
      int hardwareMinor,
      int hardwareMajor,
      int uniqueId) {
    this.firmwareMajor = firmwareMajor;
    this.firmwareMinor = firmwareMinor;
    this.firmwareFix = firmwareFix;
    this.hardwareMinor = hardwareMinor;
    this.hardwareMajor = hardwareMajor;
    this.uniqueId = uniqueId;
  }
}
