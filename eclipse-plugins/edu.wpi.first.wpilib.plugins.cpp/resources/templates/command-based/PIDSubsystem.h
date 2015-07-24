#ifndef $classname_H
#define $classname_H

#include "Commands/PIDSubsystem.h"
#include "WPILib.h"

class $classname: public PIDSubsystem
{
public:
	$classname();
	double ReturnPIDInput() const;
	void UsePIDOutput(double output);
	void InitDefaultCommand();
};

#endif
