/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "RobotDrive.h"

#include "CANJaguar.h"
#include "GenericHID.h"
#include "Joystick.h"
#include "Talon.h"
//#include "NetworkCommunication/UsageReporting.h"
#include "Utility.h"
#include "WPIErrors.h"
#include <math.h>

#undef max
#include <algorithm>

const int32_t RobotDrive::kMaxNumberOfMotors;

/*
 * Driving functions
 * These functions provide an interface to multiple motors that is used for C programming
 * The Drive(speed, direction) function is the main part of the set that makes it easy
 * to set speeds and direction independently in one call.
 */

/**
 * Common function to initialize all the robot drive constructors.
 * Create a motor safety object (the real reason for the common code) and
 * initialize all the motor assignments. The default timeout is set for the robot drive.
 */
void RobotDrive::InitRobotDrive() {
	m_frontLeftMotor = NULL;
	m_frontRightMotor = NULL;
	m_rearRightMotor = NULL;
	m_rearLeftMotor = NULL;
	m_sensitivity = 0.5;
	m_maxOutput = 1.0;
	m_safetyHelper = new MotorSafetyHelper(this);
	m_safetyHelper->SetSafetyEnabled(true);
	m_syncGroup = 0;
}

/** 
 * Constructor for RobotDrive with 2 motors specified with channel numbers.
 * Set up parameters for a two wheel drive system where the
 * left and right motor pwm channels are specified in the call.
 * This call assumes Talons for controlling the motors.
 * @param leftMotorChannel The PWM channel number that drives the left motor. 0-9 are on-board, 10-19 are on the MXP port
 * @param rightMotorChannel The PWM channel number that drives the right motor. 0-9 are on-board, 10-19 are on the MXP port
 */
RobotDrive::RobotDrive(uint32_t leftMotorChannel, uint32_t rightMotorChannel)
{
	InitRobotDrive();
	m_rearLeftMotor = new Talon(leftMotorChannel);
	m_rearRightMotor = new Talon(rightMotorChannel);
	for (int32_t i=0; i < kMaxNumberOfMotors; i++)
	{
		m_invertedMotors[i] = 1;
	}
	SetLeftRightMotorOutputs(0.0, 0.0);
	m_deleteSpeedControllers = true;
}

/**
 * Constructor for RobotDrive with 4 motors specified with channel numbers.
 * Set up parameters for a four wheel drive system where all four motor
 * pwm channels are specified in the call.
 * This call assumes Talons for controlling the motors.
 * @param frontLeftMotor Front left motor channel number. 0-9 are on-board, 10-19 are on the MXP port
 * @param rearLeftMotor Rear Left motor channel number. 0-9 are on-board, 10-19 are on the MXP port
 * @param frontRightMotor Front right motor channel number. 0-9 are on-board, 10-19 are on the MXP port
 * @param rearRightMotor Rear Right motor channel number. 0-9 are on-board, 10-19 are on the MXP port
 */
RobotDrive::RobotDrive(uint32_t frontLeftMotor, uint32_t rearLeftMotor,
		uint32_t frontRightMotor, uint32_t rearRightMotor)
{
	InitRobotDrive();
	m_rearLeftMotor = new Talon(rearLeftMotor);
	m_rearRightMotor = new Talon(rearRightMotor);
	m_frontLeftMotor = new Talon(frontLeftMotor);
	m_frontRightMotor = new Talon(frontRightMotor);
	for (int32_t i=0; i < kMaxNumberOfMotors; i++)
	{
		m_invertedMotors[i] = 1;
	}
	SetLeftRightMotorOutputs(0.0, 0.0);
	m_deleteSpeedControllers = true;
}

/**
 * Constructor for RobotDrive with 2 motors specified as SpeedController objects.
 * The SpeedController version of the constructor enables programs to use the RobotDrive classes with
 * subclasses of the SpeedController objects, for example, versions with ramping or reshaping of
 * the curve to suit motor bias or deadband elimination.
 * @param leftMotor The left SpeedController object used to drive the robot. 
 * @param rightMotor the right SpeedController object used to drive the robot.
 */
