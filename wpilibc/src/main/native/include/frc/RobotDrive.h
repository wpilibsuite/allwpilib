/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>

#include <wpi/deprecated.h>
#include <wpi/raw_ostream.h>

#include "frc/ErrorBase.h"
#include "frc/MotorSafety.h"

namespace frc {

class SpeedController;
class GenericHID;

/**
 * Utility class for handling Robot drive based on a definition of the motor
 * configuration.
 *
 * The robot drive class handles basic driving for a robot. Currently, 2 and 4
 * motor tank and mecanum drive trains are supported. In the future other drive
 * types like swerve might be implemented. Motor channel numbers are passed
 * supplied on creation of the class. Those are used for either the Drive
 * function (intended for hand created drive code, such as autonomous) or with
 * the Tank/Arcade functions intended to be used for Operator Control driving.
 */
class RobotDrive : public MotorSafety {
 public:
  enum MotorType {
    kFrontLeftMotor = 0,
    kFrontRightMotor = 1,
    kRearLeftMotor = 2,
    kRearRightMotor = 3
  };

  /**
   * Constructor for RobotDrive with 2 motors specified with channel numbers.
   *
   * Set up parameters for a two wheel drive system where the
   * left and right motor pwm channels are specified in the call.
   * This call assumes Talons for controlling the motors.
   *
   * @param leftMotorChannel  The PWM channel number that drives the left motor.
   *                          0-9 are on-board, 10-19 are on the MXP port
   * @param rightMotorChannel The PWM channel number that drives the right
   *                          motor. 0-9 are on-board, 10-19 are on the MXP port
   */
  WPI_DEPRECATED("Use DifferentialDrive or MecanumDrive classes instead.")
  RobotDrive(int leftMotorChannel, int rightMotorChannel);

  /**
   * Constructor for RobotDrive with 4 motors specified with channel numbers.
   *
   * Set up parameters for a four wheel drive system where all four motor
   * pwm channels are specified in the call.
   * This call assumes Talons for controlling the motors.
   *
   * @param frontLeftMotor  Front left motor channel number. 0-9 are on-board,
   *                        10-19 are on the MXP port
   * @param rearLeftMotor   Rear Left motor channel number. 0-9 are on-board,
   *                        10-19 are on the MXP port
   * @param frontRightMotor Front right motor channel number. 0-9 are on-board,
   *                        10-19 are on the MXP port
   * @param rearRightMotor  Rear Right motor channel number. 0-9 are on-board,
   *                        10-19 are on the MXP port
   */
  WPI_DEPRECATED("Use DifferentialDrive or MecanumDrive classes instead.")
  RobotDrive(int frontLeftMotorChannel, int rearLeftMotorChannel,
             int frontRightMotorChannel, int rearRightMotorChannel);

  /**
   * Constructor for RobotDrive with 2 motors specified as SpeedController
   * objects.
   *
   * The SpeedController version of the constructor enables programs to use the
   * RobotDrive classes with subclasses of the SpeedController objects, for
   * example, versions with ramping or reshaping of the curve to suit motor bias
   * or deadband elimination.
   *
   * @param leftMotor  The left SpeedController object used to drive the robot.
   * @param rightMotor The right SpeedController object used to drive the robot.
   */
  WPI_DEPRECATED("Use DifferentialDrive or MecanumDrive classes instead.")
  RobotDrive(SpeedController* leftMotor, SpeedController* rightMotor);

  WPI_DEPRECATED("Use DifferentialDrive or MecanumDrive classes instead.")
  RobotDrive(SpeedController& leftMotor, SpeedController& rightMotor);

  WPI_DEPRECATED("Use DifferentialDrive or MecanumDrive classes instead.")
  RobotDrive(std::shared_ptr<SpeedController> leftMotor,
             std::shared_ptr<SpeedController> rightMotor);

  /**
   * Constructor for RobotDrive with 4 motors specified as SpeedController
   * objects.
   *
   * Speed controller input version of RobotDrive (see previous comments).
   *
   * @param frontLeftMotor  The front left SpeedController object used to drive
   *                        the robot.
   * @param rearLeftMotor   The back left SpeedController object used to drive
   *                        the robot.
   * @param frontRightMotor The front right SpeedController object used to drive
   *                        the robot.
   * @param rearRightMotor  The back right SpeedController object used to drive
   *                        the robot.
   */
  WPI_DEPRECATED("Use DifferentialDrive or MecanumDrive classes instead.")
  RobotDrive(SpeedController* frontLeftMotor, SpeedController* rearLeftMotor,
             SpeedController* frontRightMotor, SpeedController* rearRightMotor);

