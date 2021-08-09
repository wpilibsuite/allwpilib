// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation.swerve;

import static org.junit.jupiter.api.Assertions.assertEquals;

import java.util.ArrayList;
import java.util.Arrays;

import edu.wpi.first.math.controller.PIDController;
import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.system.plant.DCMotor;
import edu.wpi.first.math.util.Units;
import edu.wpi.first.wpilibj.Encoder;
import edu.wpi.first.wpilibj.RobotController;
import edu.wpi.first.wpilibj.drive.Vector2d;
import edu.wpi.first.wpilibj.motorcontrol.PWMVictorSPX;
import edu.wpi.first.wpilibj.simulation.EncoderSim;
import edu.wpi.first.wpilibj.utils.SimpleCSVWriter;

import org.junit.jupiter.api.Test;

public class SwerveModuleSimTest {
  @Test
  @SuppressWarnings({"LocalVariableName", "resource"})
  public void testBasicWheel(TestInfo testInfo) {

    SimpleCSVWriter log = new SimpleCSVWriter(
      new ArrayList<String>(Arrays.asList("u_wheel", "y_wheelforce", "wheelvel", "u_azmth", "y_azmthPos")),
      new ArrayList<String>(Arrays.asList("V", "N", "RPM", "V", "rev"))
      );

    var wheelVelRadPerSec = 0.0;
    var wheelPosRad = 0.0;

    final var SIM_TS = 0.020;
    final var WHEEL_RAD_M = Units.inchesToMeters(1.5);
    final var TEST_PLANT_MASS_RADPERSEC2_PER_N = 6.0/90.0;

    var wheelController = new PIDController(10, 0, 0);
    var azmthController = new PIDController(10, 0, 0);

    var wheelMotor = new PWMVictorSPX(2);
    var azmthMotor = new PWMVictorSPX(3);
    var wheelEncoder = new Encoder(4, 5);
    var azmthEncoder = new Encoder(6, 7);

    final var TEST_DIR = new Vector2d(1, 0);

    var sim =
        new SwerveModuleSim(DCMotor.getCIM(1),
                            DCMotor.getCIM(1), 
                            WHEEL_RAD_M, 
                            130, 
                            9.0, 
                            1.0, 
                            1.0, 
                            1.1, 
                            0.8, 
                            16.0, 
                            0.001);


    var wheelEncoderSim = new EncoderSim(wheelEncoder);
    var azmthEncoderSim = new EncoderSim(azmthEncoder);

    for (int i = 0; i < 1000; i++) {

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

      var modulePose = new Pose2d(TEST_DIR.x * wheelPosRad * WHEEL_RAD_M,
                                  TEST_DIR.y * wheelPosRad * WHEEL_RAD_M,
                                  new Rotation2d());
      sim.setModulePose(modulePose);

      sim.update(SIM_TS);

      // Wheel plant model - separate from the module
      // Very simple test - F = ma
      var y_wheelforce = sim.getWheelMotiveForce();
      wheelVelRadPerSec += y_wheelforce.m_force.getVector2d().dot(TEST_DIR) * TEST_PLANT_MASS_RADPERSEC2_PER_N * SIM_TS;
      wheelEncoderSim.setRate(Units.radiansPerSecondToRotationsPerMinute(wheelVelRadPerSec));
      wheelPosRad += wheelVelRadPerSec * SIM_TS;
     
      //Azmth plant model is built into module simulation.
      var y_azmthPos = sim.getAzimuthEncoderPositionRev();
      azmthEncoderSim.setDistance(y_azmthPos);

      var logVals = new ArrayList<Double>();
      logVals.add(u_wheel);
      logVals.add(y_wheelforce.m_force.getNorm());
      logVals.add(wheelVelRadPerSec);
      logVals.add(u_azmth);
      logVals.add(y_azmthPos);
      log.writeData(i*SIM_TS, logVals);
    }

    log.close();

    assertEquals(wheelController.getSetpoint(), wheelEncoder.getRate(), 2);

  }

  public void testBasicAzmth(TestInfo testInfo){

  }

}
