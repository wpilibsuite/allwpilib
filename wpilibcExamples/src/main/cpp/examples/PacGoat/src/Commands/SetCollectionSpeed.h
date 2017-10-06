#ifndef SetCollectionSpeed_H
#define SetCollectionSpeed_H

#include <Commands/InstantCommand.h>

/**
 * This command sets the collector rollers spinning at the given speed. Since
 * there is no sensor for detecting speed, it finishes immediately. As a result,
 * the spinners may still be adjusting their speed.
 */
class SetCollectionSpeed: public frc::InstantCommand {
public:
	SetCollectionSpeed(double speed);
	void Initialize() override;

private:
	double speed;
};

#endif  // SetCollectionSpeed_H
