// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.system.plant.proto;

import static edu.wpi.first.units.Units.Amps;
import static edu.wpi.first.units.Units.NewtonMeters;
import static edu.wpi.first.units.Units.RadiansPerSecond;
import static edu.wpi.first.units.Units.Volts;

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
        msg.getFreeSpeed());
  }

  @Override
  public void pack(ProtobufDCMotor msg, DCMotor value) {
    msg.setNominalVoltage(value.nominalVoltage.in(Volts));
    msg.setStallTorque(value.stallTorque.in(NewtonMeters));
    msg.setStallCurrent(value.stallCurrent.in(Amps));
    msg.setFreeCurrent(value.freeCurrent.in(Amps));
    msg.setFreeSpeed(value.freeSpeed.in(RadiansPerSecond));
  }
}
