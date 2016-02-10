/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import edu.wpi.first.wpilibj.communication.FRCNetworkCommunicationsLibrary.tInstances;
import edu.wpi.first.wpilibj.communication.FRCNetworkCommunicationsLibrary.tResourceType;
import edu.wpi.first.wpilibj.communication.UsageReporting;

/**
 * Utility class for handling Robot drive based on a definition of the motor
 * configuration. The robot drive class handles basic driving for a robot.
 * Currently, 2 and 4 motor tank and mecanum drive trains are supported. In the
 * future other drive types like swerve might be implemented. Motor channel
 * numbers are supplied on creation of the class. Those are used for either the
 * drive function (intended for hand created drive code, such as autonomous) or
 * with the Tank/Arcade functions intended to be used for Operator Control
 * driving.
 */
public class RobotDrive implements MotorSafety {

  protected MotorSafetyHelper m_safetyHelper;

  /**
   * The location of a motor on the robot for the purpose of driving
   */
  public static class MotorType {

    /**
     * The integer value representing this enumeration
     */
    public final int value;
    static final int kFrontLeft_val = 0;
    static final int kFrontRight_val = 1;
    static final int kRearLeft_val = 2;
    static final int kRearRight_val = 3;
    /**
     * motortype: front left
     */
    public static final MotorType kFrontLeft = new MotorType(kFrontLeft_val);
    /**
     * motortype: front right
     */
    public static final MotorType kFrontRight = new MotorType(kFrontRight_val);
    /**
     * motortype: rear left
     */
    public static final MotorType kRearLeft = new MotorType(kRearLeft_val);
    /**
     * motortype: rear right
     */
    public static final MotorType kRearRight = new MotorType(kRearRight_val);

    private MotorType(int value) {
      this.value = value;
    }
  }

  public static final double kDefaultExpirationTime = 0.1;
  public static final double kDefaultSensitivity = 0.5;
  public static final double kDefaultMaxOutput = 1.0;
  protected static final int kMaxNumberOfMotors = 4;
  protected double m_sensitivity;
  protected double m_maxOutput;
  protected SpeedController m_frontLeftMotor;
  protected SpeedController m_frontRightMotor;
  protected SpeedController m_rearLeftMotor;
  protected SpeedController m_rearRightMotor;
  protected boolean m_allocatedSpeedControllers;
  protected byte m_syncGroup = 0;
  protected static boolean kArcadeRatioCurve_Reported = false;
  protected static boolean kTank_Reported = false;
  protected static boolean kArcadeStandard_Reported = false;
  protected static boolean kMecanumCartesian_Reported = false;
  protected static boolean kMecanumPolar_Reported = false;

  /**
   * Constructor for RobotDrive with 2 motors specified with channel numbers.
   * Set up parameters for a two wheel drive system where the left and right
   * motor pwm channels are specified in the call. This call assumes Talons for
   * controlling the motors.
   *$
   * @param leftMotorChannel The PWM channel number that drives the left motor.
   * @param rightMotorChannel The PWM channel number that drives the right
   *        motor.
   */
  public RobotDrive(final int leftMotorChannel, final int rightMotorChannel) {
    m_sensitivity = kDefaultSensitivity;
    m_maxOutput = kDefaultMaxOutput;
    m_frontLeftMotor = null;
    m_rearLeftMotor = new Talon(leftMotorChannel);
    m_frontRightMotor = null;
    m_rearRightMotor = new Talon(rightMotorChannel);
    m_allocatedSpeedControllers = true;
    setupMotorSafety();
    drive(0, 0);
  }

