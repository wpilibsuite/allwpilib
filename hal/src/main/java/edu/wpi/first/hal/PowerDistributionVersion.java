// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

public class PowerDistributionVersion {
  @SuppressWarnings("MemberName")
  public final int firmwareMajor;

  @SuppressWarnings("MemberName")
  public final int firmwareMinor;

  @SuppressWarnings("MemberName")
  public final int firmwareFix;

  @SuppressWarnings("MemberName")
  public final int hardwareMinor;

  @SuppressWarnings("MemberName")
  public final int hardwareMajor;

  @SuppressWarnings("MemberName")
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
