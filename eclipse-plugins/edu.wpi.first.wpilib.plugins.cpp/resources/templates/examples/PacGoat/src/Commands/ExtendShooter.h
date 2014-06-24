#ifndef ExtendShooter_H
#define ExtendShooter_H

#include "WPILib.h"

/**
 * Extend the shooter and then retract it after a second.
 */
class ExtendShooter: public Command {
public:
	ExtendShooter();
	void Initialize();
	void Execute();
	bool IsFinished();
	void End();
	void Interrupted();
};

#endif