  WPI_DEPRECATED("Use DifferentialDrive or MecanumDrive classes instead.")
  RobotDrive(SpeedController& frontLeftMotor, SpeedController& rearLeftMotor,
             SpeedController& frontRightMotor, SpeedController& rearRightMotor);

  WPI_DEPRECATED("Use DifferentialDrive or MecanumDrive classes instead.")
  RobotDrive(std::shared_ptr<SpeedController> frontLeftMotor,
             std::shared_ptr<SpeedController> rearLeftMotor,
             std::shared_ptr<SpeedController> frontRightMotor,
             std::shared_ptr<SpeedController> rearRightMotor);

  virtual ~RobotDrive() = default;

  RobotDrive(RobotDrive&&) = default;
  RobotDrive& operator=(RobotDrive&&) = default;

  /**
   * Drive the motors at "outputMagnitude" and "curve".
   *
   * Both outputMagnitude and curve are -1.0 to +1.0 values, where 0.0
   * represents stopped and not turning. curve < 0 will turn left and curve > 0
   * will turn right.
   *
   * The algorithm for steering provides a constant turn radius for any normal
   * speed range, both forward and backward. Increasing m_sensitivity causes
   * sharper turns for fixed values of curve.
   *
   * This function will most likely be used in an autonomous routine.
   *
   * @param outputMagnitude The speed setting for the outside wheel in a turn,
   *                        forward or backwards, +1 to -1.
   * @param curve           The rate of turn, constant for different forward
   *                        speeds. Set curve < 0 for left turn or curve > 0 for
   *                        right turn.
   *
   * Set curve = e^(-r/w) to get a turn radius r for wheelbase w of your robot.
   * Conversely, turn radius r = -ln(curve)*w for a given value of curve and
   * wheelbase w.
   */
  void Drive(double outputMagnitude, double curve);

  /**
   * Provide tank steering using the stored robot configuration.
   *
   * Drive the robot using two joystick inputs. The Y-axis will be selected from
   * each Joystick object.
   *
   * @param leftStick  The joystick to control the left side of the robot.
   * @param rightStick The joystick to control the right side of the robot.
   * @param squaredInputs If true, the sensitivity will be decreased for small
   *                      values
   */
  void TankDrive(GenericHID* leftStick, GenericHID* rightStick,
                 bool squaredInputs = true);

  /**
   * Provide tank steering using the stored robot configuration.
   *
   * Drive the robot using two joystick inputs. The Y-axis will be selected from
   * each Joystick object.
   *
   * @param leftStick  The joystick to control the left side of the robot.
   * @param rightStick The joystick to control the right side of the robot.
   * @param squaredInputs If true, the sensitivity will be decreased for small
   *                      values
   */
  void TankDrive(GenericHID& leftStick, GenericHID& rightStick,
                 bool squaredInputs = true);

  /**
   * Provide tank steering using the stored robot configuration.
   *
   * This function lets you pick the axis to be used on each Joystick object for
   * the left and right sides of the robot.
   *
   * @param leftStick  The Joystick object to use for the left side of the
   *                   robot.
   * @param leftAxis   The axis to select on the left side Joystick object.
   * @param rightStick The Joystick object to use for the right side of the
   *                   robot.
   * @param rightAxis  The axis to select on the right side Joystick object.
   * @param squaredInputs If true, the sensitivity will be decreased for small
   *                      values
   */
  void TankDrive(GenericHID* leftStick, int leftAxis, GenericHID* rightStick,
                 int rightAxis, bool squaredInputs = true);

  void TankDrive(GenericHID& leftStick, int leftAxis, GenericHID& rightStick,
                 int rightAxis, bool squaredInputs = true);

  /**
   * Provide tank steering using the stored robot configuration.
   *
   * This function lets you directly provide joystick values from any source.
   *
   * @param leftValue  The value of the left stick.
   * @param rightValue The value of the right stick.
   * @param squaredInputs If true, the sensitivity will be decreased for small
   *                      values
   */
  void TankDrive(double leftValue, double rightValue,
                 bool squaredInputs = true);

