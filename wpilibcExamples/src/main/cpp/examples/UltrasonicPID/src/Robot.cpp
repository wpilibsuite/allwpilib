/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <AnalogInput.h>
#include <Drive/DifferentialDrive.h>
#include <IterativeRobot.h>
#include <PIDController.h>
#include <PIDOutput.h>
#include <Spark.h>

/**
 * This is a sample program demonstrating how to use an ultrasonic sensor and
 * proportional control to maintain a set distance from an object.
 */
class Robot : public frc::IterativeRobot {
public:
	/**
	 * Drives the robot a set distance from an object using PID control and
	 * the
	 * ultrasonic sensor.
	 */
	void TeleopInit() override {
		// Set expected range to 0-24 inches; e.g. at 24 inches from
		// object go
		// full forward, at 0 inches from object go full backward.
		m_pidController.SetInputRange(0, 24 * kValueToInches);

		// Set setpoint of the pidController
		m_pidController.SetSetpoint(kHoldDistance * kValueToInches);

		// Begin PID control
		m_pidController.Enable();
	}

private:
	// Internal class to write to robot drive using a PIDOutput
	class MyPIDOutput : public frc::PIDOutput {
	public:
		explicit MyPIDOutput(frc::DifferentialDrive& r)
		    : m_rd(r) {
			m_rd.SetSafetyEnabled(false);
		}

		void PIDWrite(double output) override {
			// Write to robot drive by reference
			m_rd.ArcadeDrive(output, 0);
		}

	private:
		frc::DifferentialDrive& m_rd;
	};

	// Distance in inches the robot wants to stay from an object
	static constexpr int kHoldDistance = 12;

	// Factor to convert sensor values to a distance in inches
	static constexpr double kValueToInches = 0.125;

	// proportional speed constant
	static constexpr double kP = 7.0;

	// integral speed constant
	static constexpr double kI = 0.018;

	// derivative speed constant
	static constexpr double kD = 1.5;

	static constexpr int kLeftMotorPort = 0;
	static constexpr int kRightMotorPort = 1;
	static constexpr int kUltrasonicPort = 0;

	frc::AnalogInput m_ultrasonic{kUltrasonicPort};

	frc::Spark m_left{kLeftMotorPort};
	frc::Spark m_right{kRightMotorPort};
	frc::DifferentialDrive m_robotDrive{m_left, m_right};

	frc::PIDController m_pidController{kP, kI, kD, &m_ultrasonic,
			new MyPIDOutput(m_robotDrive)};
};

START_ROBOT_CLASS(Robot)