RobotDrive::RobotDrive(SpeedController *leftMotor, SpeedController *rightMotor)
{
	InitRobotDrive();
	if (leftMotor == NULL || rightMotor == NULL)
	{
		wpi_setWPIError(NullParameter);
		m_rearLeftMotor = m_rearRightMotor = NULL;
		return;
	}
	m_rearLeftMotor = leftMotor;
	m_rearRightMotor = rightMotor;
	for (int32_t i=0; i < kMaxNumberOfMotors; i++)
	{
		m_invertedMotors[i] = 1;
	}
	m_deleteSpeedControllers = false;
}

RobotDrive::RobotDrive(SpeedController &leftMotor, SpeedController &rightMotor)
{
	InitRobotDrive();
	m_rearLeftMotor = &leftMotor;
	m_rearRightMotor = &rightMotor;
	for (int32_t i=0; i < kMaxNumberOfMotors; i++)
	{
		m_invertedMotors[i] = 1;
	}
	m_deleteSpeedControllers = false;
}

/**
 * Constructor for RobotDrive with 4 motors specified as SpeedController objects.
 * Speed controller input version of RobotDrive (see previous comments).
 * @param rearLeftMotor The back left SpeedController object used to drive the robot.
 * @param frontLeftMotor The front left SpeedController object used to drive the robot
 * @param rearRightMotor The back right SpeedController object used to drive the robot.
 * @param frontRightMotor The front right SpeedController object used to drive the robot.
 */
RobotDrive::RobotDrive(SpeedController *frontLeftMotor, SpeedController *rearLeftMotor,
						SpeedController *frontRightMotor, SpeedController *rearRightMotor)
{
	InitRobotDrive();
	if (frontLeftMotor == NULL || rearLeftMotor == NULL || frontRightMotor == NULL || rearRightMotor == NULL)
	{
		wpi_setWPIError(NullParameter);
		return;
	}
	m_frontLeftMotor = frontLeftMotor;
	m_rearLeftMotor = rearLeftMotor;
	m_frontRightMotor = frontRightMotor;
	m_rearRightMotor = rearRightMotor;
	for (int32_t i=0; i < kMaxNumberOfMotors; i++)
	{
		m_invertedMotors[i] = 1;
	}
	m_deleteSpeedControllers = false;
}

RobotDrive::RobotDrive(SpeedController &frontLeftMotor, SpeedController &rearLeftMotor,
						SpeedController &frontRightMotor, SpeedController &rearRightMotor)
{
	InitRobotDrive();
	m_frontLeftMotor = &frontLeftMotor;
	m_rearLeftMotor = &rearLeftMotor;
	m_frontRightMotor = &frontRightMotor;
	m_rearRightMotor = &rearRightMotor;
	for (int32_t i=0; i < kMaxNumberOfMotors; i++)
	{
		m_invertedMotors[i] = 1;
	}
	m_deleteSpeedControllers = false;
}

/**
 * RobotDrive destructor.
 * Deletes motor objects that were not passed in and created internally only.
 **/
RobotDrive::~RobotDrive()
{
	if (m_deleteSpeedControllers)
	{
		delete m_frontLeftMotor;
		delete m_rearLeftMotor;
		delete m_frontRightMotor;
		delete m_rearRightMotor;
	}
	delete m_safetyHelper;
}

/**
 * Drive the motors at "speed" and "curve".
 *
 * The speed and curve are -1.0 to +1.0 values where 0.0 represents stopped and
 * not turning. The algorithm for adding in the direction attempts to provide a constant
 * turn radius for differing speeds.
 *
 * This function will most likely be used in an autonomous routine.
 *
 * @param outputMagnitude The forward component of the output magnitude to send to the motors.
 * @param curve The rate of turn, constant for different forward speeds.
 */
