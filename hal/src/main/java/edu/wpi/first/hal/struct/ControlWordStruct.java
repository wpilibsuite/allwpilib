// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal.struct;

import edu.wpi.first.hal.ControlWord;
import edu.wpi.first.hal.RobotMode;
import edu.wpi.first.util.struct.Struct;
import java.nio.ByteBuffer;

public class ControlWordStruct implements Struct<ControlWord> {
  @Override
  public Class<ControlWord> getTypeClass() {
    return ControlWord.class;
  }

  @Override
  public String getTypeName() {
    return "ControlWord";
  }

  @Override
  public int getSize() {
    return 1;
  }

  @Override
  public String getSchema() {
    return "bool enabled:1;"
        + "enum{unknown=0,autonomous=1,teleoperated=2,test=3}int8 robotMode:2;"
        + "bool eStop:1;bool fmsAttached:1;bool dsAttached:1;";
  }

  @Override
  public ControlWord unpack(ByteBuffer bb) {
    ControlWord word = new ControlWord();
    unpackInto(word, bb);
    return word;
  }

  @Override
  public void unpackInto(ControlWord out, ByteBuffer bb) {
    byte b = bb.get();
    boolean enabled = (b & 0x01) != 0;
    RobotMode mode = RobotMode.fromInt((b & 0x06) >> 1);
    boolean eStop = (b & 0x08) != 0;
    boolean fmsAttached = (b & 0x10) != 0;
    boolean dsAttached = (b & 0x20) != 0;
    out.update(enabled, mode, eStop, fmsAttached, dsAttached);
  }

  @Override
  public void pack(ByteBuffer bb, ControlWord value) {
    bb.put(
        (byte)
            ((value.getEnabled() ? 0x01 : 0)
                | (value.getMode().getValue() << 1)
                | (value.getEStop() ? 0x08 : 0)
                | (value.getFMSAttached() ? 0x10 : 0)
                | (value.getDSAttached() ? 0x20 : 0)));
  }
}