  /**
   * Constructor for RobotDrive with 4 motors specified with channel numbers.
   * Set up parameters for a four wheel drive system where all four motor pwm
   * channels are specified in the call. This call assumes Talons for
   * controlling the motors.
   *$
   * @param frontLeftMotor Front left motor channel number
   * @param rearLeftMotor Rear Left motor channel number
   * @param frontRightMotor Front right motor channel number
   * @param rearRightMotor Rear Right motor channel number
   */
  public RobotDrive(final int frontLeftMotor, final int rearLeftMotor, final int frontRightMotor,
      final int rearRightMotor) {
    m_sensitivity = kDefaultSensitivity;
    m_maxOutput = kDefaultMaxOutput;
    m_rearLeftMotor = new Talon(rearLeftMotor);
    m_rearRightMotor = new Talon(rearRightMotor);
    m_frontLeftMotor = new Talon(frontLeftMotor);
    m_frontRightMotor = new Talon(frontRightMotor);
    m_allocatedSpeedControllers = true;
    setupMotorSafety();
    drive(0, 0);
  }

  /**
   * Constructor for RobotDrive with 2 motors specified as SpeedController
   * objects. The SpeedController version of the constructor enables programs to
   * use the RobotDrive classes with subclasses of the SpeedController objects,
   * for example, versions with ramping or reshaping of the curve to suit motor
   * bias or dead-band elimination.
   *$
   * @param leftMotor The left SpeedController object used to drive the robot.
   * @param rightMotor the right SpeedController object used to drive the robot.
   */
  public RobotDrive(SpeedController leftMotor, SpeedController rightMotor) {
    if (leftMotor == null || rightMotor == null) {
      m_rearLeftMotor = m_rearRightMotor = null;
      throw new NullPointerException("Null motor provided");
    }
    m_frontLeftMotor = null;
    m_rearLeftMotor = leftMotor;
    m_frontRightMotor = null;
    m_rearRightMotor = rightMotor;
    m_sensitivity = kDefaultSensitivity;
    m_maxOutput = kDefaultMaxOutput;
    m_allocatedSpeedControllers = false;
    setupMotorSafety();
    drive(0, 0);
  }

  /**
   * Constructor for RobotDrive with 4 motors specified as SpeedController
   * objects. Speed controller input version of RobotDrive (see previous
   * comments).
   *$
   * @param rearLeftMotor The back left SpeedController object used to drive the
   *        robot.
   * @param frontLeftMotor The front left SpeedController object used to drive
   *        the robot
   * @param rearRightMotor The back right SpeedController object used to drive
   *        the robot.
   * @param frontRightMotor The front right SpeedController object used to drive
   *        the robot.
   */
  public RobotDrive(SpeedController frontLeftMotor, SpeedController rearLeftMotor,
      SpeedController frontRightMotor, SpeedController rearRightMotor) {
    if (frontLeftMotor == null || rearLeftMotor == null || frontRightMotor == null
        || rearRightMotor == null) {
      m_frontLeftMotor = m_rearLeftMotor = m_frontRightMotor = m_rearRightMotor = null;
      throw new NullPointerException("Null motor provided");
    }
    m_frontLeftMotor = frontLeftMotor;
    m_rearLeftMotor = rearLeftMotor;
    m_frontRightMotor = frontRightMotor;
    m_rearRightMotor = rearRightMotor;
    m_sensitivity = kDefaultSensitivity;
    m_maxOutput = kDefaultMaxOutput;
    m_allocatedSpeedControllers = false;
    setupMotorSafety();
    drive(0, 0);
  }

