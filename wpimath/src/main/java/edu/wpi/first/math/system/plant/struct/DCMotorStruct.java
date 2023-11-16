// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.system.plant.struct;

import edu.wpi.first.math.system.plant.DCMotor;
import edu.wpi.first.util.struct.Struct;
import java.nio.ByteBuffer;

public class DCMotorStruct implements Struct<DCMotor> {
  @Override
  public Class<DCMotor> getTypeClass() {
    return DCMotor.class;
  }

  @Override
  public String getTypeString() {
    return "struct:DCMotor";
  }

  @Override
  public int getSize() {
    return kSizeDouble * 8;
  }

  @Override
  public String getSchema() {
    return "double nominal_voltage;double stall_torque;double stall_current;double free_current;double free_speed;double r;double kv;double kt";
  }

  @Override
  public DCMotor unpack(ByteBuffer bb) {
    double nominalVoltage = bb.getDouble();
    double stallTorque = bb.getDouble();
    double stallCurrent = bb.getDouble();
    double freeCurrent = bb.getDouble();
    double freeSpeed = bb.getDouble();
    double r = bb.getDouble();
    double kv = bb.getDouble();
    double kt = bb.getDouble();
    return new DCMotor(
        nominalVoltage, stallTorque, stallCurrent, freeCurrent, freeSpeed, r, kv, kt);
  }

  @Override
  public void pack(ByteBuffer bb, DCMotor value) {
    bb.putDouble(value.nominalVoltageVolts);
    bb.putDouble(value.stallTorqueNewtonMeters);
    bb.putDouble(value.stallCurrentAmps);
    bb.putDouble(value.freeCurrentAmps);
    bb.putDouble(value.freeSpeedRadPerSec);
    bb.putDouble(value.rOhms);
    bb.putDouble(value.KvRadPerSecPerVolt);
    bb.putDouble(value.KtNMPerAmp);
  }
}
