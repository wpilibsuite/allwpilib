/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <array>

#include <AnalogInput.h>
#include <IterativeRobot.h>
#include <Joystick.h>
#include <PIDController.h>
#include <Spark.h>

/**
 * This is a sample program to demonstrate how to use a soft potentiometer and a
 * PID Controller to reach and maintain position setpoints on an elevator
 * mechanism.
 */
class Robot : public frc::IterativeRobot {
public:
	void RobotInit() override { m_pidController.SetInputRange(0, 5); }

	void TeleopInit() override { m_pidController.Enable(); }

	void TeleopPeriodic() override {
		// when the button is pressed once, the selected elevator
		// setpoint
		// is incremented
		bool currentButtonValue = m_joystick.GetTrigger();
		if (currentButtonValue && !m_previousButtonValue) {
			// index of the elevator setpoint wraps around.
			m_index = (m_index + 1) % (sizeof(kSetPoints) / 8);
		}
		m_previousButtonValue = currentButtonValue;

		m_pidController.SetSetpoint(kSetPoints[m_index]);
	}

private:
	static constexpr int kPotChannel = 1;
	static constexpr int kMotorChannel = 7;
	static constexpr int kJoystickChannel = 0;

	// Bottom, middle, and top elevator setpoints
	static constexpr std::array<double, 3> kSetPoints = {1.0, 2.6, 4.3};

	/* proportional, integral, and derivative speed constants; motor
	 * inverted
	 * DANGER: when tuning PID constants, high/inappropriate values for
	 * pGain,
	 * iGain, and dGain may cause dangerous, uncontrollable, or
	 * undesired behavior!
	 *
	 * These may need to be positive for a non-inverted motor
	 */
	static constexpr double kP = -5.0;
	static constexpr double kI = -0.02;
	static constexpr double kD = -2.0;

	int m_index = 0;
	bool m_previousButtonValue = false;

	frc::AnalogInput m_potentiometer{kPotChannel};
	frc::Joystick m_joystick{kJoystickChannel};
	frc::Spark m_elevatorMotor{kMotorChannel};

	/* potentiometer (AnalogInput) and elevatorMotor (Victor) can be used as
	 * a
	 * PIDSource and PIDOutput respectively. The PIDController takes
	 * pointers
	 * to the PIDSource and PIDOutput, so you must use &potentiometer and
	 * &elevatorMotor to get their pointers.
	 */
	frc::PIDController m_pidController{
			kP, kI, kD, &m_potentiometer, &m_elevatorMotor};
};

constexpr std::array<double, 3> Robot::kSetPoints;

START_ROBOT_CLASS(Robot)