void RobotDrive::Drive(float outputMagnitude, float curve)
{
	float leftOutput, rightOutput;
	static bool reported = false;
	if (!reported)
	{
		HALReport(HALUsageReporting::kResourceType_RobotDrive, GetNumMotors(), HALUsageReporting::kRobotDrive_ArcadeRatioCurve);
		reported = true;
	}

	if (curve < 0)
	{
		float value = log(-curve);
		float ratio = (value - m_sensitivity)/(value + m_sensitivity);
		if (ratio == 0) ratio =.0000000001;
		leftOutput = outputMagnitude / ratio;
		rightOutput = outputMagnitude;
	}
	else if (curve > 0)
	{
		float value = log(curve);
		float ratio = (value - m_sensitivity)/(value + m_sensitivity);
		if (ratio == 0) ratio =.0000000001;
		leftOutput = outputMagnitude;
		rightOutput = outputMagnitude / ratio;
	}
	else
	{
		leftOutput = outputMagnitude;
		rightOutput = outputMagnitude;
	}
	SetLeftRightMotorOutputs(leftOutput, rightOutput);
}

/**
 * Provide tank steering using the stored robot configuration.
 * Drive the robot using two joystick inputs. The Y-axis will be selected from
 * each Joystick object.
 * @param leftStick The joystick to control the left side of the robot.
 * @param rightStick The joystick to control the right side of the robot.
 */
void RobotDrive::TankDrive(GenericHID *leftStick, GenericHID *rightStick, bool squaredInputs)
{
	if (leftStick == NULL || rightStick == NULL)
	{
		wpi_setWPIError(NullParameter);
		return;
	}
	TankDrive(leftStick->GetY(), rightStick->GetY(), squaredInputs);
}

void RobotDrive::TankDrive(GenericHID &leftStick, GenericHID &rightStick, bool squaredInputs)
{
	TankDrive(leftStick.GetY(), rightStick.GetY(), squaredInputs);
}

/**
 * Provide tank steering using the stored robot configuration.
 * This function lets you pick the axis to be used on each Joystick object for the left
 * and right sides of the robot.
 * @param leftStick The Joystick object to use for the left side of the robot.
 * @param leftAxis The axis to select on the left side Joystick object.
 * @param rightStick The Joystick object to use for the right side of the robot.
 * @param rightAxis The axis to select on the right side Joystick object.
 */
void RobotDrive::TankDrive(GenericHID *leftStick, uint32_t leftAxis,
		GenericHID *rightStick, uint32_t rightAxis, bool squaredInputs)
{
	if (leftStick == NULL || rightStick == NULL)
	{
		wpi_setWPIError(NullParameter);
		return;
	}
	TankDrive(leftStick->GetRawAxis(leftAxis), rightStick->GetRawAxis(rightAxis), squaredInputs);
}

void RobotDrive::TankDrive(GenericHID &leftStick, uint32_t leftAxis,
		GenericHID &rightStick, uint32_t rightAxis, bool squaredInputs)
{
	TankDrive(leftStick.GetRawAxis(leftAxis), rightStick.GetRawAxis(rightAxis), squaredInputs);
}


/**
 * Provide tank steering using the stored robot configuration.
 * This function lets you directly provide joystick values from any source.
 * @param leftValue The value of the left stick.
 * @param rightValue The value of the right stick.
 */
void RobotDrive::TankDrive(float leftValue, float rightValue, bool squaredInputs)
{
	static bool reported = false;
	if (!reported)
	{
		HALReport(HALUsageReporting::kResourceType_RobotDrive, GetNumMotors(), HALUsageReporting::kRobotDrive_Tank);
		reported = true;
	}

	// square the inputs (while preserving the sign) to increase fine control while permitting full power
	leftValue = Limit(leftValue);
	rightValue = Limit(rightValue);
	if(squaredInputs)
	{
		if (leftValue >= 0.0)
		{
			leftValue = (leftValue * leftValue);
		}
		else
		{
			leftValue = -(leftValue * leftValue);
		}
		if (rightValue >= 0.0)
		{
			rightValue = (rightValue * rightValue);
		}
		else
		{
			rightValue = -(rightValue * rightValue);
		}
	}

	SetLeftRightMotorOutputs(leftValue, rightValue);
}

/**
 * Arcade drive implements single stick driving.
 * Given a single Joystick, the class assumes the Y axis for the move value and the X axis
 * for the rotate value.
 * (Should add more information here regarding the way that arcade drive works.)
 * @param stick The joystick to use for Arcade single-stick driving. The Y-axis will be selected
 * for forwards/backwards and the X-axis will be selected for rotation rate.
 * @param squaredInputs If true, the sensitivity will be increased for small values
 */
