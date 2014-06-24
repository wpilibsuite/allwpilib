#ifndef WaitForBall_H
#define WaitForBall_H

#include "WPILib.h"

/**
 * Wait until the collector senses that it has the ball. This command does
 * nothing and is intended to be used in command groups to wait for this
 * condition.
 */
class WaitForBall: public Command {
public:
	WaitForBall();
	void Initialize();
	void Execute();
	bool IsFinished();
	void End();
	void Interrupted();
};

#endif
