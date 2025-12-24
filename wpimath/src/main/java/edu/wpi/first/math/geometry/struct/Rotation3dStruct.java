// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.geometry.struct;

import edu.wpi.first.math.geometry.Rotation3d;
import edu.wpi.first.util.struct.Struct;
import java.nio.ByteBuffer;

public class Rotation3dStruct implements Struct<Rotation3d> {
  @Override
  public Class<Rotation3d> getTypeClass() {
    return Rotation3d.class;
  }

  @Override
  public String getTypeName() {
    return "Rotation3d";
  }

  @Override
  public int getSize() {
    return kSizeDouble * 3;
  }

  @Override
  public String getSchema() {
    return "double roll;double pitch;double yaw";
  }

  @Override
  public Rotation3d unpack(ByteBuffer bb) {
    double roll = bb.getDouble();
    double pitch = bb.getDouble();
    double yaw = bb.getDouble();
    return new Rotation3d(roll, pitch, yaw);
  }

  @Override
  public void pack(ByteBuffer bb, Rotation3d value) {
    bb.putDouble(value.getX());
    bb.putDouble(value.getY());
    bb.putDouble(value.getZ());
  }

  @Override
  public boolean isImmutable() {
    return true;
  }
}