void RobotDrive::ArcadeDrive(GenericHID *stick, bool squaredInputs)
{
	// simply call the full-featured ArcadeDrive with the appropriate values
	ArcadeDrive(stick->GetY(), stick->GetX(), squaredInputs);
}

/**
 * Arcade drive implements single stick driving.
 * Given a single Joystick, the class assumes the Y axis for the move value and the X axis
 * for the rotate value.
 * (Should add more information here regarding the way that arcade drive works.)
 * @param stick The joystick to use for Arcade single-stick driving. The Y-axis will be selected
 * for forwards/backwards and the X-axis will be selected for rotation rate.
 * @param squaredInputs If true, the sensitivity will be increased for small values
 */
void RobotDrive::ArcadeDrive(GenericHID &stick, bool squaredInputs)
{
	// simply call the full-featured ArcadeDrive with the appropriate values
	ArcadeDrive(stick.GetY(), stick.GetX(), squaredInputs);
}

/**
 * Arcade drive implements single stick driving.
 * Given two joystick instances and two axis, compute the values to send to either two
 * or four motors.
 * @param moveStick The Joystick object that represents the forward/backward direction
 * @param moveAxis The axis on the moveStick object to use for fowards/backwards (typically Y_AXIS)
 * @param rotateStick The Joystick object that represents the rotation value
 * @param rotateAxis The axis on the rotation object to use for the rotate right/left (typically X_AXIS)
 * @param squaredInputs Setting this parameter to true increases the sensitivity at lower speeds
 */
void RobotDrive::ArcadeDrive(GenericHID* moveStick, uint32_t moveAxis,
								GenericHID* rotateStick, uint32_t rotateAxis,
								bool squaredInputs)
{
	float moveValue = moveStick->GetRawAxis(moveAxis);
	float rotateValue = rotateStick->GetRawAxis(rotateAxis);

	ArcadeDrive(moveValue, rotateValue, squaredInputs);
}

/**
 * Arcade drive implements single stick driving.
 * Given two joystick instances and two axis, compute the values to send to either two
 * or four motors.
 * @param moveStick The Joystick object that represents the forward/backward direction
 * @param moveAxis The axis on the moveStick object to use for fowards/backwards (typically Y_AXIS)
 * @param rotateStick The Joystick object that represents the rotation value
 * @param rotateAxis The axis on the rotation object to use for the rotate right/left (typically X_AXIS)
 * @param squaredInputs Setting this parameter to true increases the sensitivity at lower speeds
 */

void RobotDrive::ArcadeDrive(GenericHID &moveStick, uint32_t moveAxis,
								GenericHID &rotateStick, uint32_t rotateAxis,
								bool squaredInputs)
{
	float moveValue = moveStick.GetRawAxis(moveAxis);
	float rotateValue = rotateStick.GetRawAxis(rotateAxis);

	ArcadeDrive(moveValue, rotateValue, squaredInputs);
}

/**
 * Arcade drive implements single stick driving.
 * This function lets you directly provide joystick values from any source.
 * @param moveValue The value to use for fowards/backwards
 * @param rotateValue The value to use for the rotate right/left
 * @param squaredInputs If set, increases the sensitivity at low speeds
 */
