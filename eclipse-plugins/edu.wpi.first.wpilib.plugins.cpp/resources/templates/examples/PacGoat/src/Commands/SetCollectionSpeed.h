#ifndef SetCollectionSpeed_H
#define SetCollectionSpeed_H

#include "WPILib.h"

/**
 * This command sets the collector rollers spinning at the given speed. Since
 * there is no sensor for detecting speed, it finishes immediately. As a result,
 * the spinners may still be adjusting their speed.
 */
class SetCollectionSpeed: public Command {
private:
	double speed;
public:
	SetCollectionSpeed(double speed);
	void Initialize();
	void Execute();
	bool IsFinished();
	void End();
	void Interrupted();
};

#endif
