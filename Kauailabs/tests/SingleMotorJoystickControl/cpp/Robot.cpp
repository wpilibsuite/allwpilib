/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <IterativeRobot.h>
#include <Joystick.h>
#include <Talon.h>
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
		    wpi::outs() << "Joystick:  " << joystickVal << "\n";
		m_motor.Set(m_stick.GetX());
		Wait(0.005);
	}

	void TestPeriodic() override {}

private:
	Talon m_motor{0};

	Joystick m_stick{0};
};

START_ROBOT_CLASS(Robot)
