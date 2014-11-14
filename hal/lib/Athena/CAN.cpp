#include "HAL/CAN.hpp"
#include <map>

struct CANMessage
{
	uint8_t data[8];
};

static std::map<uint32_t, CANMessage> outgoingMessages;
static std::map<uint32_t, CANMessage> incomingMessages;

static const uint32_t kFullMessageIDMask = 0x1fffffff;

/**
 * Gets the data from the outgoing hashmap and calls
 * CANSessionMux...sendMessage.
 */
void canTxSend(uint32_t arbID, uint8_t length, int32_t period)
{
	CANMessage &message = outgoingMessages[arbID];

	int32_t status;

	FRC_NetworkCommunication_CANSessionMux_sendMessage(
		arbID, message.data, length, period, &status);
}

/**
 * Updates a field in the outgoing hashmap.
 *
 * This is called every time an single byte field changes in a message.data,
 * such as when a setter on a CAN device is called.
 */
void canTxPackInt8(uint32_t arbID, uint8_t offset, uint8_t value)
{
	CANMessage &message = outgoingMessages[arbID];

	message.data[offset] = value;
}

/**
 * Updates a field in the outgoing hashmap.
 *
 * This is called every time a short integer field changes in a message.data,
 * such as when a setter on a CAN device is called.
 */
void canTxPackInt16(uint32_t arbID, uint8_t offset, uint16_t value)
{
	CANMessage &message = outgoingMessages[arbID];

	*(uint16_t *)(message.data + offset) = value;
}

/**
 * Updates a field in the outgoing hashmap.
 *
 * This is called every time a long integer field changes in a message.data,
 * such as when a setter on a CAN device is called.
 */
void canTxPackInt32(uint32_t arbID, uint8_t offset, uint32_t value)
{
	CANMessage &message = outgoingMessages[arbID];

	*(uint32_t *)(message.data + offset) = value;
}

/**
 * Updates a field in the outgoing hashmap.
 *
 * This is called every time an 8.8 fixed point field changes in a message,
 * such as when a setter on a CAN device is called.
 */
void canTxPackFXP16(uint32_t arbID, uint8_t offset, double value)
{
	int16_t raw = value * 255.0;

	canTxPackInt16(arbID, offset, raw);
}

/**
 * Updates a field in the outgoing hashmap.
 *
 * This is called every time a 16.16 fixed point field changes in a message,
 * such as when a setter on a CAN device is called.
 */
void canTxPackFXP32(uint32_t arbID, uint8_t offset, double value)
{
	int32_t raw = value * 65535.0;

	canTxPackInt32(arbID, offset, raw);
}

/**
 * Unpack a field from the outgoing hashmap.
 *
 * This is called in getters for configuration data.
 */
uint8_t canTxUnpackInt8(uint32_t arbID, uint8_t offset)
{
	CANMessage &message = outgoingMessages[arbID];

	return message.data[offset];
}

/**
 * Unpack a field from the outgoing hashmap.
 *
 * This is called in getters for configuration data.
 */
uint16_t canTxUnpackInt16(uint32_t arbID, uint8_t offset)
{
	CANMessage &message = outgoingMessages[arbID];

	return *reinterpret_cast<uint16_t *>(message.data + offset);
}

/**
 * Unpack a field from the outgoing hashmap.
 *
 * This is called in getters for configuration data.
 */
uint32_t canTxUnpackInt32(uint32_t arbID, uint8_t offset)
{
	CANMessage &message = outgoingMessages[arbID];

	return *reinterpret_cast<uint32_t *>(message.data + offset);
}

/**
 * Unpack a field from the outgoing hashmap.
 *
 * This is called in getters for configuration data.
 */
double canTxUnpackFXP16(uint32_t arbID, uint8_t offset)
{
	int16_t raw = canTxUnpackInt16(arbID, offset);

	return raw / 255.0;
}

/**
 * Unpack a field from the outgoing hashmap.
 *
 * This is called in getters for configuration data.
 */
double canTxUnpackFXP32(uint32_t arbID, uint8_t offset)
{
	int32_t raw = canTxUnpackInt32(arbID, offset);

	return raw / 65535.0;
}

/**
 * Get data from CANSessionMux (if it's available) and put it in the incoming
 * hashmap.
 *
 * @return true if there's new data.  Otherwise, the last received value should
 * still be in the hashmap.
 */
bool canRxReceive(uint32_t arbID)
{
	CANMessage &message = incomingMessages[arbID];

	uint8_t length;
	uint32_t timestamp;
	int32_t status;

	FRC_NetworkCommunication_CANSessionMux_receiveMessage(
		&arbID, kFullMessageIDMask, message.data, &length, &timestamp, &status);

	return status != ERR_CANSessionMux_MessageNotFound;
}

/**
 * Unpack a field from the incoming hashmap.
 *
 * This is called in getters for status data.
 */
uint8_t canRxUnpackInt8(uint32_t arbID, uint8_t offset)
{
	CANMessage &message = incomingMessages[arbID];

	return message.data[offset];
}

/**
 * Unpack a field from the incoming hashmap.
 *
 * This is called in getters for status data.
 */
uint16_t canRxUnpackInt16(uint32_t arbID, uint8_t offset)
{
	CANMessage &message = incomingMessages[arbID];

	return *reinterpret_cast<uint16_t *>(message.data + offset);
}

/**
 * Unpack a field from the incoming hashmap.
 *
 * This is called in getters for status data.
 */
uint32_t canRxUnpackInt32(uint32_t arbID, uint8_t offset)
{
	CANMessage &message = incomingMessages[arbID];

	return *reinterpret_cast<uint32_t *>(message.data + offset);
}

/**
 * Unpack a field from the incoming hashmap.
 *
 * This is called in getters for status data.
 */
double canRxUnpackFXP16(uint32_t arbID, uint8_t offset)
{
	int16_t raw = canRxUnpackInt16(arbID, offset);

	return raw / 255.0;
}

/**
 * Unpack a field from the incoming hashmap.
 *
 * This is called in getters for status data.
 */
double canRxUnpackFXP32(uint32_t arbID, uint8_t offset)
{
	int32_t raw = canRxUnpackInt32(arbID, offset);

	return raw / 65535.0;
}
