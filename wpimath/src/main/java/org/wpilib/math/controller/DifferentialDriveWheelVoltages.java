// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.controller;

import org.wpilib.math.controller.proto.DifferentialDriveWheelVoltagesProto;
import org.wpilib.math.controller.struct.DifferentialDriveWheelVoltagesStruct;
import org.wpilib.util.protobuf.ProtobufSerializable;
import org.wpilib.util.struct.StructSerializable;

/** Motor voltages for a differential drive. */
public class DifferentialDriveWheelVoltages implements ProtobufSerializable, StructSerializable {
  /** Left wheel voltage. */
  public double left;

  /** Right wheel voltage. */
  public double right;

  /** DifferentialDriveWheelVoltages protobuf for serialization. */
  public static final DifferentialDriveWheelVoltagesProto proto =
      new DifferentialDriveWheelVoltagesProto();

  /** DifferentialDriveWheelVoltages struct for serialization. */
  public static final DifferentialDriveWheelVoltagesStruct struct =
      new DifferentialDriveWheelVoltagesStruct();

  /** Default constructor. */
  public DifferentialDriveWheelVoltages() {}

  /**
   * Constructs a DifferentialDriveWheelVoltages.
   *
   * @param left Left wheel voltage.
   * @param right Right wheel voltage.
   */
  public DifferentialDriveWheelVoltages(double left, double right) {
    this.left = left;
    this.right = right;
  }
}
