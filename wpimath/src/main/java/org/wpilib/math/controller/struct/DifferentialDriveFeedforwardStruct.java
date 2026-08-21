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
    return "double V_LINEAR;double A_LINEAR;double V_ANGULAR;double A_ANGULAR";
  }

  @Override
  public DifferentialDriveFeedforward unpack(ByteBuffer bb) {
    double V_LINEAR = bb.getDouble();
    double A_LINEAR = bb.getDouble();
    double V_ANGULAR = bb.getDouble();
    double A_ANGULAR = bb.getDouble();
    return new DifferentialDriveFeedforward(V_LINEAR, A_LINEAR, V_ANGULAR, A_ANGULAR);
  }

  @Override
  public void pack(ByteBuffer bb, DifferentialDriveFeedforward value) {
    bb.putDouble(value.V_LINEAR);
    bb.putDouble(value.A_LINEAR);
    bb.putDouble(value.V_ANGULAR);
    bb.putDouble(value.A_ANGULAR);
  }
}
