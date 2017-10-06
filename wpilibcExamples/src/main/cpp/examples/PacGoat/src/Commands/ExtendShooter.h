#ifndef ExtendShooter_H
#define ExtendShooter_H

#include <Commands/TimedCommand.h>

/**
 * Extend the shooter and then retract it after a second.
 */
class ExtendShooter : public frc::TimedCommand {
public:
	ExtendShooter();
	void Initialize() override;
	void End() override;
};

#endif  // ExtendShooter_H
