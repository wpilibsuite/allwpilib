#include "Shooter.h"

Shooter::Shooter() :
		Subsystem("Shooter"),
		// Configure Devices
		piston1(new DoubleSolenoid(3, 4)),
		piston2(new DoubleSolenoid(5, 6)),
		latchPiston(new Solenoid(1, 2)),
		piston1ReedSwitchFront(new DigitalInput(9)),
		piston1ReedSwitchBack(new DigitalInput(11)),
		hotGoalSensor(new DigitalInput(7))
{

	// Put everything to the LiveWindow for testing.
	LiveWindow::GetInstance().AddSensor("Shooter", "Hot Goal Sensor", hotGoalSensor);
	LiveWindow::GetInstance().AddSensor("Shooter", "Piston1 Reed Switch Front ", piston1ReedSwitchFront);
	LiveWindow::GetInstance().AddSensor("Shooter", "Piston1 Reed Switch Back ", piston1ReedSwitchBack);
	LiveWindow::GetInstance().AddActuator("Shooter", "Latch Piston", latchPiston);
}

void Shooter::InitDefaultCommand()
{
	// Set the default command for a subsystem here.
	//SetDefaultCommand(new MySpecialCommand());
}

void Shooter::ExtendBoth() {
	piston1->Set(DoubleSolenoid::kForward);
	piston2->Set(DoubleSolenoid::kForward);
}

void Shooter::RetractBoth() {
	piston1->Set(DoubleSolenoid::kReverse);
	piston2->Set(DoubleSolenoid::kReverse);
}

void Shooter::Extend1() {
	piston1->Set(DoubleSolenoid::kForward);
}

void Shooter::Retract1() {
	piston1->Set(DoubleSolenoid::kReverse);
}

void Shooter::Extend2() {
	piston2->Set(DoubleSolenoid::kReverse);
}

void Shooter::Retract2() {
	piston2->Set(DoubleSolenoid::kForward);
}

void Shooter::Off1() {
	piston1->Set(DoubleSolenoid::kOff);
}

void Shooter::Off2() {
	piston2->Set(DoubleSolenoid::kOff);
}

void Shooter::Unlatch() {
	latchPiston->Set(true);
}

void Shooter::Latch() {
	latchPiston->Set(false);
}

void Shooter::ToggleLatchPosition() {
	latchPiston->Set(!latchPiston->Get());
}

bool Shooter::Piston1IsExtended() {
	return !piston1ReedSwitchFront->Get();
}

bool Shooter::Piston1IsRetracted() {
	return !piston1ReedSwitchBack->Get();
}

void Shooter::OffBoth() {
	piston1->Set(DoubleSolenoid::kOff);
	piston2->Set(DoubleSolenoid::kOff);
}

bool Shooter::GoalIsHot() {
	return hotGoalSensor->Get();
}