  /**
   * Arcade drive implements single stick driving.
   *
   * Given a single Joystick, the class assumes the Y axis for the move value
   * and the X axis for the rotate value. (Should add more information here
   * regarding the way that arcade drive works.)
   *
   * @param stick         The joystick to use for Arcade single-stick driving.
   *                      The Y-axis will be selected for forwards/backwards and
   *                      the X-axis will be selected for rotation rate.
   * @param squaredInputs If true, the sensitivity will be decreased for small
   *                      values
   */
  void ArcadeDrive(GenericHID* stick, bool squaredInputs = true);

  /**
   * Arcade drive implements single stick driving.
   *
   * Given a single Joystick, the class assumes the Y axis for the move value
   * and the X axis for the rotate value. (Should add more information here
   * regarding the way that arcade drive works.)
   *
   * @param stick         The joystick to use for Arcade single-stick driving.
   *                      The Y-axis will be selected for forwards/backwards and
   *                      the X-axis will be selected for rotation rate.
   * @param squaredInputs If true, the sensitivity will be decreased for small
   *                      values
   */
  void ArcadeDrive(GenericHID& stick, bool squaredInputs = true);

  /**
   * Arcade drive implements single stick driving.
   *
   * Given two joystick instances and two axis, compute the values to send to
   * either two or four motors.
   *
   * @param moveStick     The Joystick object that represents the
   *                      forward/backward direction
   * @param moveAxis      The axis on the moveStick object to use for
   *                      forwards/backwards (typically Y_AXIS)
   * @param rotateStick   The Joystick object that represents the rotation value
   * @param rotateAxis    The axis on the rotation object to use for the rotate
   *                      right/left (typically X_AXIS)
   * @param squaredInputs Setting this parameter to true increases the
   *                      sensitivity at lower speeds
   */
  void ArcadeDrive(GenericHID* moveStick, int moveChannel,
                   GenericHID* rotateStick, int rotateChannel,
                   bool squaredInputs = true);

  /**
   * Arcade drive implements single stick driving.
   *
   * Given two joystick instances and two axis, compute the values to send to
   * either two or four motors.
   *
   * @param moveStick     The Joystick object that represents the
   *                      forward/backward direction
   * @param moveAxis      The axis on the moveStick object to use for
   *                      forwards/backwards (typically Y_AXIS)
   * @param rotateStick   The Joystick object that represents the rotation value
   * @param rotateAxis    The axis on the rotation object to use for the rotate
   *                      right/left (typically X_AXIS)
   * @param squaredInputs Setting this parameter to true increases the
   *                      sensitivity at lower speeds
   */
  void ArcadeDrive(GenericHID& moveStick, int moveChannel,
                   GenericHID& rotateStick, int rotateChannel,
                   bool squaredInputs = true);

  /**
   * Arcade drive implements single stick driving.
   *
   * This function lets you directly provide joystick values from any source.
   *
   * @param moveValue     The value to use for fowards/backwards
   * @param rotateValue   The value to use for the rotate right/left
   * @param squaredInputs If set, increases the sensitivity at low speeds
   */
  void ArcadeDrive(double moveValue, double rotateValue,
                   bool squaredInputs = true);

  /**
   * Drive method for Mecanum wheeled robots.
   *
   * A method for driving with Mecanum wheeled robots. There are 4 wheels
   * on the robot, arranged so that the front and back wheels are toed in 45
   * degrees.
   * When looking at the wheels from the top, the roller axles should form an X
   * across the robot.
   *
   * This is designed to be directly driven by joystick axes.
   *
   * @param x         The speed that the robot should drive in the X direction.
   *                  [-1.0..1.0]
   * @param y         The speed that the robot should drive in the Y direction.
   *                  This input is inverted to match the forward == -1.0 that
   *                  joysticks produce. [-1.0..1.0]
   * @param rotation  The rate of rotation for the robot that is completely
   *                  independent of the translation. [-1.0..1.0]
   * @param gyroAngle The current angle reading from the gyro.  Use this to
   *                  implement field-oriented controls.
   */
  void MecanumDrive_Cartesian(double x, double y, double rotation,
                              double gyroAngle = 0.0);

