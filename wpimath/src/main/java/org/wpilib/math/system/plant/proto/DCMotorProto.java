// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.system.plant.proto;

import edu.wpi.first.math.proto.Plant.ProtobufDCMotor;
import edu.wpi.first.math.system.plant.DCMotor;
import edu.wpi.first.util.protobuf.Protobuf;
import us.hebi.quickbuf.Descriptors.Descriptor;

public class DCMotorProto implements Protobuf<DCMotor, ProtobufDCMotor> {
  @Override
  public Class<DCMotor> getTypeClass() {
    return DCMotor.class;
  }

  @Override
  public Descriptor getDescriptor() {
    return ProtobufDCMotor.getDescriptor();
  }

  @Override
  public ProtobufDCMotor createMessage() {
    return ProtobufDCMotor.newInstance();
  }

  @Override
  public DCMotor unpack(ProtobufDCMotor msg) {
    return new DCMotor(
        msg.getNominalVoltage(),
        msg.getStallTorque(),
        msg.getStallCurrent(),
        msg.getFreeCurrent(),
        msg.getFreeSpeed(),
        1);
  }

  @Override
  public void pack(ProtobufDCMotor msg, DCMotor value) {
    msg.setNominalVoltage(value.nominalVoltage);
    msg.setStallTorque(value.stallTorque);
    msg.setStallCurrent(value.stallCurrent);
    msg.setFreeCurrent(value.freeCurrent);
    msg.setFreeSpeed(value.freeSpeed);
  }
}
