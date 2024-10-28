// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.controller.struct;

import edu.wpi.first.math.controller.ArmFeedforward;
import edu.wpi.first.util.struct.Struct;
import java.nio.ByteBuffer;

public class ArmFeedforwardStruct implements Struct<ArmFeedforward> {
  @Override
  public Class<ArmFeedforward> getTypeClass() {
    return ArmFeedforward.class;
  }

  @Override
  public String getTypeName() {
    return "ArmFeedforward";
  }

  @Override
  public int getSize() {
    return kSizeDouble * 5;
  }

  @Override
  public String getSchema() {
    return "double ks;double kg;double kv;double ka;double dt";
  }

  @Override
  public ArmFeedforward unpack(ByteBuffer bb) {
    double ks = bb.getDouble();
    double kg = bb.getDouble();
    double kv = bb.getDouble();
    double ka = bb.getDouble();
    double dt = bb.getDouble();
    return new ArmFeedforward(ks, kg, kv, ka, dt);
  }

  @Override
  public void pack(ByteBuffer bb, ArmFeedforward value) {
    bb.putDouble(value.getKs());
    bb.putDouble(value.getKg());
    bb.putDouble(value.getKv());
    bb.putDouble(value.getKa());
    bb.putDouble(value.getDt());
  }
}
