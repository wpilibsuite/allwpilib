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
import org.junit.jupiter.api.TestInfo;

public class QuadSwerveDriveTest {

  final double SIM_TS = 0.020;
  final double WHEEL_RAD_M = Units.inchesToMeters(1.5);
  final double TEST_PLANT_MASS_RADPERSEC2_PER_N = 2.0 / 90.0;

  final Vector2d TEST_DIR_XHAT = new Vector2d(1, 0);
  final Vector2d TEST_DIR_YHAT = new Vector2d(0, 1);

  final int FL = 0;
  final int FR = 1;
  final int BL = 2;
  final int BR = 3;
  final int NUM_MOD = 4;

  ArrayList<PWMVictorSPX> wheelMotors = new ArrayList<PWMVictorSPX>(NUM_MOD);
  ArrayList<PWMVictorSPX> azmthMotors = new ArrayList<PWMVictorSPX>(NUM_MOD);
  ArrayList<Encoder> wheelEncoders = new ArrayList<Encoder>(NUM_MOD);
  ArrayList<Encoder> azmthEncoders = new ArrayList<Encoder>(NUM_MOD);
  ArrayList<SwerveModuleSim> modules = new ArrayList<SwerveModuleSim>(NUM_MOD);

  QuadSwerveSim sim;

  private SwerveModuleSim moduleFactory(){
    return new SwerveModuleSim(
      DCMotor.getCIM(1), 
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
  }

  private void initHardware(){

    wheelMotors.add(new PWMVictorSPX(2));
    wheelMotors.add(new PWMVictorSPX(3));
    wheelMotors.add(new PWMVictorSPX(4));
    wheelMotors.add(new PWMVictorSPX(5));

    azmthMotors.add(new PWMVictorSPX(6));
    azmthMotors.add(new PWMVictorSPX(7));
    azmthMotors.add(new PWMVictorSPX(8));
    azmthMotors.add(new PWMVictorSPX(9));

    wheelEncoders.add(new Encoder(4, 5));
    wheelEncoders.add(new Encoder(6, 7));
    wheelEncoders.add(new Encoder(8, 9));
    wheelEncoders.add(new Encoder(10, 11));
    
    azmthEncoders.add(new Encoder(12, 13));
    azmthEncoders.add(new Encoder(14, 15));
    azmthEncoders.add(new Encoder(16, 17));
    azmthEncoders.add(new Encoder(18, 19));

    azmthEncoders.forEach(enc -> enc.setDistancePerPulse(1.0/4096.0));
    wheelEncoders.forEach(enc -> enc.setDistancePerPulse(1.0/128.0));

    for(int idx = 0; idx < NUM_MOD; idx++){
      modules.add(moduleFactory());
    }

    sim = new QuadSwerveSim(0.75, 0.75, 64.0, 1.0, modules);

  }

  private void closeHardware(){
    wheelMotors.forEach(m -> {m.close();});
    azmthMotors.forEach(m -> {m.close();});
    wheelEncoders.forEach(m -> {m.close();});
    azmthEncoders.forEach(m -> {m.close();});
  }

  @Test
  @SuppressWarnings({ "LocalVariableName", "resource" })
  public void testBasic(TestInfo testInfo) {

    SimpleCSVWriter log = new SimpleCSVWriter(testInfo.getDisplayName(),
        new ArrayList<String>(Arrays.asList("u_wheel", "y_wheelforce", "wheelVel", "wheelVelEnc", "u_azmth", "y_azmthPos")),
        new ArrayList<String>(Arrays.asList("V", "N", "RPM", "RPM", "V", "rev")));

    initHardware();
    
    // Set up closed-loop controllers
    var wheelControllers = new ArrayList<PIDController>(NUM_MOD);
    var azmthControllers = new ArrayList<PIDController>(NUM_MOD);

    for(int idx = 0; idx < NUM_MOD; idx++){
      wheelControllers.add(new PIDController(3, 0, 0));
      azmthControllers.add(new PIDController(8, 0, 0));
    }

    // Set up sensor simulation
    var wheelEncoderSim = new ArrayList<EncoderSim>(NUM_MOD);
    var azmthEncoderSim = new ArrayList<EncoderSim>(NUM_MOD);
    wheelEncoders.forEach(enc -> wheelEncoderSim.add(new EncoderSim(enc)));
    azmthEncoders.forEach(enc -> azmthEncoderSim.add(new EncoderSim(enc)));

    for (int i = 0; i < 500; i++) {

      // ------ User Periodic -------

      wheelControllers.forEach(ctrl -> ctrl.setSetpoint(100));
      azmthControllers.forEach(ctrl -> ctrl.setSetpoint(0.125));

      double currentBatteryVoltage = RobotController.getBatteryVoltage();
      
      //todo set voltages

      // ------ Simulation Periodic -------

      sim.update(SIM_TS);

      var logVals = new ArrayList<Double>();
      //todo populate logVals
      log.writeData(i * SIM_TS, logVals);
    }

    log.close();

    azmthControllers.forEach(c -> c.close());
    wheelControllers.forEach(c -> c.close());

    closeHardware();

    //todo pass fail
  }

}
