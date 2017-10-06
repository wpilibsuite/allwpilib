#ifndef CloseClaw_H
#define CloseClaw_H

#include <Commands/InstantCommand.h>

/**
 * Close the claw.
 *
 * NOTE: It doesn't wait for the claw to close since there is no sensor to
 * detect that.
 */
class CloseClaw: public frc::InstantCommand {
public:
	CloseClaw();
	void Initialize() override;
};

#endif  // CloseClaw_H
