/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.statespaceswervedrivesimulation;

import edu.wpi.first.wpilibj.AnalogEncoder;
import edu.wpi.first.wpilibj.AnalogInput;
import edu.wpi.first.wpilibj.Encoder;
import edu.wpi.first.wpilibj.PWMSpeedController;
import edu.wpi.first.wpilibj.controller.PIDController;
import edu.wpi.first.wpilibj.geometry.Rotation2d;
import edu.wpi.first.wpilibj.kinematics.SwerveModuleState;
import edu.wpi.first.wpilibj.simulation.AnalogEncoderSim;
import edu.wpi.first.wpilibj.simulation.EncoderSim;
import edu.wpi.first.wpilibj.smartdashboard.SmartDashboard;

class SwerveModule {
  final PIDController angleController;

  AnalogEncoder angleEncoder;
  AnalogEncoderSim angleEncoderSim;

  Encoder distanceEncoder;
  EncoderSim distanceEncoderSim;

  PWMSpeedController angleMotor;
  PWMSpeedController distanceMotor;

  SwerveModule(int angleEncoderChannel, Encoder distanceEncoder,
               PWMSpeedController angleMotor,
               PWMSpeedController distanceMotor) {
    this.angleEncoder = new AnalogEncoder(new AnalogInput(angleEncoderChannel));
    this.angleEncoderSim = new AnalogEncoderSim(angleEncoder);
    this.distanceEncoder = distanceEncoder;
    this.distanceEncoderSim = new EncoderSim(distanceEncoder);
    this.angleMotor = angleMotor;
    this.distanceMotor = distanceMotor;
    this.angleController = new PIDController(0.1, 0, 0);

    SmartDashboard.putData("Angle controller " + angleEncoderChannel, angleController);
  }

  SwerveModuleState getState() {
    return new SwerveModuleState(distanceEncoder.getRate(), Rotation2d.fromDegrees(angleEncoder.get() * 360.0));
  }
}
