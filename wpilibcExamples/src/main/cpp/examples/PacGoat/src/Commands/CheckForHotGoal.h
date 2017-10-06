#ifndef CheckForHotGoal_H
#define CheckForHotGoal_H

#include <Commands/Command.h>

/**
 * This command looks for the hot goal and waits until it's detected or timed
 * out. The timeout is because it's better to shoot and get some autonomous
 * points than get none. When called sequentially, this command will block until
 * the hot goal is detected or until it is timed out.
 */
class CheckForHotGoal: public frc::Command {
public:
	CheckForHotGoal(double time);
	bool IsFinished() override;
};

#endif  // CheckForHotGoal_H
