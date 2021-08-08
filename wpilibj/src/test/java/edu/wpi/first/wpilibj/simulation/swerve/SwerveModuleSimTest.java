// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation.swerve;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.controller.PIDController;
import edu.wpi.first.math.system.plant.DCMotor;
import edu.wpi.first.wpilibj.Encoder;
import edu.wpi.first.wpilibj.RobotController;
import edu.wpi.first.wpilibj.motorcontrol.PWMVictorSPX;
import edu.wpi.first.wpilibj.simulation.EncoderSim;

import org.junit.jupiter.api.Test;

public class SwerveModuleSimTest {
  @Test
  @SuppressWarnings({"LocalVariableName", "resource"})
  public void testBasic() {

    var wheelVel = 0;

    var wheelController = new PIDController(10, 0, 0);
    var azmthController = new PIDController(10, 0, 0);

    var wheelMotor = new PWMVictorSPX(2);
    var azmthMotor = new PWMVictorSPX(3);
    var wheelEncoder = new Encoder(4, 5);
    var azmthEncoder = new Encoder(6, 7);

    var sim =
        new SwerveModuleSim(DCMotor.getCIM(1),
                            DCMotor.getCIM(1), 
                            0.1, 
                            130, 
                            9.0, 
                            1.0, 
                            1.0, 
                            1.1, 
                            0.8, 
                            16.0, 
                            0.01);


    var wheelEncoderSim = new EncoderSim(wheelEncoder);
    var azmthEncoderSim = new EncoderSim(azmthEncoder);

    for (int i = 0; i < 100; i++) {

      // ------ User Periodic -------

      wheelController.setSetpoint(100); // RPM
      azmthController.setSetpoint(0.35); // Rotations

      double currentBatteryVoltage = RobotController.getBatteryVoltage();
      double nextWheelVoltage = wheelController.calculate(wheelEncoder.getRate());
      double nextAzmthVoltage = azmthController.calculate(azmthEncoder.getDistance());

      wheelMotor.set(nextWheelVoltage / currentBatteryVoltage);
      azmthMotor.set(nextAzmthVoltage / currentBatteryVoltage);

      // ------ Simulation Periodic -------

      var u_wheel = wheelMotor.get() * currentBatteryVoltage;
      var u_azmth = azmthMotor.get() * currentBatteryVoltage;
      sim.setInputVoltages(u_wheel, u_azmth);

      sim.update(0.020);

      // Wheel plant model - separate from the module
      // Very simple test - F = ma
      var y_wheelforce = sim.getWheelMotiveForce();
      wheelVel += y_wheelforce.m_force.getNorm()/2.0;
      wheelEncoderSim.setRate(wheelVel);
     
      //Azmth plant model is built into module simulation.
      var y_azmthPos = sim.getAzimuthEncoderPositionRev();
      azmthEncoderSim.setDistance(y_azmthPos);
    }

    assertEquals(wheelController.getSetpoint(), wheelEncoder.getRate(), 2);
    assertEquals(azmthController.getSetpoint(), azmthEncoder.getDistance(), 0.02);
  }

}
