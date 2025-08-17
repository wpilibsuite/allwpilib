// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.kinematics.proto;

import edu.wpi.first.math.kinematics.ChassisAccelerations;
import edu.wpi.first.math.proto.Kinematics.ProtobufChassisAccelerations;
import edu.wpi.first.util.protobuf.Protobuf;
import us.hebi.quickbuf.Descriptors.Descriptor;

public class ChassisAccelerationsProto implements Protobuf<ChassisAccelerations, ProtobufChassisAccelerations> {
  @Override
  public Class<ChassisAccelerations> getTypeClass() {
    return ChassisAccelerations.class;
  }

  @Override
  public Descriptor getDescriptor() {
    return ProtobufChassisAccelerations.getDescriptor();
  }

  @Override
  public ProtobufChassisAccelerations createMessage() {
    return ProtobufChassisAccelerations.newInstance();
  }

  @Override
  public ChassisAccelerations unpack(ProtobufChassisAccelerations msg) {
    return new ChassisAccelerations(msg.getAx(), msg.getAy(), msg.getAlpha());
  }

  @Override
  public void pack(ProtobufChassisAccelerations msg, ChassisAccelerations value) {
    msg.setAx(value.ax);
    msg.setAy(value.ay);
    msg.setAlpha(value.alpha);
  }
}