void RobotDrive::ArcadeDrive(float moveValue, float rotateValue, bool squaredInputs)
{
	static bool reported = false;
	if (!reported)
	{
		HALReport(HALUsageReporting::kResourceType_RobotDrive, GetNumMotors(), HALUsageReporting::kRobotDrive_ArcadeStandard);
		reported = true;
	}

	// local variables to hold the computed PWM values for the motors
	float leftMotorOutput;
	float rightMotorOutput;

	moveValue = Limit(moveValue);
	rotateValue = Limit(rotateValue);

	if (squaredInputs)
	{
		// square the inputs (while preserving the sign) to increase fine control while permitting full power
		if (moveValue >= 0.0)
		{
			moveValue = (moveValue * moveValue);
		}
		else
		{
			moveValue = -(moveValue * moveValue);
		}
		if (rotateValue >= 0.0)
		{
			rotateValue = (rotateValue * rotateValue);
		}
		else
		{
			rotateValue = -(rotateValue * rotateValue);
		}
	}

	if (moveValue > 0.0)
	{
		if (rotateValue > 0.0)
		{
			leftMotorOutput = moveValue - rotateValue;
			rightMotorOutput = std::max(moveValue, rotateValue);
		}
		else
		{
			leftMotorOutput = std::max(moveValue, -rotateValue);
			rightMotorOutput = moveValue + rotateValue;
		}
	}
	else
	{
		if (rotateValue > 0.0)
		{
			leftMotorOutput = - std::max(-moveValue, rotateValue);
			rightMotorOutput = moveValue + rotateValue;
		}
		else
		{
			leftMotorOutput = moveValue - rotateValue;
			rightMotorOutput = - std::max(-moveValue, -rotateValue);
		}
	}
	SetLeftRightMotorOutputs(leftMotorOutput, rightMotorOutput);
}

/**
 * Drive method for Mecanum wheeled robots.
 *
 * A method for driving with Mecanum wheeled robots. There are 4 wheels
 * on the robot, arranged so that the front and back wheels are toed in 45 degrees.
 * When looking at the wheels from the top, the roller axles should form an X across the robot.
 *
 * This is designed to be directly driven by joystick axes.
 *
 * @param x The speed that the robot should drive in the X direction. [-1.0..1.0]
 * @param y The speed that the robot should drive in the Y direction.
 * This input is inverted to match the forward == -1.0 that joysticks produce. [-1.0..1.0]
 * @param rotation The rate of rotation for the robot that is completely independent of
 * the translation. [-1.0..1.0]
 * @param gyroAngle The current angle reading from the gyro.  Use this to implement field-oriented controls.
 */
void RobotDrive::MecanumDrive_Cartesian(float x, float y, float rotation, float gyroAngle)
{
	static bool reported = false;
	if (!reported)
	{
		HALReport(HALUsageReporting::kResourceType_RobotDrive, GetNumMotors(), HALUsageReporting::kRobotDrive_MecanumCartesian);
		reported = true;
	}

	double xIn = x;
	double yIn = y;
	// Negate y for the joystick.
	yIn = -yIn;
	// Compenstate for gyro angle.
	RotateVector(xIn, yIn, gyroAngle);

	double wheelSpeeds[kMaxNumberOfMotors];
	wheelSpeeds[kFrontLeftMotor] = xIn + yIn + rotation;
	wheelSpeeds[kFrontRightMotor] = -xIn + yIn - rotation;
	wheelSpeeds[kRearLeftMotor] = -xIn + yIn + rotation;
	wheelSpeeds[kRearRightMotor] = xIn + yIn - rotation;

	Normalize(wheelSpeeds);

	m_frontLeftMotor->Set(wheelSpeeds[kFrontLeftMotor] * m_invertedMotors[kFrontLeftMotor] * m_maxOutput, m_syncGroup);
	m_frontRightMotor->Set(wheelSpeeds[kFrontRightMotor] * m_invertedMotors[kFrontRightMotor] * m_maxOutput, m_syncGroup);
	m_rearLeftMotor->Set(wheelSpeeds[kRearLeftMotor] * m_invertedMotors[kRearLeftMotor] * m_maxOutput, m_syncGroup);
	m_rearRightMotor->Set(wheelSpeeds[kRearRightMotor] * m_invertedMotors[kRearRightMotor] * m_maxOutput, m_syncGroup);

	if (m_syncGroup != 0)
	{
		CANJaguar::UpdateSyncGroup(m_syncGroup);
	}

	m_safetyHelper->Feed();
}

/**
 * Drive method for Mecanum wheeled robots.
 *
 * A method for driving with Mecanum wheeled robots. There are 4 wheels
 * on the robot, arranged so that the front and back wheels are toed in 45 degrees.
 * When looking at the wheels from the top, the roller axles should form an X across the robot.
 *
 * @param magnitude The speed that the robot should drive in a given direction. [-1.0..1.0]
 * @param direction The direction the robot should drive in degrees. The direction and maginitute are
 * independent of the rotation rate.
 * @param rotation The rate of rotation for the robot that is completely independent of
 * the magnitute or direction. [-1.0..1.0]
 */
