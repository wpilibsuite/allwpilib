// CANInterfacePlugin.h
//
//  Defines the API for building a CAN Interface Plugin to support
//    PWM-cable-free CAN motor control on FRC robots.  This allows you
//    to connect any CAN interface to the secure Jaguar CAN driver.
//

#ifndef __CANInterfacePlugin_h__
#define __CANInterfacePlugin_h__

#include <stdint.h>

#define CAN_IS_FRAME_REMOTE 0x80000000
#define CAN_IS_FRAME_11BIT  0x40000000
#define CAN_29BIT_MESSAGE_ID_MASK 0x1FFFFFFF
#define CAN_11BIT_MESSAGE_ID_MASK 0x000007FF

class CANInterfacePlugin
{
public:
	CANInterfacePlugin() {}
	virtual ~CANInterfacePlugin() {}

	/**
	 * This entry-point of the CANInterfacePlugin is passed a message that the driver needs to send to
	 * a device on the CAN bus.
	 * 
	 * This function may be called from multiple contexts and must therefore be reentrant.
	 * 
	 * @param messageID The 29-bit CAN message ID in the lsbs.  The msb can indicate a remote frame.
	 * @param data A pointer to a buffer containing between 0 and 8 bytes to send with the message.  May be NULL if dataSize is 0.
	 * @param dataSize The number of bytes to send with the message.
	 * @return Return any error code.  On success return 0.
	 */
	virtual int32_t sendMessage(uint32_t messageID, const uint8_t *data, uint8_t dataSize) = 0;

	/**
	 * This entry-point of the CANInterfacePlugin is passed buffers which should be populated with
	 * any received messages from devices on the CAN bus.
	 * 
	 * This function is always called by a single task in the Jaguar driver, so it need not be reentrant.
	 * 
	 * This function is expected to block for some period of time waiting for a message from the CAN bus.
	 * It may timeout periodically (returning non-zero to indicate no message was populated) to allow for
	 * shutdown and unloading of the plugin.
	 * 
	 * @param messageID A reference to be populated with a received 29-bit CAN message ID in the lsbs.
	 * @param data A pointer to a buffer of 8 bytes to be populated with data received with the message.
	 * @param dataSize A reference to be populated with the size of the data received (0 - 8 bytes).
	 * @return This should return 0 if a message was populated, non-0 if no message was not populated.
	 */
	virtual int32_t receiveMessage(uint32_t &messageID, uint8_t *data, uint8_t &dataSize) = 0;

#if defined(__linux)
	/**
	 * This entry-point of the CANInterfacePlugin returns status of the CAN bus.
	 * 
	 * This function may be called from multiple contexts and must therefore be reentrant.
	 * 
	 * This function will return detailed hardware status if available for diagnostics of the CAN interface.
	 * 
	 * @param busOffCount The number of times that sendMessage failed with a busOff error indicating that messages
	 *  are not successfully transmitted on the bus.
	 * @param txFullCount The number of times that sendMessage failed with a txFifoFull error indicating that messages
	 *  are not successfully received by any CAN device.
	 * @param receiveErrorCount The count of receive errors as reported by the CAN driver.
	 * @param transmitErrorCount The count of transmit errors as reported by the CAN driver.
	 * @return This should return 0 if all status was retrieved successfully or an error code if not.
	 */
	virtual int32_t getStatus(uint32_t &busOffCount, uint32_t &txFullCount, uint32_t &receiveErrorCount, uint32_t &transmitErrorCount) {return 0;}
#endif
};

/**
 * This function allows you to register a CANInterfacePlugin to provide access a CAN bus.
 * 
 * @param interface A pointer to an object that inherits from CANInterfacePlugin and implements
 * the pure virtual interface.  If NULL, unregister the current plugin.
 */
void FRC_NetworkCommunication_CANSessionMux_registerInterface(CANInterfacePlugin* interface);

#endif // __CANInterfacePlugin_h__
