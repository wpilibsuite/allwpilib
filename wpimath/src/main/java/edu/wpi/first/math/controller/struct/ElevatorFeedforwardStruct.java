// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.controller.struct;

import edu.wpi.first.math.controller.ElevatorFeedforward;
import edu.wpi.first.util.struct.Struct;
import java.nio.ByteBuffer;

public class ElevatorFeedforwardStruct implements Struct<ElevatorFeedforward> {
  @Override
  public Class<ElevatorFeedforward> getTypeClass() {
    return ElevatorFeedforward.class;
  }

  @Override
  public String getTypeString() {
    return "struct:ElevatorFeedforward";
  }

  @Override
  public int getSize() {
    return kSizeDouble * 4;
  }

  @Override
  public String getSchema() {
    return "double ks;double kg;double kv;double ka";
  }

  @Override
  public ElevatorFeedforward unpack(ByteBuffer bb) {
    double ks = bb.getDouble();
    double kg = bb.getDouble();
    double kv = bb.getDouble();
    double ka = bb.getDouble();
    return new ElevatorFeedforward(ks, kg, kv, ka);
  }

  @Override
  public void pack(ByteBuffer bb, ElevatorFeedforward value) {
    bb.putDouble(value.ks);
    bb.putDouble(value.kg);
    bb.putDouble(value.kv);
    bb.putDouble(value.ka);
  }
}
