/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#ifndef __ENUM_CAMERA_PARAMETER_H__
#define __ENUM_CAMERA_PARAMETER_H__

#include "IntCameraParameter.h"

/**
 * Enumerated camera parameter.
 * This class represents a camera parameter that takes an enumerated type for a value.
 */
class EnumCameraParameter: public IntCameraParameter
{
private:
	const char *const*m_enumValues;
	int m_numChoices;

public:
	EnumCameraParameter(const char *setString, const char *getString, bool requiresRestart, const char *const*choices, int numChoices);
	virtual bool CheckChanged(bool &changed, char *param);
	virtual void GetParamFromString(const char *string, int stringLength);
};

#endif
