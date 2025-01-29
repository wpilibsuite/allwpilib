// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.hardware.hal.struct;

import java.nio.ByteBuffer;
import org.wpilib.hardware.hal.ControlWord;
import org.wpilib.util.struct.Struct;

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
    return 8;
  }

  @Override
  public String getSchema() {
    return "uint64 opModeHash:56;"
        + "enum{unknown=0,autonomous=1,teleoperated=2,test=3} uint64 robotMode:2;"
        + "bool enabled:1;bool eStop:1;bool fmsAttached:1;bool dsAttached:1;";
  }

  @Override
  public ControlWord unpack(ByteBuffer bb) {
    ControlWord word = new ControlWord();
    unpackInto(word, bb);
    return word;
  }

  @Override
  public void unpackInto(ControlWord out, ByteBuffer bb) {
    out.update(bb.getLong());
  }

  @Override
  public void pack(ByteBuffer bb, ControlWord value) {
    bb.putLong(value.getNative());
  }
}