void RobotDrive::MecanumDrive_Polar(float magnitude, float direction, float rotation)
{
	static bool reported = false;
	if (!reported)
	{
		HALReport(HALUsageReporting::kResourceType_RobotDrive, GetNumMotors(), HALUsageReporting::kRobotDrive_MecanumPolar);
		reported = true;
	}

	// Normalized for full power along the Cartesian axes.
	magnitude = Limit(magnitude) * sqrt(2.0);
	// The rollers are at 45 degree angles.
	double dirInRad = (direction + 45.0) * 3.14159 / 180.0;
	double cosD = cos(dirInRad);
	double sinD = sin(dirInRad);

	double wheelSpeeds[kMaxNumberOfMotors];
	wheelSpeeds[kFrontLeftMotor] = sinD * magnitude + rotation;
	wheelSpeeds[kFrontRightMotor] = cosD * magnitude - rotation;
	wheelSpeeds[kRearLeftMotor] = cosD * magnitude + rotation;
	wheelSpeeds[kRearRightMotor] = sinD * magnitude - rotation;

	Normalize(wheelSpeeds);

	m_frontLeftMotor->Set(wheelSpeeds[kFrontLeftMotor] * m_invertedMotors[kFrontLeftMotor] * m_maxOutput, m_syncGroup);
	m_frontRightMotor->Set(wheelSpeeds[kFrontRightMotor] * m_invertedMotors[kFrontRightMotor] * m_maxOutput, m_syncGroup);
	m_rearLeftMotor->Set(wheelSpeeds[kRearLeftMotor] * m_invertedMotors[kRearLeftMotor] * m_maxOutput, m_syncGroup);
	m_rearRightMotor->Set(wheelSpeeds[kRearRightMotor] * m_invertedMotors[kRearRightMotor] * m_maxOutput, m_syncGroup);

	if (m_syncGroup != 0)
	{
		CANJaguar::UpdateSyncGroup(m_syncGroup);
	}

	m_safetyHelper->Feed();
}

/**
 * Holonomic Drive method for Mecanum wheeled robots.
 *
 * This is an alias to MecanumDrive_Polar() for backward compatability
 *
 * @param magnitude The speed that the robot should drive in a given direction.  [-1.0..1.0]
 * @param direction The direction the robot should drive. The direction and maginitute are
 * independent of the rotation rate.
 * @param rotation The rate of rotation for the robot that is completely independent of
 * the magnitute or direction.  [-1.0..1.0]
 */
void RobotDrive::HolonomicDrive(float magnitude, float direction, float rotation)
{
	MecanumDrive_Polar(magnitude, direction, rotation);
}

/** Set the speed of the right and left motors.
 * This is used once an appropriate drive setup function is called such as
 * TwoWheelDrive(). The motors are set to "leftOutput" and "rightOutput"
 * and includes flipping the direction of one side for opposing motors.
 * @param leftOutput The speed to send to the left side of the robot.
 * @param rightOutput The speed to send to the right side of the robot.
 */
void RobotDrive::SetLeftRightMotorOutputs(float leftOutput, float rightOutput)
{
	wpi_assert(m_rearLeftMotor != NULL && m_rearRightMotor != NULL);

	if (m_frontLeftMotor != NULL)
		m_frontLeftMotor->Set(Limit(leftOutput) * m_invertedMotors[kFrontLeftMotor] * m_maxOutput, m_syncGroup);
	m_rearLeftMotor->Set(Limit(leftOutput) * m_invertedMotors[kRearLeftMotor] * m_maxOutput, m_syncGroup);

	if (m_frontRightMotor != NULL)
		m_frontRightMotor->Set(-Limit(rightOutput) * m_invertedMotors[kFrontRightMotor] * m_maxOutput, m_syncGroup);
	m_rearRightMotor->Set(-Limit(rightOutput) * m_invertedMotors[kRearRightMotor] * m_maxOutput, m_syncGroup);

	if (m_syncGroup != 0)
	{
		CANJaguar::UpdateSyncGroup(m_syncGroup);
	}

	m_safetyHelper->Feed();
}

