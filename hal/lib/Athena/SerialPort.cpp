#include "HAL/SerialPort.hpp"
#include "visa/visa.h"


uint32_t m_resourceManagerHandle;
uint32_t m_portHandle[2];

void serialInitializePort(uint8_t port, int32_t *status) {
	char const * portName;

	if (m_resourceManagerHandle ==0)
		viOpenDefaultRM((ViSession*)&m_resourceManagerHandle);
	
	if(port == 0)
		portName = "ASRL1::INSTR";
	else if (port == 1)
		portName = "ASRL2::INSTR";
	else
		portName = "ASRL3::INSTR";

	*status = viOpen(m_resourceManagerHandle, const_cast<char*>(portName), VI_NULL, VI_NULL, (ViSession*)&m_portHandle[port]);
	if(*status > 0)
		*status = 0;
}

void serialSetBaudRate(uint8_t port, uint32_t baud, int32_t *status) {
	*status = viSetAttribute(m_portHandle[port], VI_ATTR_ASRL_BAUD, baud);
	if(*status > 0)
		*status = 0;
}
	
void serialSetDataBits(uint8_t port, uint8_t bits, int32_t *status) {
	*status = viSetAttribute(m_portHandle[port], VI_ATTR_ASRL_DATA_BITS, bits);
	if(*status > 0)
		*status = 0;
}

void serialSetParity(uint8_t port, uint8_t parity, int32_t *status) {
	*status = viSetAttribute(m_portHandle[port], VI_ATTR_ASRL_PARITY, parity);
	if(*status > 0)
		*status = 0;
}
	
void serialSetStopBits(uint8_t port, uint8_t stopBits, int32_t *status) {
	*status = viSetAttribute(m_portHandle[port], VI_ATTR_ASRL_STOP_BITS, stopBits);
	if(*status > 0)
		*status = 0;
}

void serialSetWriteMode(uint8_t port, uint8_t mode, int32_t *status) {
	*status = viSetAttribute(m_portHandle[port], VI_ATTR_WR_BUF_OPER_MODE, mode);
	if(*status > 0)
		*status = 0;
}	

void serialSetFlowControl(uint8_t port, uint8_t flow, int32_t *status) {
	*status = viSetAttribute(m_portHandle[port], VI_ATTR_ASRL_FLOW_CNTRL, flow);
	if(*status > 0)
		*status = 0;
}

void serialSetTimeout(uint8_t port, float timeout, int32_t *status) {
	*status = viSetAttribute(m_portHandle[port], VI_ATTR_TMO_VALUE, (uint32_t)(timeout * 1e3));
	if(*status > 0)
		*status = 0;
}
	
void serialEnableTermination(uint8_t port, char terminator, int32_t *status) {
	viSetAttribute(m_portHandle[port], VI_ATTR_TERMCHAR_EN, VI_TRUE);
	viSetAttribute(m_portHandle[port], VI_ATTR_TERMCHAR, terminator);
	*status = viSetAttribute(m_portHandle[port], VI_ATTR_ASRL_END_IN, VI_ASRL_END_TERMCHAR);
	if(*status > 0)
		*status = 0;
}

void serialDisableTermination(uint8_t port, int32_t *status) {
	viSetAttribute(m_portHandle[port], VI_ATTR_TERMCHAR_EN, VI_FALSE);
	*status = viSetAttribute(m_portHandle[port], VI_ATTR_ASRL_END_IN, VI_ASRL_END_NONE);
	if(*status > 0)
		*status = 0;
}

void serialSetReadBufferSize(uint8_t port, uint32_t size, int32_t *status) {
	*status = viSetBuf(m_portHandle[port], VI_READ_BUF, size);
	if(*status > 0)
		*status = 0;
}
	
void serialSetWriteBufferSize(uint8_t port, uint32_t size, int32_t *status) {
	*status = viSetBuf(m_portHandle[port], VI_WRITE_BUF, size);
	if(*status > 0)
		*status = 0;
}

int32_t serialGetBytesReceived(uint8_t port, int32_t *status) {
	int32_t bytes = 0;

	*status = viGetAttribute(m_portHandle[port], VI_ATTR_ASRL_AVAIL_NUM, &bytes);
	if(*status > 0)
		*status = 0;
	return bytes;
}
	
uint32_t serialRead(uint8_t port, char* buffer, int32_t count, int32_t *status) {
	uint32_t retCount = 0;
	
	*status = viRead(m_portHandle[port], (ViPBuf)buffer, count, (ViPUInt32)&retCount);
	
	if(*status == VI_ERROR_IO || *status == VI_ERROR_ASRL_OVERRUN || *status == VI_ERROR_ASRL_FRAMING || *status == VI_ERROR_ASRL_PARITY)
	{
		int32_t localStatus = 0;
		serialClear(port, &localStatus);
	}
	
	if(*status == VI_ERROR_TMO || *status > 0)
		*status = 0;
	return retCount;
}
	
uint32_t serialWrite(uint8_t port, const char *buffer, int32_t count, int32_t *status) {
	uint32_t retCount = 0;

	*status = viWrite(m_portHandle[port], (ViPBuf)buffer, count, (ViPUInt32)&retCount);
	
	if(*status > 0)
		*status = 0;
	return retCount;
}

void serialFlush(uint8_t port, int32_t *status) {
	*status = viFlush(m_portHandle[port], VI_WRITE_BUF);
	if(*status > 0)
		*status = 0;
}

void serialClear(uint8_t port, int32_t *status) {
	*status = viClear(m_portHandle[port]);
	if(*status > 0)
		*status = 0;
}

void serialClose(uint8_t port, int32_t *status) {
	*status = viClose(m_portHandle[port]);
	if(*status > 0)
		*status = 0;
}
	
	