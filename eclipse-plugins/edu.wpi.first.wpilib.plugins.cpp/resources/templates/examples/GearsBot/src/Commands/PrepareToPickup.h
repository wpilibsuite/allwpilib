#ifndef PrepareToPickup_H
#define PrepareToPickup_H

#include "Commands/CommandGroup.h"

/**
 * Make sure the robot is in a state to pickup soda cans.
 */
class PrepareToPickup: public CommandGroup {
public:
	PrepareToPickup();
};

#endif
