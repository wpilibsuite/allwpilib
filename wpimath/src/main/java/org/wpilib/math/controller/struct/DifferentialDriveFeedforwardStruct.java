// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.controller.struct;

import edu.wpi.first.math.controller.DifferentialDriveFeedforward;
import edu.wpi.first.util.struct.Struct;
import java.nio.ByteBuffer;

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
    return kSizeDouble * 4;
  }

  @Override
  public String getSchema() {
    return "double kVLinear;double kALinear;double kVAngular;double kAAngular";
  }

  @Override
  public DifferentialDriveFeedforward unpack(ByteBuffer bb) {
    double kVLinear = bb.getDouble();
    double kALinear = bb.getDouble();
    double kVAngular = bb.getDouble();
    double kAAngular = bb.getDouble();
    return new DifferentialDriveFeedforward(kVLinear, kALinear, kVAngular, kAAngular);
  }

  @Override
  public void pack(ByteBuffer bb, DifferentialDriveFeedforward value) {
    bb.putDouble(value.m_kVLinear);
    bb.putDouble(value.m_kALinear);
    bb.putDouble(value.m_kVAngular);
    bb.putDouble(value.m_kAAngular);
  }
}
