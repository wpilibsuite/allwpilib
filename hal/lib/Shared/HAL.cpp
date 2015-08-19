//This file must compile on ALL PLATFORMS. Be very careful what you put in here.
#include "HAL/HAL.hpp"
#include "NetworkCommunication/FRCComm.h"

int HALGetControlWord(HALControlWord *data)
{
	return FRC_NetworkCommunication_getControlWord((ControlWord_t*) data);
}

void HALSetNewDataSem(NATIVE_MULTIWAIT_ID sem)
{
	setNewDataSem(sem);
}

int HALGetAllianceStation(enum HALAllianceStationID *allianceStation)
{
	return FRC_NetworkCommunication_getAllianceStation((AllianceStationID_t*) allianceStation);
}

int HALGetJoystickAxes(uint8_t joystickNum, HALJoystickAxes *axes)
{
	return FRC_NetworkCommunication_getJoystickAxes(joystickNum, (JoystickAxes_t*) axes, kMaxJoystickAxes);
}

int HALGetJoystickPOVs(uint8_t joystickNum, HALJoystickPOVs *povs)
{
	return FRC_NetworkCommunication_getJoystickPOVs(joystickNum, (JoystickPOV_t*) povs, kMaxJoystickPOVs);
}

int HALGetJoystickButtons(uint8_t joystickNum, HALJoystickButtons *buttons)
{
	return FRC_NetworkCommunication_getJoystickButtons(joystickNum, &buttons->buttons, &buttons->count);
}

int HALGetJoystickDescriptor(uint8_t joystickNum, HALJoystickDescriptor *desc)
{
	return FRC_NetworkCommunication_getJoystickDesc(joystickNum, &desc->isXbox, &desc->type, (char *)(&desc->name),
		&desc->axisCount, (uint8_t *)&desc->axisTypes, &desc->buttonCount, &desc->povCount);
}

int HALGetJoystickIsXbox(uint8_t joystickNum)
{
	HALJoystickDescriptor joystickDesc;
	if(HALGetJoystickDescriptor(joystickNum, &joystickDesc)<0)
	{
		return 0;
	}else
	{
	return joystickDesc.isXbox;
	}
}

int HALGetJoystickType(uint8_t joystickNum)
{
	HALJoystickDescriptor joystickDesc;
	if(HALGetJoystickDescriptor(joystickNum, &joystickDesc)<0)
	{
		return -1;
	} else
	{
	return joystickDesc.type;
	}
}

const char* HALGetJoystickName(uint8_t joystickNum)
{
	HALJoystickDescriptor joystickDesc;
	if(HALGetJoystickDescriptor(joystickNum, &joystickDesc)<0)
	{
		const char* retval = "";
		return retval;
	} else
	{
	const char* retval(joystickDesc.name);
	return retval;
	}
}

int HALGetJoystickAxisType(uint8_t joystickNum, int axis)
{
	HALJoystickDescriptor joystickDesc;
	if(HALGetJoystickDescriptor(joystickNum, &joystickDesc)<0)
	{
		return -1;
	} else
	{
	return joystickDesc.axisTypes[axis];
	}
}

int HALSetJoystickOutputs(uint8_t joystickNum, uint32_t outputs, uint16_t leftRumble, uint16_t rightRumble)
{
	return FRC_NetworkCommunication_setJoystickOutputs(joystickNum, outputs, leftRumble, rightRumble);
}

int HALGetMatchTime(float *matchTime)
{
	return FRC_NetworkCommunication_getMatchTime(matchTime);
}

void HALNetworkCommunicationObserveUserProgramStarting(void)
{
	FRC_NetworkCommunication_observeUserProgramStarting();
}

void HALNetworkCommunicationObserveUserProgramDisabled(void)
{
	FRC_NetworkCommunication_observeUserProgramDisabled();
}

void HALNetworkCommunicationObserveUserProgramAutonomous(void)
{
	FRC_NetworkCommunication_observeUserProgramAutonomous();
}

void HALNetworkCommunicationObserveUserProgramTeleop(void)
{
	FRC_NetworkCommunication_observeUserProgramTeleop();
}

void HALNetworkCommunicationObserveUserProgramTest(void)
{
	FRC_NetworkCommunication_observeUserProgramTest();
}