  /**
   * Drive the motors at "outputMagnitude" and "curve".
   * Both outputMagnitude and curve are -1.0 to +1.0 values, where 0.0
   * represents stopped and not turning. {@literal curve < 0 will turn left and curve > 0}
   * will turn right.
   *
   * The algorithm for steering provides a constant turn radius for any normal
   * speed range, both forward and backward. Increasing m_sensitivity causes
   * sharper turns for fixed values of curve.
   *
   * This function will most likely be used in an autonomous routine.
   *
   * @param outputMagnitude The speed setting for the outside wheel in a turn,
   *        forward or backwards, +1 to -1.
   * @param curve The rate of turn, constant for different forward speeds. Set
   *        {@literal curve < 0 for left turn or curve > 0 for right turn.}
   * Set curve = e^(-r/w) to get a turn radius r for wheelbase w of your robot.
   * Conversely, turn radius r = -ln(curve)*w for a given value of curve and
   * wheelbase w.
   */
  public void drive(double outputMagnitude, double curve) {
    double leftOutput, rightOutput;

    if (!kArcadeRatioCurve_Reported) {
      UsageReporting.report(tResourceType.kResourceType_RobotDrive, getNumMotors(),
          tInstances.kRobotDrive_ArcadeRatioCurve);
      kArcadeRatioCurve_Reported = true;
    }
    if (curve < 0) {
      double value = Math.log(-curve);
      double ratio = (value - m_sensitivity) / (value + m_sensitivity);
      if (ratio == 0) {
        ratio = .0000000001;
      }
      leftOutput = outputMagnitude / ratio;
      rightOutput = outputMagnitude;
    } else if (curve > 0) {
      double value = Math.log(curve);
      double ratio = (value - m_sensitivity) / (value + m_sensitivity);
      if (ratio == 0) {
        ratio = .0000000001;
      }
      leftOutput = outputMagnitude;
      rightOutput = outputMagnitude / ratio;
    } else {
      leftOutput = outputMagnitude;
      rightOutput = outputMagnitude;
    }
    setLeftRightMotorOutputs(leftOutput, rightOutput);
  }

  /**
   * Provide tank steering using the stored robot configuration. drive the robot
   * using two joystick inputs. The Y-axis will be selected from each Joystick
   * object.
   *$
   * @param leftStick The joystick to control the left side of the robot.
   * @param rightStick The joystick to control the right side of the robot.
   */
  public void tankDrive(GenericHID leftStick, GenericHID rightStick) {
    if (leftStick == null || rightStick == null) {
      throw new NullPointerException("Null HID provided");
    }
    tankDrive(leftStick.getY(), rightStick.getY(), true);
  }

  /**
   * Provide tank steering using the stored robot configuration. drive the robot
   * using two joystick inputs. The Y-axis will be selected from each Joystick
   * object.
   *$
   * @param leftStick The joystick to control the left side of the robot.
   * @param rightStick The joystick to control the right side of the robot.
   * @param squaredInputs Setting this parameter to true decreases the
   *        sensitivity at lower speeds
   */
  public void tankDrive(GenericHID leftStick, GenericHID rightStick, boolean squaredInputs) {
    if (leftStick == null || rightStick == null) {
      throw new NullPointerException("Null HID provided");
    }
    tankDrive(leftStick.getY(), rightStick.getY(), squaredInputs);
  }

  /**
   * Provide tank steering using the stored robot configuration. This function
   * lets you pick the axis to be used on each Joystick object for the left and
   * right sides of the robot.
   *$
   * @param leftStick The Joystick object to use for the left side of the robot.
   * @param leftAxis The axis to select on the left side Joystick object.
   * @param rightStick The Joystick object to use for the right side of the
   *        robot.
   * @param rightAxis The axis to select on the right side Joystick object.
   */
  public void tankDrive(GenericHID leftStick, final int leftAxis, GenericHID rightStick,
      final int rightAxis) {
    if (leftStick == null || rightStick == null) {
      throw new NullPointerException("Null HID provided");
    }
    tankDrive(leftStick.getRawAxis(leftAxis), rightStick.getRawAxis(rightAxis), true);
  }

  /**
   * Provide tank steering using the stored robot configuration. This function
   * lets you pick the axis to be used on each Joystick object for the left and
   * right sides of the robot.
   *$
   * @param leftStick The Joystick object to use for the left side of the robot.
   * @param leftAxis The axis to select on the left side Joystick object.
   * @param rightStick The Joystick object to use for the right side of the
   *        robot.
   * @param rightAxis The axis to select on the right side Joystick object.
   * @param squaredInputs Setting this parameter to true decreases the
   *        sensitivity at lower speeds
   */
  public void tankDrive(GenericHID leftStick, final int leftAxis, GenericHID rightStick,
      final int rightAxis, boolean squaredInputs) {
    if (leftStick == null || rightStick == null) {
      throw new NullPointerException("Null HID provided");
    }
    tankDrive(leftStick.getRawAxis(leftAxis), rightStick.getRawAxis(rightAxis), squaredInputs);
  }

