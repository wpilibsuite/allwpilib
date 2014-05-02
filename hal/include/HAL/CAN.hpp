#pragma once

#ifdef __vxworks
#include <vxWorks.h>
#else
#include <stdint.h>
#endif

extern "C"
{
	void JaguarCANSendMessage(uint32_t messageID, const uint8_t *data, uint8_t dataSize,
			int32_t *status);
	void JaguarCANReceiveMessage(uint32_t *messageID, uint8_t *data, uint8_t *dataSize,
			uint32_t timeoutMs, int32_t *status);
}
