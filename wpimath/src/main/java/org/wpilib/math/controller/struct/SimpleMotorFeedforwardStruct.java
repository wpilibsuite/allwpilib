// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.controller.struct;

import edu.wpi.first.math.controller.SimpleMotorFeedforward;
import edu.wpi.first.util.struct.Struct;
import java.nio.ByteBuffer;

public final class SimpleMotorFeedforwardStruct implements Struct<SimpleMotorFeedforward> {
  @Override
  public Class<SimpleMotorFeedforward> getTypeClass() {
    return SimpleMotorFeedforward.class;
  }

  @Override
  public String getTypeName() {
    return "SimpleMotorFeedforward";
  }

  @Override
  public int getSize() {
    return kSizeDouble * 4;
  }

  @Override
  public String getSchema() {
    return "double ks;double kv;double ka;double dt";
  }

  @Override
  public SimpleMotorFeedforward unpack(ByteBuffer bb) {
    double ks = bb.getDouble();
    double kv = bb.getDouble();
    double ka = bb.getDouble();
    double dt = bb.getDouble();
    return new SimpleMotorFeedforward(ks, kv, ka, dt);
  }

  @Override
  public void pack(ByteBuffer bb, SimpleMotorFeedforward value) {
    bb.putDouble(value.getKs());
    bb.putDouble(value.getKv());
    bb.putDouble(value.getKa());
    bb.putDouble(value.getDt());
  }
}