/**
 * Limit motor values to the -1.0 to +1.0 range.
 */
float RobotDrive::Limit(float num)
{
	if (num > 1.0)
	{
		return 1.0;
	}
	if (num < -1.0)
	{
		return -1.0;
	}
	return num;
}

/**
 * Normalize all wheel speeds if the magnitude of any wheel is greater than 1.0.
 */
void RobotDrive::Normalize(double *wheelSpeeds)
{
	double maxMagnitude = fabs(wheelSpeeds[0]);
	int32_t i;
	for (i=1; i<kMaxNumberOfMotors; i++)
	{
		double temp = fabs(wheelSpeeds[i]);
		if (maxMagnitude < temp) maxMagnitude = temp;
	}
	if (maxMagnitude > 1.0)
	{
		for (i=0; i<kMaxNumberOfMotors; i++)
		{
			wheelSpeeds[i] = wheelSpeeds[i] / maxMagnitude;
		}
	}
}

/**
 * Rotate a vector in Cartesian space.
 */
void RobotDrive::RotateVector(double &x, double &y, double angle)
{
	double cosA = cos(angle * (3.14159 / 180.0));
	double sinA = sin(angle * (3.14159 / 180.0));
	double xOut = x * cosA - y * sinA;
	double yOut = x * sinA + y * cosA;
	x = xOut;
	y = yOut;
}

/*
 * Invert a motor direction.
 * This is used when a motor should run in the opposite direction as the drive
 * code would normally run it. Motors that are direct drive would be inverted, the
 * Drive code assumes that the motors are geared with one reversal.
 * @param motor The motor index to invert.
 * @param isInverted True if the motor should be inverted when operated.
 */
void RobotDrive::SetInvertedMotor(MotorType motor, bool isInverted)
{
	if (motor < 0 || motor > 3)
	{
		wpi_setWPIError(InvalidMotorIndex);
		return;
	}
	m_invertedMotors[motor] = isInverted ? -1 : 1;
}

/**
 * Set the turning sensitivity.
 *
 * This only impacts the Drive() entry-point.
 * @param sensitivity Effectively sets the turning sensitivity (or turn radius for a given value)
 */
void RobotDrive::SetSensitivity(float sensitivity)
{
	m_sensitivity = sensitivity;
}

/**
 * Configure the scaling factor for using RobotDrive with motor controllers in a mode other than PercentVbus.
 * @param maxOutput Multiplied with the output percentage computed by the drive functions.
 */
void RobotDrive::SetMaxOutput(double maxOutput)
{
	m_maxOutput = maxOutput;
}

/**
 * Set the number of the sync group for the motor controllers.  If the motor controllers are {@link CANJaguar}s,
 * then they will all be added to this sync group, causing them to update their values at the same time.
 *
 * @param syncGroup the update group to add the motor controllers to
 */
void RobotDrive::SetCANJaguarSyncGroup(uint8_t syncGroup) {
	m_syncGroup = syncGroup;
}

void RobotDrive::SetExpiration(float timeout)
{
	m_safetyHelper->SetExpiration(timeout);
}

float RobotDrive::GetExpiration()
{
	return m_safetyHelper->GetExpiration();
}

bool RobotDrive::IsAlive()
{
	return m_safetyHelper->IsAlive();
}

bool RobotDrive::IsSafetyEnabled()
{
	return m_safetyHelper->IsSafetyEnabled();
}

void RobotDrive::SetSafetyEnabled(bool enabled)
{
	m_safetyHelper->SetSafetyEnabled(enabled);
}

void RobotDrive::GetDescription(char *desc)
{
	sprintf(desc, "RobotDrive");
}

void RobotDrive::StopMotor()
{
	if (m_frontLeftMotor != NULL) m_frontLeftMotor->Disable();
	if (m_frontRightMotor != NULL) m_frontRightMotor->Disable();
	if (m_rearLeftMotor != NULL) m_rearLeftMotor->Disable();
	if (m_rearRightMotor != NULL) m_rearRightMotor->Disable();
	m_safetyHelper->Feed();
}
