/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HAL/SerialPort.h"

#include "visa/visa.h"

static uint32_t m_resourceManagerHandle;
static uint32_t m_portHandle[2];

extern "C" {

void HAL_InitializeSerialPort(int32_t port, int32_t* status) {
  char const* portName;

  if (m_resourceManagerHandle == 0)
    viOpenDefaultRM(reinterpret_cast<ViSession*>(&m_resourceManagerHandle));

  if (port == 0)
    portName = "ASRL1::INSTR";
  else if (port == 1)
    portName = "ASRL2::INSTR";
  else
    portName = "ASRL3::INSTR";

  *status =
      viOpen(m_resourceManagerHandle, const_cast<char*>(portName), VI_NULL,
             VI_NULL, reinterpret_cast<ViSession*>(&m_portHandle[port]));
  if (*status > 0) *status = 0;
}

void HAL_SetSerialBaudRate(int32_t port, int32_t baud, int32_t* status) {
  *status = viSetAttribute(m_portHandle[port], VI_ATTR_ASRL_BAUD, baud);
  if (*status > 0) *status = 0;
}

void HAL_SetSerialDataBits(int32_t port, int32_t bits, int32_t* status) {
  *status = viSetAttribute(m_portHandle[port], VI_ATTR_ASRL_DATA_BITS, bits);
  if (*status > 0) *status = 0;
}

void HAL_SetSerialParity(int32_t port, int32_t parity, int32_t* status) {
  *status = viSetAttribute(m_portHandle[port], VI_ATTR_ASRL_PARITY, parity);
  if (*status > 0) *status = 0;
}

void HAL_SetSerialStopBits(int32_t port, int32_t stopBits, int32_t* status) {
  *status =
      viSetAttribute(m_portHandle[port], VI_ATTR_ASRL_STOP_BITS, stopBits);
  if (*status > 0) *status = 0;
}

void HAL_SetSerialWriteMode(int32_t port, int32_t mode, int32_t* status) {
  *status = viSetAttribute(m_portHandle[port], VI_ATTR_WR_BUF_OPER_MODE, mode);
  if (*status > 0) *status = 0;
}

void HAL_SetSerialFlowControl(int32_t port, int32_t flow, int32_t* status) {
  *status = viSetAttribute(m_portHandle[port], VI_ATTR_ASRL_FLOW_CNTRL, flow);
  if (*status > 0) *status = 0;
}

void HAL_SetSerialTimeout(int32_t port, double timeout, int32_t* status) {
  *status = viSetAttribute(m_portHandle[port], VI_ATTR_TMO_VALUE,
                           static_cast<uint32_t>(timeout * 1e3));
  if (*status > 0) *status = 0;
}

void HAL_EnableSerialTermination(int32_t port, char terminator,
                                 int32_t* status) {
  viSetAttribute(m_portHandle[port], VI_ATTR_TERMCHAR_EN, VI_TRUE);
  viSetAttribute(m_portHandle[port], VI_ATTR_TERMCHAR, terminator);
  *status = viSetAttribute(m_portHandle[port], VI_ATTR_ASRL_END_IN,
                           VI_ASRL_END_TERMCHAR);
  if (*status > 0) *status = 0;
}

void HAL_DisableSerialTermination(int32_t port, int32_t* status) {
  viSetAttribute(m_portHandle[port], VI_ATTR_TERMCHAR_EN, VI_FALSE);
  *status =
      viSetAttribute(m_portHandle[port], VI_ATTR_ASRL_END_IN, VI_ASRL_END_NONE);
  if (*status > 0) *status = 0;
}

void HAL_SetSerialReadBufferSize(int32_t port, int32_t size, int32_t* status) {
  *status = viSetBuf(m_portHandle[port], VI_READ_BUF, size);
  if (*status > 0) *status = 0;
}

void HAL_SetSerialWriteBufferSize(int32_t port, int32_t size, int32_t* status) {
  *status = viSetBuf(m_portHandle[port], VI_WRITE_BUF, size);
  if (*status > 0) *status = 0;
}

int32_t HAL_GetSerialBytesReceived(int32_t port, int32_t* status) {
  int32_t bytes = 0;

  *status = viGetAttribute(m_portHandle[port], VI_ATTR_ASRL_AVAIL_NUM, &bytes);
  if (*status > 0) *status = 0;
  return bytes;
}

int32_t HAL_ReadSerial(int32_t port, char* buffer, int32_t count,
                       int32_t* status) {
  uint32_t retCount = 0;

  *status =
      viRead(m_portHandle[port], (ViPBuf)buffer, count, (ViPUInt32)&retCount);

  if (*status == VI_ERROR_IO || *status == VI_ERROR_ASRL_OVERRUN ||
      *status == VI_ERROR_ASRL_FRAMING || *status == VI_ERROR_ASRL_PARITY) {
    int32_t localStatus = 0;
    HAL_ClearSerial(port, &localStatus);
  }

  if (*status == VI_ERROR_TMO || *status > 0) *status = 0;
  return static_cast<int32_t>(retCount);
}

int32_t HAL_WriteSerial(int32_t port, const char* buffer, int32_t count,
                        int32_t* status) {
  uint32_t retCount = 0;

  *status =
      viWrite(m_portHandle[port], (ViPBuf)buffer, count, (ViPUInt32)&retCount);

  if (*status > 0) *status = 0;
  return static_cast<int32_t>(retCount);
}

void HAL_FlushSerial(int32_t port, int32_t* status) {
  *status = viFlush(m_portHandle[port], VI_WRITE_BUF);
  if (*status > 0) *status = 0;
}

void HAL_ClearSerial(int32_t port, int32_t* status) {
  *status = viClear(m_portHandle[port]);
  if (*status > 0) *status = 0;
}

void HAL_CloseSerial(int32_t port, int32_t* status) {
  *status = viClose(m_portHandle[port]);
  if (*status > 0) *status = 0;
}

}  // extern "C"
