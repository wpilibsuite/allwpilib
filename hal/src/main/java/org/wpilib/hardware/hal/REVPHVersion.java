// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

/** Version and device data received from a REV PH. */
@SuppressWarnings("MemberName")
public class REVPHVersion {
  /** The firmware major version. */
  public final int firmwareMajor;

  /** The firmware minor version. */
  public final int firmwareMinor;

  /** The firmware fix version. */
  public final int firmwareFix;

  /** The hardware minor version. */
  public final int hardwareMinor;

  /** The hardware major version. */
  public final int hardwareMajor;

  /** The device's unique ID. */
  public final int uniqueId;

  /**
   * Constructs a revph version (Called from the HAL).
   *
   * @param firmwareMajor firmware major
   * @param firmwareMinor firmware minor
   * @param firmwareFix firmware fix
   * @param hardwareMinor hardware minor
   * @param hardwareMajor hardware major
   * @param uniqueId unique id
   */
  public REVPHVersion(
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
