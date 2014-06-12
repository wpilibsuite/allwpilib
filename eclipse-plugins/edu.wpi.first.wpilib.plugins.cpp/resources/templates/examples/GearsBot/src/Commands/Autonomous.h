#ifndef Autonomous_H
#define Autonomous_H

#include "Commands/CommandGroup.h"

/**
 * The main autonomous command to pickup and deliver the
 * soda to the box.
 */
class Autonomous: public CommandGroup {
public:
	Autonomous();
};

#endif