  /**
   * Provide tank steering using the stored robot configuration. This function
   * lets you directly provide joystick values from any source.
   *$
   * @param leftValue The value of the left stick.
   * @param rightValue The value of the right stick.
   * @param squaredInputs Setting this parameter to true decreases the
   *        sensitivity at lower speeds
   */
  public void tankDrive(double leftValue, double rightValue, boolean squaredInputs) {

    if (!kTank_Reported) {
      UsageReporting.report(tResourceType.kResourceType_RobotDrive, getNumMotors(),
          tInstances.kRobotDrive_Tank);
      kTank_Reported = true;
    }

    // square the inputs (while preserving the sign) to increase fine control
    // while permitting full power
    leftValue = limit(leftValue);
    rightValue = limit(rightValue);
    if (squaredInputs) {
      if (leftValue >= 0.0) {
        leftValue = (leftValue * leftValue);
      } else {
        leftValue = -(leftValue * leftValue);
      }
      if (rightValue >= 0.0) {
        rightValue = (rightValue * rightValue);
      } else {
        rightValue = -(rightValue * rightValue);
      }
    }
    setLeftRightMotorOutputs(leftValue, rightValue);
  }

  /**
   * Provide tank steering using the stored robot configuration. This function
   * lets you directly provide joystick values from any source.
   *$
   * @param leftValue The value of the left stick.
   * @param rightValue The value of the right stick.
   */
  public void tankDrive(double leftValue, double rightValue) {
    tankDrive(leftValue, rightValue, true);
  }

  /**
   * Arcade drive implements single stick driving. Given a single Joystick, the
   * class assumes the Y axis for the move value and the X axis for the rotate
   * value. (Should add more information here regarding the way that arcade
   * drive works.)
   *$
   * @param stick The joystick to use for Arcade single-stick driving. The
   *        Y-axis will be selected for forwards/backwards and the X-axis will
   *        be selected for rotation rate.
   * @param squaredInputs If true, the sensitivity will be decreased for small
   *        values
   */
  public void arcadeDrive(GenericHID stick, boolean squaredInputs) {
    // simply call the full-featured arcadeDrive with the appropriate values
    arcadeDrive(stick.getY(), stick.getX(), squaredInputs);
  }

  /**
   * Arcade drive implements single stick driving. Given a single Joystick, the
   * class assumes the Y axis for the move value and the X axis for the rotate
   * value. (Should add more information here regarding the way that arcade
   * drive works.)
   *$
   * @param stick The joystick to use for Arcade single-stick driving. The
   *        Y-axis will be selected for forwards/backwards and the X-axis will
   *        be selected for rotation rate.
   */
  public void arcadeDrive(GenericHID stick) {
    this.arcadeDrive(stick, true);
  }

  /**
   * Arcade drive implements single stick driving. Given two joystick instances
   * and two axis, compute the values to send to either two or four motors.
   *$
   * @param moveStick The Joystick object that represents the forward/backward
   *        direction
   * @param moveAxis The axis on the moveStick object to use for
   *        forwards/backwards (typically Y_AXIS)
   * @param rotateStick The Joystick object that represents the rotation value
   * @param rotateAxis The axis on the rotation object to use for the rotate
   *        right/left (typically X_AXIS)
   * @param squaredInputs Setting this parameter to true decreases the
   *        sensitivity at lower speeds
   */
  public void arcadeDrive(GenericHID moveStick, final int moveAxis, GenericHID rotateStick,
      final int rotateAxis, boolean squaredInputs) {
    double moveValue = moveStick.getRawAxis(moveAxis);
    double rotateValue = rotateStick.getRawAxis(rotateAxis);

    arcadeDrive(moveValue, rotateValue, squaredInputs);
  }

