/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/
#pragma once

#include "HAL/HAL.hpp"
#include "ErrorBase.h"

class DashboardBase : public ErrorBase
{
public:
	virtual void GetStatusBuffer(char** userStatusData, int32_t* userStatusDataSize) = 0;
	virtual void Flush() = 0;
	virtual ~DashboardBase() {}
protected:
	DashboardBase()
	{
	}
private:
	DISALLOW_COPY_AND_ASSIGN(DashboardBase);
};
