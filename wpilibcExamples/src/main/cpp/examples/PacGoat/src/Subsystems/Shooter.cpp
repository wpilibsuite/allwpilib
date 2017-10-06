#include "Shooter.h"

#include <LiveWindow/LiveWindow.h>

Shooter::Shooter() :
		Subsystem("Shooter") {
	// Put everything to the LiveWindow for testing.
	frc::LiveWindow::GetInstance()->AddSensor("Shooter", "Hot Goal Sensor",
			&hotGoalSensor);
	frc::LiveWindow::GetInstance()->AddSensor("Shooter",
			"Piston1 Reed Switch Front ", &piston1ReedSwitchFront);
	frc::LiveWindow::GetInstance()->AddSensor("Shooter",
			"Piston1 Reed Switch Back ", &piston1ReedSwitchBack);
	frc::LiveWindow::GetInstance()->AddActuator("Shooter", "Latch Piston",
			&latchPiston);
}

void Shooter::InitDefaultCommand() {
	// Set the default command for a subsystem here.
	// SetDefaultCommand(new MySpecialCommand());
}

void Shooter::ExtendBoth() {
	piston1.Set(frc::DoubleSolenoid::kForward);
	piston2.Set(frc::DoubleSolenoid::kForward);
}

void Shooter::RetractBoth() {
	piston1.Set(frc::DoubleSolenoid::kReverse);
	piston2.Set(frc::DoubleSolenoid::kReverse);
}

void Shooter::Extend1() {
	piston1.Set(frc::DoubleSolenoid::kForward);
}

void Shooter::Retract1() {
	piston1.Set(frc::DoubleSolenoid::kReverse);
}

void Shooter::Extend2() {
	piston2.Set(frc::DoubleSolenoid::kReverse);
}

void Shooter::Retract2() {
	piston2.Set(frc::DoubleSolenoid::kForward);
}

void Shooter::Off1() {
	piston1.Set(frc::DoubleSolenoid::kOff);
}

void Shooter::Off2() {
	piston2.Set(frc::DoubleSolenoid::kOff);
}

void Shooter::Unlatch() {
	latchPiston.Set(true);
}

void Shooter::Latch() {
	latchPiston.Set(false);
}

void Shooter::ToggleLatchPosition() {
	latchPiston.Set(!latchPiston.Get());
}

bool Shooter::Piston1IsExtended() {
	return !piston1ReedSwitchFront.Get();
}

bool Shooter::Piston1IsRetracted() {
	return !piston1ReedSwitchBack.Get();
}

void Shooter::OffBoth() {
	piston1.Set(frc::DoubleSolenoid::kOff);
	piston2.Set(frc::DoubleSolenoid::kOff);
}

bool Shooter::GoalIsHot() {
	return hotGoalSensor.Get();
}
