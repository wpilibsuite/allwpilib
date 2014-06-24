#ifndef OpenClaw_H
#define OpenClaw_H

#include "WPILib.h"

/**
 * Opens the claw
 */
class OpenClaw: public Command {
public:
	OpenClaw();
	void Initialize();
	void Execute();
	bool IsFinished();
	void End();
	void Interrupted();
};

#endif
