// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.controller.struct;

import java.nio.ByteBuffer;
import org.wpilib.math.controller.DifferentialDriveFeedforward;
import org.wpilib.util.struct.Struct;

public final class DifferentialDriveFeedforwardStruct
    implements Struct<DifferentialDriveFeedforward> {
  @Override
  public Class<DifferentialDriveFeedforward> getTypeClass() {
    return DifferentialDriveFeedforward.class;
  }

  @Override
  public String getTypeName() {
    return "DifferentialDriveFeedforward";
  }

  @Override
  public int getSize() {
    return DOUBLE_SIZE * 4;
  }

  @Override
  public String getSchema() {
    return "double kvLinear;double kaLinear;double kvAngular;double kaAngular";
  }

  @Override
  public DifferentialDriveFeedforward unpack(ByteBuffer bb) {
    double kvLinear = bb.getDouble();
    double kaLinear = bb.getDouble();
    double kvAngular = bb.getDouble();
    double kaAngular = bb.getDouble();
    return new DifferentialDriveFeedforward(kvLinear, kaLinear, kvAngular, kaAngular);
  }

  @Override
  public void pack(ByteBuffer bb, DifferentialDriveFeedforward value) {
    bb.putDouble(value.kvLinear);
    bb.putDouble(value.kaLinear);
    bb.putDouble(value.kvAngular);
    bb.putDouble(value.kaAngular);
  }
}
