#ifndef CloseClaw_H
#define CloseClaw_H

#include <Commands/Command.h>

/**
 * Opens the claw for one second. Real robots should use sensors, stalling
 * motors is BAD!
 */
class CloseClaw: public frc::Command {
public:
	CloseClaw();
	void Initialize() override;
	bool IsFinished() override;
	void End() override;
};

#endif  // CloseClaw_H