  /**
   * Arcade drive implements single stick driving. Given two joystick instances
   * and two axis, compute the values to send to either two or four motors.
   *$
   * @param moveStick The Joystick object that represents the forward/backward
   *        direction
   * @param moveAxis The axis on the moveStick object to use for
   *        forwards/backwards (typically Y_AXIS)
   * @param rotateStick The Joystick object that represents the rotation value
   * @param rotateAxis The axis on the rotation object to use for the rotate
   *        right/left (typically X_AXIS)
   */
  public void arcadeDrive(GenericHID moveStick, final int moveAxis, GenericHID rotateStick,
      final int rotateAxis) {
    this.arcadeDrive(moveStick, moveAxis, rotateStick, rotateAxis, true);
  }

  /**
   * Arcade drive implements single stick driving. This function lets you
   * directly provide joystick values from any source.
   *$
   * @param moveValue The value to use for forwards/backwards
   * @param rotateValue The value to use for the rotate right/left
   * @param squaredInputs If set, decreases the sensitivity at low speeds
   */
  public void arcadeDrive(double moveValue, double rotateValue, boolean squaredInputs) {
    // local variables to hold the computed PWM values for the motors
    if (!kArcadeStandard_Reported) {
      UsageReporting.report(tResourceType.kResourceType_RobotDrive, getNumMotors(),
          tInstances.kRobotDrive_ArcadeStandard);
      kArcadeStandard_Reported = true;
    }

    double leftMotorSpeed;
    double rightMotorSpeed;

    moveValue = limit(moveValue);
    rotateValue = limit(rotateValue);

    if (squaredInputs) {
      // square the inputs (while preserving the sign) to increase fine control
      // while permitting full power
      if (moveValue >= 0.0) {
        moveValue = (moveValue * moveValue);
      } else {
        moveValue = -(moveValue * moveValue);
      }
      if (rotateValue >= 0.0) {
        rotateValue = (rotateValue * rotateValue);
      } else {
        rotateValue = -(rotateValue * rotateValue);
      }
    }

    if (moveValue > 0.0) {
      if (rotateValue > 0.0) {
        leftMotorSpeed = moveValue - rotateValue;
        rightMotorSpeed = Math.max(moveValue, rotateValue);
      } else {
        leftMotorSpeed = Math.max(moveValue, -rotateValue);
        rightMotorSpeed = moveValue + rotateValue;
      }
    } else {
      if (rotateValue > 0.0) {
        leftMotorSpeed = -Math.max(-moveValue, rotateValue);
        rightMotorSpeed = moveValue + rotateValue;
      } else {
        leftMotorSpeed = moveValue - rotateValue;
        rightMotorSpeed = -Math.max(-moveValue, -rotateValue);
      }
    }

    setLeftRightMotorOutputs(leftMotorSpeed, rightMotorSpeed);
  }

  /**
   * Arcade drive implements single stick driving. This function lets you
   * directly provide joystick values from any source.
   *$
   * @param moveValue The value to use for fowards/backwards
   * @param rotateValue The value to use for the rotate right/left
   */
  public void arcadeDrive(double moveValue, double rotateValue) {
    this.arcadeDrive(moveValue, rotateValue, true);
  }

