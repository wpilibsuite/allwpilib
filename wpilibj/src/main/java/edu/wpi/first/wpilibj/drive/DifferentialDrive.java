/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2017 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.drive;

import edu.wpi.first.wpilibj.SpeedController;
import edu.wpi.first.wpilibj.hal.FRCNetComm.tInstances;
import edu.wpi.first.wpilibj.hal.FRCNetComm.tResourceType;
import edu.wpi.first.wpilibj.hal.HAL;

/**
 * A class for driving differential drive/skid-steer drive platforms such as the Kit of Parts drive
 * base, "tank drive", or West Coast Drive.
 *
 * <p>These drive bases typically have drop-center / skid-steer with two or more wheels per side
 * (e.g., 6WD or 8WD). This class takes a SpeedController per side. For four and
 * six motor drivetrains, construct and pass in {@link SpeedControllerGroup} instances as follows.
 *
 * <p>Four motor drivetrain:
 * <pre><code>
 * public class Robot {
 *   Talon m_frontLeft = new Talon(1);
 *   Talon m_rearLeft = new Talon(2);
 *   SpeedControllerGroup m_left = new SpeedControllerGroup(m_frontLeft, m_rearLeft);
 *
 *   Talon m_frontRight = new Talon(3);
 *   Talon m_rearRight = new Talon(4);
 *   SpeedControllerGroup m_right = new SpeedControllerGroup(m_frontRight, m_rearRight);
 *
 *   DifferentialDrive m_drive = new DifferentialDrive(m_left, m_right);
 * }
 * </code></pre>
 *
 * <p>Six motor drivetrain:
 * <pre><code>
 * public class Robot {
 *   Talon m_frontLeft = new Talon(1);
 *   Talon m_midLeft = new Talon(2);
 *   Talon m_rearLeft = new Talon(3);
 *   SpeedControllerGroup m_left = new SpeedControllerGroup(m_frontLeft, m_midLeft, m_rearLeft);
 *
 *   Talon m_frontRight = new Talon(4);
 *   Talon m_midRight = new Talon(5);
 *   Talon m_rearRight = new Talon(6);
 *   SpeedControllerGroup m_right = new SpeedControllerGroup(m_frontRight, m_midRight, m_rearRight);
 *
 *   DifferentialDrive m_drive = new DifferentialDrive(m_left, m_right);
 * }
 * </code></pre>
 *
 * <p>A differential drive robot has left and right wheels separated by an arbitrary width.
 *
 * <p>Drive base diagram:
 * <pre>
 * |_______|
 * | |   | |
 *   |   |
 * |_|___|_|
 * |       |
 * </pre>
 *
 * <p>Each drive() function provides different inverse kinematic relations for a differential drive
 * robot. Motor outputs for the right side are negated, so motor direction inversion by the user is
 * usually unnecessary.
 */
public class DifferentialDrive extends RobotDriveBase {
  private SpeedController m_leftMotor;
  private SpeedController m_rightMotor;

  private double m_quickStopAccumulator = 0.0;
  private boolean m_reported = false;

  /**
   * Construct a DifferentialDrive.
   *
   * <p>To pass multiple motors per side, use a {@link SpeedControllerGroup}. If a motor needs to be
   * inverted, do so before passing it in.
   */
  public DifferentialDrive(SpeedController leftMotor, SpeedController rightMotor) {
    m_leftMotor = leftMotor;
    m_rightMotor = rightMotor;
  }

  /**
   * Arcade drive method for differential drive platform.
   *
   * @param y             The value to use for forwards/backwards. [-1.0..1.0]
   * @param rotation      The value to use for the rotation right/left. [-1.0..1.0]
   */
  @SuppressWarnings("ParameterName")
  public void arcadeDrive(double y, double rotation) {
    arcadeDrive(y, rotation, true);
  }

  /**
   * Arcade drive method for differential drive platform.
   *
   * @param y             The value to use for forwards/backwards. [-1.0..1.0]
   * @param rotation      The value to use for the rotation right/left [-1.0..1.0]
   * @param squaredInputs If set, decreases the input sensitivity at low speeds.
   */
  @SuppressWarnings("ParameterName")
  public void arcadeDrive(double y, double rotation, boolean squaredInputs) {
    if (!m_reported) {
      HAL.report(tResourceType.kResourceType_RobotDrive, 2, tInstances.kRobotDrive_ArcadeStandard);
      m_reported = true;
    }

    y = limit(y);
    y = applyDeadband(y, m_deadband);

    rotation = limit(rotation);
    rotation = applyDeadband(rotation, m_deadband);

    // square the inputs (while preserving the sign) to increase fine control
    // while permitting full power
    if (squaredInputs) {
      y = Math.copySign(y * y, y);
      rotation = Math.copySign(rotation * rotation, rotation);
    }

    double leftMotorOutput;
    double rightMotorOutput;

    double maxInput = Math.copySign(Math.max(Math.abs(y), Math.abs(rotation)), y);

    if (y > 0.0) {
      // First quadrant, else second quadrant
      if (rotation > 0.0) {
        leftMotorOutput = maxInput;
        rightMotorOutput = y - rotation;
      } else {
        leftMotorOutput = y + rotation;
        rightMotorOutput = maxInput;
      }
    } else {
      // Third quadrant, else fourth quadrant
      if (rotation > 0.0) {
        leftMotorOutput = y + rotation;
        rightMotorOutput = maxInput;
      } else {
        leftMotorOutput = maxInput;
        rightMotorOutput = y - rotation;
      }
    }

    m_leftMotor.set(limit(leftMotorOutput) * m_maxOutput);
    m_rightMotor.set(-limit(rightMotorOutput) * m_maxOutput);

    m_safetyHelper.feed();
  }

