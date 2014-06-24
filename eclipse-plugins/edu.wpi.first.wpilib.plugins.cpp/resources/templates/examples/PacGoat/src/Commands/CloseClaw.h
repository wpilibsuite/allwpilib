#ifndef CloseClaw_H
#define CloseClaw_H

#include "WPILib.h"

/**
 * Close the claw.
 *
 * NOTE: It doesn't wait for the claw to close since there is no sensor to
 * detect that.
 */
class CloseClaw: public Command {
public:
	CloseClaw();
	void Initialize();
	void Execute();
	bool IsFinished();
	void End();
	void Interrupted();
};

#endif
