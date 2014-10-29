#pragma once

#ifdef __vxworks
#include <vxWorks.h>
#else
#include <stdint.h>
#endif

extern "C"
{
	void serialInitializePort(uint8_t port, int32_t *status);
	void serialSetBaudRate(uint8_t port, uint32_t baud, int32_t *status);
	void serialSetDataBits(uint8_t port, uint8_t bits, int32_t *status);
	void serialSetParity(uint8_t port, uint8_t parity, int32_t *status);
	void serialSetStopBits(uint8_t port, uint8_t stopBits, int32_t *status);
	void serialSetWriteMode(uint8_t port, uint8_t mode, int32_t *status);
	void serialSetFlowControl(uint8_t port, uint8_t flow, int32_t *status);
	void serialSetTimeout(uint8_t port, float timeout, int32_t *status);
	void serialEnableTermination(uint8_t port, char terminator, int32_t *status);
	void serialDisableTermination(uint8_t port, int32_t *status);
	void serialSetReadBufferSize(uint8_t port, uint32_t size, int32_t *status);
	void serialSetWriteBufferSize(uint8_t port, uint32_t size, int32_t *status);
	int32_t serialGetBytesReceived(uint8_t port, int32_t *status);
	uint32_t serialRead(uint8_t port, char* buffer, int32_t count, int32_t *status);
	uint32_t serialWrite(uint8_t port, const char *buffer, int32_t count, int32_t *status);
	void serialFlush(uint8_t port, int32_t *status);
	void serialClear(uint8_t port, int32_t *status);
	void serialClose(uint8_t port, int32_t *status);
}