  /**
   * Curvature drive method for differential drive platform.
   *
   * <p>The rotation argument controls the curvature of the robot's path rather than its rate of
   * heading change. This makes the robot more controllable at high speeds. Also handles the
   * robot's quick turn functionality - "quick turn" overrides constant-curvature turning for
   * turn-in-place maneuvers.
   *
   * @param y           The value to use for forwards/backwards. [-1.0..1.0]
   * @param rotation    The value to use for the rotation right/left. [-1.0..1.0]
   * @param isQuickTurn If set, overrides constant-curvature turning for
   *                    turn-in-place maneuvers.
   */
  @SuppressWarnings("ParameterName")
  public void curvatureDrive(double y, double rotation, boolean isQuickTurn) {
    if (!m_reported) {
      // HAL.report(tResourceType.kResourceType_RobotDrive, 2, tInstances.kRobotDrive_Curvature);
      m_reported = true;
    }

    y = limit(y);
    y = applyDeadband(y, m_deadband);

    rotation = limit(rotation);
    rotation = applyDeadband(rotation, m_deadband);

    double angularPower;
    boolean overPower;

    if (isQuickTurn) {
      if (Math.abs(y) < 0.2) {
        final double alpha = 0.1;
        m_quickStopAccumulator =
            (1 - alpha) * m_quickStopAccumulator + alpha * limit(rotation) * 2;
      }
      overPower = true;
      angularPower = rotation;
    } else {
      overPower = false;
      angularPower = Math.abs(y) * rotation - m_quickStopAccumulator;

      if (m_quickStopAccumulator > 1) {
        m_quickStopAccumulator -= 1;
      } else if (m_quickStopAccumulator < -1) {
        m_quickStopAccumulator += 1;
      } else {
        m_quickStopAccumulator = 0.0;
      }
    }

    double leftMotorOutput = y + angularPower;
    double rightMotorOutput = y - angularPower;

    // If rotation is overpowered, reduce both outputs to within acceptable range
    if (overPower) {
      if (leftMotorOutput > 1.0) {
        rightMotorOutput -= leftMotorOutput - 1.0;
        leftMotorOutput = 1.0;
      } else if (rightMotorOutput > 1.0) {
        leftMotorOutput -= rightMotorOutput - 1.0;
        rightMotorOutput = 1.0;
      } else if (leftMotorOutput < -1.0) {
        rightMotorOutput -= leftMotorOutput + 1.0;
        leftMotorOutput = -1.0;
      } else if (rightMotorOutput < -1.0) {
        leftMotorOutput -= rightMotorOutput + 1.0;
        rightMotorOutput = -1.0;
      }
    }

    m_leftMotor.set(leftMotorOutput * m_maxOutput);
    m_rightMotor.set(-rightMotorOutput * m_maxOutput);

    m_safetyHelper.feed();
  }

  /**
   * Tank drive method for differential drive platform.
   *
   * @param left  The value to use for left side motors. [-1.0..1.0]
   * @param right The value to use for right side motors. [-1.0..1.0]
   */
  public void tankDrive(double left, double right) {
    tankDrive(left, right, true);
  }

  /**
   * Tank drive method for differential drive platform.
   *
   * @param left  The value to use for left side motors. [-1.0..1.0]
   * @param right The value to use for right side motors. [-1.0..1.0]
   * @param squaredInputs If set, decreases the input sensitivity at low speeds.
   */
  public void tankDrive(double left, double right, boolean squaredInputs) {
    if (!m_reported) {
      HAL.report(tResourceType.kResourceType_RobotDrive, 2, tInstances.kRobotDrive_Tank);
      m_reported = true;
    }

    left = limit(left);
    left = applyDeadband(left, m_deadband);

    right = limit(right);
    right = applyDeadband(right, m_deadband);

    // square the inputs (while preserving the sign) to increase fine control
    // while permitting full power
    if (squaredInputs) {
      left = Math.copySign(left * left, left);
      right = Math.copySign(right * right, right);
    }

    m_leftMotor.set(left * m_maxOutput);
    m_rightMotor.set(-right * m_maxOutput);

    m_safetyHelper.feed();
  }

  @Override
  public void stopMotor() {
    m_leftMotor.stopMotor();
    m_rightMotor.stopMotor();
    m_safetyHelper.feed();
  }

  @Override
  public String getDescription() {
    return "DifferentialDrive";
  }
}