  /**
   * Drive method for Mecanum wheeled robots.
   *
   * A method for driving with Mecanum wheeled robots. There are 4 wheels on the
   * robot, arranged so that the front and back wheels are toed in 45 degrees.
   * When looking at the wheels from the top, the roller axles should form an X
   * across the robot.
   *
   * This is designed to be directly driven by joystick axes.
   *
   * @param x The speed that the robot should drive in the X direction.
   *        [-1.0..1.0]
   * @param y The speed that the robot should drive in the Y direction. This
   *        input is inverted to match the forward == -1.0 that joysticks
   *        produce. [-1.0..1.0]
   * @param rotation The rate of rotation for the robot that is completely
   *        independent of the translation. [-1.0..1.0]
   * @param gyroAngle The current angle reading from the gyro. Use this to
   *        implement field-oriented controls.
   */
  public void mecanumDrive_Cartesian(double x, double y, double rotation, double gyroAngle) {
    if (!kMecanumCartesian_Reported) {
      UsageReporting.report(tResourceType.kResourceType_RobotDrive, getNumMotors(),
          tInstances.kRobotDrive_MecanumCartesian);
      kMecanumCartesian_Reported = true;
    }
    double xIn = x;
    double yIn = y;
    // Negate y for the joystick.
    yIn = -yIn;
    // Compenstate for gyro angle.
    double rotated[] = rotateVector(xIn, yIn, gyroAngle);
    xIn = rotated[0];
    yIn = rotated[1];

    double wheelSpeeds[] = new double[kMaxNumberOfMotors];
    wheelSpeeds[MotorType.kFrontLeft_val] = xIn + yIn + rotation;
    wheelSpeeds[MotorType.kFrontRight_val] = -xIn + yIn - rotation;
    wheelSpeeds[MotorType.kRearLeft_val] = -xIn + yIn + rotation;
    wheelSpeeds[MotorType.kRearRight_val] = xIn + yIn - rotation;

    normalize(wheelSpeeds);
    m_frontLeftMotor.set(wheelSpeeds[MotorType.kFrontLeft_val] * m_maxOutput, m_syncGroup);
    m_frontRightMotor.set(wheelSpeeds[MotorType.kFrontRight_val] * m_maxOutput, m_syncGroup);
    m_rearLeftMotor.set(wheelSpeeds[MotorType.kRearLeft_val] * m_maxOutput, m_syncGroup);
    m_rearRightMotor.set(wheelSpeeds[MotorType.kRearRight_val] * m_maxOutput, m_syncGroup);

    if (m_syncGroup != 0) {
      CANJaguar.updateSyncGroup(m_syncGroup);
    }

    if (m_safetyHelper != null)
      m_safetyHelper.feed();
  }

  /**
   * Drive method for Mecanum wheeled robots.
   *
   * A method for driving with Mecanum wheeled robots. There are 4 wheels on the
   * robot, arranged so that the front and back wheels are toed in 45 degrees.
   * When looking at the wheels from the top, the roller axles should form an X
   * across the robot.
   *
   * @param magnitude The speed that the robot should drive in a given
   *        direction.
   * @param direction The direction the robot should drive in degrees. The
   *        direction and maginitute are independent of the rotation rate.
   * @param rotation The rate of rotation for the robot that is completely
   *        independent of the magnitute or direction. [-1.0..1.0]
   */
  public void mecanumDrive_Polar(double magnitude, double direction, double rotation) {
    if (!kMecanumPolar_Reported) {
      UsageReporting.report(tResourceType.kResourceType_RobotDrive, getNumMotors(),
          tInstances.kRobotDrive_MecanumPolar);
      kMecanumPolar_Reported = true;
    }
    double frontLeftSpeed, rearLeftSpeed, frontRightSpeed, rearRightSpeed;
    // Normalized for full power along the Cartesian axes.
    magnitude = limit(magnitude) * Math.sqrt(2.0);
    // The rollers are at 45 degree angles.
    double dirInRad = (direction + 45.0) * 3.14159 / 180.0;
    double cosD = Math.cos(dirInRad);
    double sinD = Math.sin(dirInRad);

    double wheelSpeeds[] = new double[kMaxNumberOfMotors];
    wheelSpeeds[MotorType.kFrontLeft_val] = (sinD * magnitude + rotation);
    wheelSpeeds[MotorType.kFrontRight_val] = (cosD * magnitude - rotation);
    wheelSpeeds[MotorType.kRearLeft_val] = (cosD * magnitude + rotation);
    wheelSpeeds[MotorType.kRearRight_val] = (sinD * magnitude - rotation);

    normalize(wheelSpeeds);

    m_frontLeftMotor.set(wheelSpeeds[MotorType.kFrontLeft_val] * m_maxOutput, m_syncGroup);
    m_frontRightMotor.set(wheelSpeeds[MotorType.kFrontRight_val] * m_maxOutput, m_syncGroup);
    m_rearLeftMotor.set(wheelSpeeds[MotorType.kRearLeft_val] * m_maxOutput, m_syncGroup);
    m_rearRightMotor.set(wheelSpeeds[MotorType.kRearRight_val] * m_maxOutput, m_syncGroup);

    if (this.m_syncGroup != 0) {
      CANJaguar.updateSyncGroup(m_syncGroup);
    }

    if (m_safetyHelper != null)
      m_safetyHelper.feed();
  }

