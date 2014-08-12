#ifndef $classname_H
#define $classname_H

#include "../CommandBase.h"
#include "WPILib.h"

class $classname: public CommandBase
{
public:
	$classname();
	void Initialize();
	void Execute();
	bool IsFinished();
	void End();
	void Interrupted();
};

#endif
