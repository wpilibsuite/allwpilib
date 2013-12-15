/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2009. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

// JaguarCANDriver.h
//
//  Defines the API for building a CAN Interface Plugin to support
//    PWM-cable-free CAN motor control on FRC robots.  This allows you
//    to connect any CAN interface to the secure Jaguar CAN driver.
//

#ifndef __JaguarCANDriver_h__
#define __JaguarCANDriver_h__

#include "HAL/HAL.h"

#ifdef __cplusplus
extern "C"
{
#endif

	void FRC_NetworkCommunication_JaguarCANDriver_sendMessage(uint32_t messageID, const uint8_t *data, uint8_t dataSize, int32_t *status);
	void FRC_NetworkCommunication_JaguarCANDriver_receiveMessage(uint32_t *messageID, uint8_t *data, uint8_t *dataSize, uint32_t timeoutMs, int32_t *status);

#ifdef __cplusplus
}
#endif

#endif // __JaguarCANDriver_h__