  /**
   * Holonomic Drive method for Mecanum wheeled robots.
   *
   * This is an alias to mecanumDrive_Polar() for backward compatability
   *
   * @param magnitude The speed that the robot should drive in a given
   *        direction. [-1.0..1.0]
   * @param direction The direction the robot should drive. The direction and
   *        maginitute are independent of the rotation rate.
   * @param rotation The rate of rotation for the robot that is completely
   *        independent of the magnitute or direction. [-1.0..1.0]
   */
  void holonomicDrive(float magnitude, float direction, float rotation) {
    mecanumDrive_Polar(magnitude, direction, rotation);
  }

  /**
   * Set the speed of the right and left motors. This is used once an
   * appropriate drive setup function is called such as twoWheelDrive(). The
   * motors are set to "leftSpeed" and "rightSpeed" and includes flipping the
   * direction of one side for opposing motors.
   *$
   * @param leftOutput The speed to send to the left side of the robot.
   * @param rightOutput The speed to send to the right side of the robot.
   */
  public void setLeftRightMotorOutputs(double leftOutput, double rightOutput) {
    if (m_rearLeftMotor == null || m_rearRightMotor == null) {
      throw new NullPointerException("Null motor provided");
    }

    if (m_frontLeftMotor != null) {
      m_frontLeftMotor.set(limit(leftOutput) * m_maxOutput, m_syncGroup);
    }
    m_rearLeftMotor.set(limit(leftOutput) * m_maxOutput, m_syncGroup);

    if (m_frontRightMotor != null) {
      m_frontRightMotor.set(-limit(rightOutput) * m_maxOutput, m_syncGroup);
    }
    m_rearRightMotor.set(-limit(rightOutput) * m_maxOutput, m_syncGroup);

    if (this.m_syncGroup != 0) {
      CANJaguar.updateSyncGroup(m_syncGroup);
    }

    if (m_safetyHelper != null)
      m_safetyHelper.feed();
  }

  /**
   * Limit motor values to the -1.0 to +1.0 range.
   */
  protected static double limit(double num) {
    if (num > 1.0) {
      return 1.0;
    }
    if (num < -1.0) {
      return -1.0;
    }
    return num;
  }

  /**
   * Normalize all wheel speeds if the magnitude of any wheel is greater than
   * 1.0.
   */
  protected static void normalize(double wheelSpeeds[]) {
    double maxMagnitude = Math.abs(wheelSpeeds[0]);
    int i;
    for (i = 1; i < kMaxNumberOfMotors; i++) {
      double temp = Math.abs(wheelSpeeds[i]);
      if (maxMagnitude < temp)
        maxMagnitude = temp;
    }
    if (maxMagnitude > 1.0) {
      for (i = 0; i < kMaxNumberOfMotors; i++) {
        wheelSpeeds[i] = wheelSpeeds[i] / maxMagnitude;
      }
    }
  }

  /**
   * Rotate a vector in Cartesian space.
   */
  protected static double[] rotateVector(double x, double y, double angle) {
    double cosA = Math.cos(angle * (3.14159 / 180.0));
    double sinA = Math.sin(angle * (3.14159 / 180.0));
    double out[] = new double[2];
    out[0] = x * cosA - y * sinA;
    out[1] = x * sinA + y * cosA;
    return out;
  }