  /**
   * Drive method for Mecanum wheeled robots.
   *
   * A method for driving with Mecanum wheeled robots. There are 4 wheels
   * on the robot, arranged so that the front and back wheels are toed in 45
   * degrees.
   * When looking at the wheels from the top, the roller axles should form an X
   * across the robot.
   *
   * @param magnitude The speed that the robot should drive in a given
   *                  direction. [-1.0..1.0]
   * @param direction The direction the robot should drive in degrees. The
   *                  direction and maginitute are independent of the rotation
   *                  rate.
   * @param rotation  The rate of rotation for the robot that is completely
   *                  independent of the magnitute or direction. [-1.0..1.0]
   */
  void MecanumDrive_Polar(double magnitude, double direction, double rotation);

  /**
   * Holonomic Drive method for Mecanum wheeled robots.
   *
   * This is an alias to MecanumDrive_Polar() for backward compatability
   *
   * @param magnitude The speed that the robot should drive in a given
   *                  direction. [-1.0..1.0]
   * @param direction The direction the robot should drive. The direction and
   *                  magnitude are independent of the rotation rate.
   * @param rotation  The rate of rotation for the robot that is completely
   *                  independent of the magnitude or direction.  [-1.0..1.0]
   */
  void HolonomicDrive(double magnitude, double direction, double rotation);

  /**
   * Set the speed of the right and left motors.
   *
   * This is used once an appropriate drive setup function is called such as
   * TwoWheelDrive(). The motors are set to "leftOutput" and "rightOutput"
   * and includes flipping the direction of one side for opposing motors.
   *
   * @param leftOutput  The speed to send to the left side of the robot.
   * @param rightOutput The speed to send to the right side of the robot.
   */
  virtual void SetLeftRightMotorOutputs(double leftOutput, double rightOutput);

  /*
   * Invert a motor direction.
   *
   * This is used when a motor should run in the opposite direction as the drive
   * code would normally run it. Motors that are direct drive would be inverted,
   * the Drive code assumes that the motors are geared with one reversal.
   *
   * @param motor      The motor index to invert.
   * @param isInverted True if the motor should be inverted when operated.
   */
  void SetInvertedMotor(MotorType motor, bool isInverted);

  /**
   * Set the turning sensitivity.
   *
   * This only impacts the Drive() entry-point.
   *
   * @param sensitivity Effectively sets the turning sensitivity (or turn radius
   *                    for a given value)
   */
  void SetSensitivity(double sensitivity);

  /**
   * Configure the scaling factor for using RobotDrive with motor controllers in
   * a mode other than PercentVbus.
   *
   * @param maxOutput Multiplied with the output percentage computed by the
   *                  drive functions.
   */
  void SetMaxOutput(double maxOutput);

  void StopMotor() override;
  void GetDescription(wpi::raw_ostream& desc) const override;

 protected:
  /**
   * Common function to initialize all the robot drive constructors.
   *
   * Create a motor safety object (the real reason for the common code) and
   * initialize all the motor assignments. The default timeout is set for the
   * robot drive.
   */
  void InitRobotDrive();

  /**
   * Limit motor values to the -1.0 to +1.0 range.
   */
  double Limit(double number);

  /**
   * Normalize all wheel speeds if the magnitude of any wheel is greater than
   * 1.0.
   */
  void Normalize(double* wheelSpeeds);

  /**
   * Rotate a vector in Cartesian space.
   */
  void RotateVector(double& x, double& y, double angle);

  static constexpr int kMaxNumberOfMotors = 4;

  double m_sensitivity = 0.5;
  double m_maxOutput = 1.0;

  std::shared_ptr<SpeedController> m_frontLeftMotor;
  std::shared_ptr<SpeedController> m_frontRightMotor;
  std::shared_ptr<SpeedController> m_rearLeftMotor;
  std::shared_ptr<SpeedController> m_rearRightMotor;

 private:
  int GetNumMotors() {
    int motors = 0;
    if (m_frontLeftMotor) motors++;
    if (m_frontRightMotor) motors++;
    if (m_rearLeftMotor) motors++;
    if (m_rearRightMotor) motors++;
    return motors;
  }
};

}  // namespace frc
