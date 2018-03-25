/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.ctrlsys;

import edu.wpi.first.wpilibj.Controller;
import edu.wpi.first.wpilibj.PIDOutput;

/**
 * A feedback controller for a differential-drive robot. A differential-drive robot has left and
 * right wheels separated by an arbitrary width.
 *
 * <p>A forward distance controller and angle controller are run in parallel and their outputs are
 * composed to drive each wheel. Since the forward controller uses the average distance of the two
 * sides while the angle controller uses the difference between them, the controllers act
 * independently on the drive base and can thus be tuned separately.
 *
 * <p>If you don't have a gyroscope for an angle sensor, the following equation can be used in a
 * FuncNode to estimate it.
 *
 * <p>angle = (right - left) / width * 180 / pi
 *
 * <p>where "right" is the right encoder reading, "left" is the left encoder reading, "width" is the
 * width of the robot in the same units as the encoders, and "angle" is the angle of the robot in
 * degrees. We recommend passing this angle estimation through a low-pass filter (see
 * LinearFilter).
 *
 * <p>Set the position and angle PID constants via getPositionPID().setPID() and
 * getAnglePID().setPID() before enabling this controller.
 */
public class DiffDriveController implements Controller {
  // Control system references
  private INode m_positionRef;
  private INode m_angleRef;

  // Encoders
  private INode m_leftEncoder;
  private INode m_rightEncoder;

  // Angle sensor (e.g., gyroscope)
  private INode m_angleSensor;
  private boolean m_clockwise;

  // Motors
  private PIDOutput m_leftMotor;
  private PIDOutput m_rightMotor;

  // Position PID
  private FuncNode m_positionCalc = new FuncNode(() -> {
    return (m_leftEncoder.getOutput() + m_rightEncoder.getOutput()) / 2.0;
  });
  private SumNode m_positionError = new SumNode(m_positionRef, true, m_positionCalc, false);
  private PIDNode m_positionPID = new PIDNode(0.0, 0.0, 0.0, m_positionError);

  // Angle PID
  private SumNode m_angleError = new SumNode(m_angleRef, true, m_angleSensor, false);
  private PIDNode m_anglePID = new PIDNode(0.0, 0.0, 0.0, m_angleError);

  // Combine outputs for left motor
  private SumNode m_leftMotorInput = new SumNode(m_positionPID, true, m_anglePID, true);
  private Output m_leftOutput;

  // Combine outputs for right motor
  private SumNode m_rightMotorInput = new SumNode(m_positionPID, true, m_anglePID, false);
  private Output m_rightOutput;

  private OutputGroup m_outputs;
  private double m_period;

  /**
   * Constructs DiffDriveController.
   *
   * @param positionRef position reference input
   * @param angleRef angle reference input
   * @param leftEncoder left encoder
   * @param rightEncoder right encoder
   * @param angleSensor angle sensor (e.g, gyroscope)
   * @param clockwise true if clockwise rotation increases angle measurement
   * @param leftMotor left motor output
   * @param rightMotor right motor output
   * @param period the loop time for doing calculations.
   */
  public DiffDriveController(INode positionRef, INode angleRef, INode leftEncoder,
                      INode rightEncoder, INode angleSensor, boolean clockwise,
                      PIDOutput leftMotor, PIDOutput rightMotor,
                      double period) {
    m_positionRef = positionRef;
    m_angleRef = angleRef;
    m_leftEncoder = leftEncoder;
    m_rightEncoder = rightEncoder;
    m_angleSensor = angleSensor;
    m_clockwise = clockwise;
    m_leftMotor = leftMotor;
    m_rightMotor = rightMotor;
    m_leftMotorInput = new SumNode(m_positionPID, true, m_anglePID, m_clockwise);
    m_leftOutput = new Output(m_leftMotorInput, m_leftMotor);
    m_rightMotorInput = new SumNode(m_positionPID, true, m_anglePID, !m_clockwise);
    m_rightOutput = new Output(m_rightMotorInput, m_rightMotor);
    m_outputs = new OutputGroup(m_leftOutput, m_rightOutput);
    m_period = period;
  }

  /**
   * Constructs DiffDriveController.
   *
   * @param positionRef position reference input
   * @param angleRef angle reference input
   * @param leftEncoder left encoder
   * @param rightEncoder right encoder
   * @param angleSensor angle sensor (e.g, gyroscope)
   * @param clockwise true if clockwise rotation increases angle measurement
   * @param leftMotor left motor output
   * @param rightMotor right motor output
   */
  public DiffDriveController(INode positionRef, INode angleRef, INode leftEncoder,
                      INode rightEncoder, INode angleSensor, boolean clockwise,
                      PIDOutput leftMotor, PIDOutput rightMotor) {
    this(positionRef, angleRef, leftEncoder, rightEncoder, angleSensor, clockwise, leftMotor,
        rightMotor, INode.DEFAULT_PERIOD);
  }

  @Override
  public void enable() {
    m_outputs.enable(m_period);
  }

  @Override
  public void disable() {
    m_outputs.disable();
  }

  public PIDNode getPositionPID() {
    return m_positionPID;
  }

  public PIDNode getAnglePID() {
    return m_anglePID;
  }

  public double getPosition() {
    return m_positionCalc.getOutput();
  }

  public double getAngle() {
    return m_angleSensor.getOutput();
  }

  public void setPositionTolerance(double tolerance, double deltaTolerance) {
    m_positionError.setTolerance(tolerance, deltaTolerance);
  }

  public void setAngleTolerance(double tolerance, double deltaTolerance) {
    m_angleError.setTolerance(tolerance, deltaTolerance);
  }

  public boolean atPosition() {
    return m_positionError.inTolerance();
  }

  public boolean atAngle() {
    return m_angleError.inTolerance();
  }
}
