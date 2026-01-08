// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

import edu.wpi.first.util.struct.Struct;
import edu.wpi.first.util.struct.StructSerializable;
import java.nio.ByteBuffer;

/** Sticky faults for a REV PH. These faults will remain active until they are reset by the user. */
@SuppressWarnings("MemberName")
public class REVPHStickyFaults implements StructSerializable {
  // Encoding and decoding based on the bitfield and not the boolean fields
  // introduces some invariance concerns with the promise this class makes.
  // From now on the bitfield is the absolute source of truth for the state of
  // the object.
  /** The faults bitfield the object was constructed with. */
  public final int m_bitfield;

  /** An overcurrent event occurred on the compressor output. */
  public final boolean CompressorOverCurrent;

  /** The compressor output has an open circuit. */
  public final boolean CompressorOpen;

  /** An overcurrent event occurred on a solenoid output. */
  public final boolean SolenoidOverCurrent;

  /** The input voltage is below the minimum voltage. */
  public final boolean Brownout;

  /** A warning was raised by the device's CAN controller. */
  public final boolean CanWarning;

  /** The device's CAN controller experienced a "Bus Off" event. */
  public final boolean CanBusOff;

  /** The hardware on the device has malfunctioned. */
  public final boolean HardwareFault;

  /** The firmware on the device has malfunctioned. */
  public final boolean FirmwareFault;

  /** The device has rebooted. */
  public final boolean HasReset;

  /**
   * Called from HAL.
   *
   * @param faults sticky fault bit mask
   */
  public REVPHStickyFaults(int faults) {
    m_bitfield = faults;
    CompressorOverCurrent = (faults & 0x1) != 0;
    CompressorOpen = (faults & 0x2) != 0;
    SolenoidOverCurrent = (faults & 0x4) != 0;
    Brownout = (faults & 0x8) != 0;
    CanWarning = (faults & 0x10) != 0;
    CanBusOff = (faults & 0x20) != 0;
    HardwareFault = (faults & 0x40) != 0;
    FirmwareFault = (faults & 0x80) != 0;
    HasReset = (faults & 0x100) != 0;
  }

  public static final REVPHStickyFaultsStruct struct = new REVPHStickyFaultsStruct();

  public static final class REVPHStickyFaultsStruct implements Struct<REVPHStickyFaults> {
    @Override
    public Class<REVPHStickyFaults> getTypeClass() {
      return REVPHStickyFaults.class;
    }

    @Override
    public int getSize() {
      return kSizeInt32;
    }

    @Override
    public String getSchema() {
      return "bool compressorOverCurrent:1 "
          + "bool compressorOpen:1 "
          + "bool solenoidOverCurrent:1 "
          + "bool brownout:1; "
          + "bool canWarning:1; "
          + "bool canbusOff:1; "
          + "bool hardwareFault:1;"
          + "bool firmwareFault:1; "
          + "bool hasReset:1; ";
    }

    @Override
    public String getTypeName() {
      return "REVPHStickyFaults";
    }

    @Override
    public void pack(ByteBuffer bb, REVPHStickyFaults value) {
      bb.putInt(value.m_bitfield);
    }

    public void pack(ByteBuffer bb, int value) {
      bb.putInt(value);
    }

    @Override
    public REVPHStickyFaults unpack(ByteBuffer bb) {
      int packed = bb.getInt();
      return new REVPHStickyFaults(packed);
    }
  }
}
