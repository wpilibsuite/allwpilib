// CANSessionMux.h
//
//  Defines the API for building a CAN Interface Plugin to support
//    PWM-cable-free CAN motor control on FRC robots.  This allows you
//    to connect any CAN interface to the secure Jaguar CAN driver.
//

#ifndef __CANSessionMux_h__
#define __CANSessionMux_h__

#if defined(__vxworks)
#include <vxWorks.h>
#else
#include <stdint.h>
#endif

#define CAN_SEND_PERIOD_NO_REPEAT 0
#define CAN_SEND_PERIOD_STOP_REPEATING -1

/* Flags in the upper bits of the messageID */
#define CAN_IS_FRAME_REMOTE 0x80000000
#define CAN_IS_FRAME_11BIT  0x40000000

#define ERR_CANSessionMux_InvalidBuffer   -44086
#define ERR_CANSessionMux_MessageNotFound -44087
#define WARN_CANSessionMux_NoToken         44087
#define ERR_CANSessionMux_NotAllowed      -44088
#define ERR_CANSessionMux_NotInitialized  -44089

struct tCANStreamMessage{
	uint32_t messageID;
	uint32_t timeStamp;
	uint8_t data[8];
	uint8_t dataSize;
};

namespace nCANSessionMux
{
	void sendMessage_wrapper(uint32_t messageID, const uint8_t *data, uint8_t dataSize, int32_t periodMs, int32_t *status);
	void receiveMessage_wrapper(uint32_t *messageID, uint32_t messageIDMask, uint8_t *data, uint8_t *dataSize, uint32_t *timeStamp, int32_t *status);
	void openStreamSession(uint32_t *sessionHandle, uint32_t messageID, uint32_t messageIDMask, uint32_t maxMessages, int32_t *status);
	void closeStreamSession(uint32_t sessionHandle);
	void readStreamSession(uint32_t sessionHandle, struct tCANStreamMessage *messages, uint32_t messagesToRead, uint32_t *messagesRead, int32_t *status);
	void getCANStatus(float *percentBusUtilization, uint32_t *busOffCount, uint32_t *txFullCount, uint32_t *receiveErrorCount, uint32_t *transmitErrorCount, int32_t *status);
}

#ifdef __cplusplus
extern "C"
{
#endif

	void FRC_NetworkCommunication_CANSessionMux_sendMessage(uint32_t messageID, const uint8_t *data, uint8_t dataSize, int32_t periodMs, int32_t *status);
	void FRC_NetworkCommunication_CANSessionMux_receiveMessage(uint32_t *messageID, uint32_t messageIDMask, uint8_t *data, uint8_t *dataSize, uint32_t *timeStamp, int32_t *status);
	void FRC_NetworkCommunication_CANSessionMux_openStreamSession(uint32_t *sessionHandle, uint32_t messageID, uint32_t messageIDMask, uint32_t maxMessages, int32_t *status);
	void FRC_NetworkCommunication_CANSessionMux_closeStreamSession(uint32_t sessionHandle);
	void FRC_NetworkCommunication_CANSessionMux_readStreamSession(uint32_t sessionHandle, struct tCANStreamMessage *messages, uint32_t messagesToRead, uint32_t *messagesRead, int32_t *status);
	void FRC_NetworkCommunication_CANSessionMux_getCANStatus(float *percentBusUtilization, uint32_t *busOffCount, uint32_t *txFullCount, uint32_t *receiveErrorCount, uint32_t *transmitErrorCount, int32_t *status);

#ifdef __cplusplus
}
#endif

#endif // __CANSessionMux_h__
