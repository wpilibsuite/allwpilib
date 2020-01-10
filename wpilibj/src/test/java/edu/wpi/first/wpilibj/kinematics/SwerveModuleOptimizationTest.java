/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.kinematics;

import org.junit.jupiter.api.Test;

import edu.wpi.first.wpilibj.geometry.Rotation2d;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;

class SwerveModuleOptimizationTest {
  private final SwerveModuleState m_state = new SwerveModuleState(1, Rotation2d.fromDegrees(0));

  @Test
  void noOptimization() {
    SwerveModuleState newState = new SwerveModuleState(1, Rotation2d.fromDegrees(0));

    SwerveModuleState finalState = m_state.optimizeModuleAngle(newState);

    assertAll(
        () -> assertEquals(0, finalState.angle.getDegrees(), 0.01),
        () -> assertEquals(1, finalState.speedMetersPerSecond, 0.01)
    );
  }

  @Test
  void turn180() {
    SwerveModuleState newState = new SwerveModuleState(1, Rotation2d.fromDegrees(180));

    SwerveModuleState finalState = m_state.optimizeModuleAngle(newState);

    assertAll(
        () -> assertEquals(0, finalState.angle.getDegrees(), 0.01),
        () -> assertEquals(-1, finalState.speedMetersPerSecond, 0.01)
    );
  }

  @Test
  void turn45() {
    SwerveModuleState newState = new SwerveModuleState(1, Rotation2d.fromDegrees(45));

    SwerveModuleState finalState = m_state.optimizeModuleAngle(newState);

    assertAll(
        () -> assertEquals(45, finalState.angle.getDegrees(), 0.01),
        () -> assertEquals(1, finalState.speedMetersPerSecond, 0.01)
    );
  }


  @Test
  void turn135() {
    SwerveModuleState newState = new SwerveModuleState(1, Rotation2d.fromDegrees(135));

    SwerveModuleState finalState = m_state.optimizeModuleAngle(newState);

    assertAll(
        () -> assertEquals(315, finalState.angle.getDegrees(), 0.01),
        () -> assertEquals(-1, finalState.speedMetersPerSecond, 0.01)
    );
  }
}