  /**
   * Invert a motor direction. This is used when a motor should run in the
   * opposite direction as the drive code would normally run it. Motors that are
   * direct drive would be inverted, the drive code assumes that the motors are
   * geared with one reversal.
   *$
   * @param motor The motor index to invert.
   * @param isInverted True if the motor should be inverted when operated.
   */
  public void setInvertedMotor(MotorType motor, boolean isInverted) {
    switch (motor.value) {
      case MotorType.kFrontLeft_val:
        m_frontLeftMotor.setInverted(isInverted);
        break;
      case MotorType.kFrontRight_val:
        m_frontRightMotor.setInverted(isInverted);
        break;
      case MotorType.kRearLeft_val:
        m_rearLeftMotor.setInverted(isInverted);
        break;
      case MotorType.kRearRight_val:
        m_rearRightMotor.setInverted(isInverted);
        break;
    }
  }

  /**
   * Set the turning sensitivity.
   *
   * This only impacts the drive() entry-point.
   *$
   * @param sensitivity Effectively sets the turning sensitivity (or turn radius
   *        for a given value)
   */
  public void setSensitivity(double sensitivity) {
    m_sensitivity = sensitivity;
  }

  /**
   * Configure the scaling factor for using RobotDrive with motor controllers in
   * a mode other than PercentVbus.
   *$
   * @param maxOutput Multiplied with the output percentage computed by the
   *        drive functions.
   */
  public void setMaxOutput(double maxOutput) {
    m_maxOutput = maxOutput;
  }

  /**
   * Set the number of the sync group for the motor controllers. If the motor
   * controllers are {@link CANJaguar}s, then they will all be added to this
   * sync group, causing them to update their values at the same time.
   *
   * @param syncGroup the update group to add the motor controllers to
   */
  public void setCANJaguarSyncGroup(byte syncGroup) {
    m_syncGroup = syncGroup;
  }

  /**
   * Free the speed controllers if they were allocated locally
   */
  public void free() {
    if (m_allocatedSpeedControllers) {
      if (m_frontLeftMotor != null) {
        ((PWM) m_frontLeftMotor).free();
      }
      if (m_frontRightMotor != null) {
        ((PWM) m_frontRightMotor).free();
      }
      if (m_rearLeftMotor != null) {
        ((PWM) m_rearLeftMotor).free();
      }
      if (m_rearRightMotor != null) {
        ((PWM) m_rearRightMotor).free();
      }
    }
  }

  public void setExpiration(double timeout) {
    m_safetyHelper.setExpiration(timeout);
  }

  public double getExpiration() {
    return m_safetyHelper.getExpiration();
  }

  public boolean isAlive() {
    return m_safetyHelper.isAlive();
  }

  public boolean isSafetyEnabled() {
    return m_safetyHelper.isSafetyEnabled();
  }

  public void setSafetyEnabled(boolean enabled) {
    m_safetyHelper.setSafetyEnabled(enabled);
  }

  public String getDescription() {
    return "Robot Drive";
  }

  public void stopMotor() {
    if (m_frontLeftMotor != null) {
      m_frontLeftMotor.stopMotor();
    }
    if (m_frontRightMotor != null) {
      m_frontRightMotor.stopMotor();
    }
    if (m_rearLeftMotor != null) {
      m_rearLeftMotor.stopMotor();
    }
    if (m_rearRightMotor != null) {
      m_rearRightMotor.stopMotor();
    }
    if (m_safetyHelper != null)
      m_safetyHelper.feed();
  }

  private void setupMotorSafety() {
    m_safetyHelper = new MotorSafetyHelper(this);
    m_safetyHelper.setExpiration(kDefaultExpirationTime);
    m_safetyHelper.setSafetyEnabled(true);
  }

  protected int getNumMotors() {
    int motors = 0;
    if (m_frontLeftMotor != null)
      motors++;
    if (m_frontRightMotor != null)
      motors++;
    if (m_rearLeftMotor != null)
      motors++;
    if (m_rearRightMotor != null)
      motors++;
    return motors;
  }
}
