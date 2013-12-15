/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#ifndef __INT_CAMERA_PARAMETER_H__
#define __INT_CAMERA_PARAMETER_H__

#include "HAL/HAL.h"

/**
 * Integer camera parameter.
 * This class represents a camera parameter that takes an integer value.
 */
class IntCameraParameter
{
protected:
	const char *m_setString;
	const char *m_getString;
	bool m_changed;
	bool m_requiresRestart;
	int m_value;			// parameter value

	int SearchForParam(const char *pattern, const char *searchString, int searchStringLen, char *result);

public:
	IntCameraParameter(const char *setString, const char *getString, bool requiresRestart);
	virtual ~IntCameraParameter(){}
	int GetValue();
	void SetValue(int value);
	virtual bool CheckChanged(bool &changed, char *param);
	virtual void GetParamFromString(const char *string, int stringLength);
};

#endif
