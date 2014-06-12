#ifndef Pickup_H
#define Pickup_H

#include "Commands/CommandGroup.h"

/**
 * Pickup a soda can (if one is between the open claws) and
 * get it in a safe state to drive around.
 */
class Pickup: public CommandGroup {
public:
	Pickup();
};

#endif
