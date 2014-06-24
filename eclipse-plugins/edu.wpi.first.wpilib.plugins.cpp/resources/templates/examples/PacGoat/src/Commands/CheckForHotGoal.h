#ifndef CheckForHotGoal_H
#define CheckForHotGoal_H

#include "WPILib.h"

/**
 * This command looks for the hot goal and waits until it's detected or timed
 * out. The timeout is because it's better to shoot and get some autonomous
 * points than get none. When called sequentially, this command will block until
 * the hot goal is detected or until it is timed out.
 */
class CheckForHotGoal : Command {
public:
	CheckForHotGoal(double time);
	void Initialize();
	void Execute();
	bool IsFinished();
	void End();
	void Interrupted();
};

#endif
