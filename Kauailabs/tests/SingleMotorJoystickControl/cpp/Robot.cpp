/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <IterativeRobot.h>
#include <Joystick.h>
#include <Talon.h>
#include <Victor.h>
#include <Jaguar.h>
#include <Servo.h>
#include <Timer.h>

using namespace frc;

class Robot : public IterativeRobot {
public:
	Robot() {
	}

	void AutonomousInit() override {
	}

	void AutonomousPeriodic() override {
		Wait(0.02);
	}

	void TeleopInit() override {}

	void TeleopPeriodic() override {
		char joystickVal[10];
		sprintf(joystickVal, "%0.2f", m_stick.GetX());
		char servoVal[10];
		sprintf(servoVal, "%0.2f", (m_stick.GetX()+1.0)/2.0);
		wpi::outs() << "Joystick:  " << joystickVal << ", Motors:  " << joystickVal << ", Servo:  " << servoVal << "\n";
		m_motor1.Set(m_stick.GetX());
		m_motor2.Set(m_stick.GetX());
		m_motor3.Set(m_stick.GetX());
		m_servo.Set((m_stick.GetX()+1.0)/2.0);
		Wait(0.005);
	}

	void TestPeriodic() override {}

private:
	Talon m_motor1{0};
	Victor m_motor2{1};
	Jaguar m_motor3{2};
	Servo m_servo{21};

	Joystick m_stick{0};
};

START_ROBOT_CLASS(Robot)
