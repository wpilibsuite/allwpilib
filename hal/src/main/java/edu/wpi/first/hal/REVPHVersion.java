// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

import edu.wpi.first.util.struct.Struct;
import edu.wpi.first.util.struct.StructSerializable;
import java.nio.ByteBuffer;

/** Version and device data received from a REV PH. */
@SuppressWarnings("MemberName")
public class REVPHVersion implements StructSerializable {
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

  public static final REVPHVersionStruct struct = new REVPHVersionStruct();

  public static final class REVPHVersionStruct implements Struct<REVPHVersion> {
    @Override
    public Class<REVPHVersion> getTypeClass() {
      return REVPHVersion.class;
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
      return "REVPHVersion";
    }

    @Override
    public void pack(ByteBuffer bb, REVPHVersion value) {
      bb.put((byte) value.firmwareMajor);
      bb.put((byte) value.firmwareMinor);
      bb.put((byte) value.firmwareFix);
      bb.put((byte) value.hardwareMinor);
      bb.put((byte) value.hardwareMajor);
      bb.putInt(value.uniqueId);
    }

    @Override
    public REVPHVersion unpack(ByteBuffer bb) {
      return new REVPHVersion(bb.get(), bb.get(), bb.get(), bb.get(), bb.get(), bb.getInt());
    }
  }
}
