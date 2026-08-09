// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.hardware.hal;

/** Firmware and hardware version data received from a FIRST A301. */
public class A301FirmwareVersion {
  public final int major;
  public final int minor;
  public final int patch;
  public final int prerelease;
  public final int hardwareRevision;
  public final int raw;

  /**
   * Constructs A301 version data (called from the HAL).
   *
   * @param major major firmware version
   * @param minor minor firmware version
   * @param patch patch firmware version
   * @param prerelease prerelease identifier
   * @param hardwareRevision hardware revision
   * @param raw packed firmware version
   */
  public A301FirmwareVersion(
      int major, int minor, int patch, int prerelease, int hardwareRevision, int raw) {
    this.major = major;
    this.minor = minor;
    this.patch = patch;
    this.prerelease = prerelease;
    this.hardwareRevision = hardwareRevision;
    this.raw = raw;
  }
}
