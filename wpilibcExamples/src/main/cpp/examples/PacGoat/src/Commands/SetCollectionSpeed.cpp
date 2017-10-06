#include "SetCollectionSpeed.h"

#include "../Robot.h"

SetCollectionSpeed::SetCollectionSpeed(double speed) {
	Requires(Robot::collector.get());
	this->speed = speed;
}

// Called just before this Command runs the first time
void SetCollectionSpeed::Initialize() {
	Robot::collector->SetSpeed(speed);
}
