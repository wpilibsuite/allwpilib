// JaguarCANDriver.h
//
//  Defines the API for building a CAN Interface Plugin to support
//    PWM-cable-free CAN motor control on FRC robots.  This allows you
//    to connect any CAN interface to the secure Jaguar CAN driver.
//

#ifndef __JaguarCANDriver_h__
#define __JaguarCANDriver_h__

#if defined(__vxworks)
#include <vxWorks.h>
#else
#include <stdint.h>
#include <pthread.h>
#endif
#ifdef USE_THRIFT
#include "NetCommRPCComm.h"
#include <vector>
#endif
namespace nJaguarCANDriver
{
	void sendMessage_wrapper(uint32_t messageID, const uint8_t *data, uint8_t dataSize, int32_t *status);
	void receiveMessage_wrapper(uint32_t *messageID, uint8_t *data, uint8_t *dataSize, uint32_t timeoutMs, int32_t *status);
	int32_t receiveMessageStart_wrapper(uint32_t messageID, uint32_t occurRefNum, uint32_t timeoutMs, int32_t *status);
#if defined (__vxworks)
	int32_t receiveMessageStart_sem_wrapper(uint32_t messageID, uint32_t semaphoreID, uint32_t timeoutMs, int32_t *status);
#else
	int32_t receiveMessageStart_mutex_wrapper(uint32_t messageID, pthread_mutex_t *mutex, uint32_t timeoutMs, int32_t *status);
#endif
	void receiveMessageComplete_wrapper(uint32_t *messageID, uint8_t *data, uint8_t *dataSize, int32_t *status);
#ifdef USE_THRIFT
	void checkEvent_CAN(std::vector< CANEvent >& events);
#endif
}

#ifdef __cplusplus
extern "C"
{
#endif

	void FRC_NetworkCommunication_JaguarCANDriver_sendMessage(uint32_t messageID, const uint8_t *data, uint8_t dataSize, int32_t *status);
	void FRC_NetworkCommunication_JaguarCANDriver_receiveMessage(uint32_t *messageID, uint8_t *data, uint8_t *dataSize, uint32_t timeoutMs, int32_t *status);
	int32_t FRC_NetworkCommunication_JaguarCANDriver_receiveMessageStart(uint32_t messageID, uint32_t occurRefNum, uint32_t timeoutMs, int32_t *status);
#if defined (__vxworks)
	int32_t FRC_NetworkCommunication_JaguarCANDriver_receiveMessageStart_sem(uint32_t messageID, uint32_t semaphoreID, uint32_t timeoutMs, int32_t *status);
#else
	int32_t FRC_NetworkCommunication_JaguarCANDriver_receiveMessageStart_mutex(uint32_t messageID, pthread_mutex_t *mutex, uint32_t timeoutMs, int32_t *status);
#endif
	void FRC_NetworkCommunication_JaguarCANDriver_receiveMessageComplete(uint32_t *messageID, uint8_t *data, uint8_t *dataSize, int32_t *status);

#ifdef __cplusplus
}
#endif

#endif // __JaguarCANDriver_h__
