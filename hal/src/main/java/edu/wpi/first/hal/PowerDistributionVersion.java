// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

import edu.wpi.first.util.struct.Struct;
import edu.wpi.first.util.struct.StructSerializable;
import java.nio.ByteBuffer;

/** Power distribution version. */
@SuppressWarnings("MemberName")
public class PowerDistributionVersion implements StructSerializable {
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

  public static final PowerDistributionVersionStruct struct = new PowerDistributionVersionStruct();

  public static final class PowerDistributionVersionStruct
      implements Struct<PowerDistributionVersion> {
    @Override
    public Class<PowerDistributionVersion> getTypeClass() {
      return PowerDistributionVersion.class;
    }

    @Override
    public int getSize() {
      return kSizeInt32 + (kSizeInt8 * 5);
    }

    @Override
    public String getSchema() {
      return "uint8 firmwareMajor; "
          + "uint8 firmwareMinor; "
          + "uint8 firmwareFix; "
          + "uint8 hardwareMinor; "
          + "uint8 hardwareMajor; "
          + "int32 uniqueId;";
    }

    @Override
    public String getTypeName() {
      return "PowerDistributionVersion";
    }

    @Override
    public void pack(ByteBuffer bb, PowerDistributionVersion value) {
      bb.put((byte) value.firmwareMajor);
      bb.put((byte) value.firmwareMinor);
      bb.put((byte) value.firmwareFix);
      bb.put((byte) value.hardwareMinor);
      bb.put((byte) value.hardwareMajor);
      bb.putInt(value.uniqueId);
    }

    @Override
    public PowerDistributionVersion unpack(ByteBuffer bb) {
      return new PowerDistributionVersion(
          bb.get(), bb.get(), bb.get(), bb.get(), bb.get(), bb.getInt());
    }
  }
}
