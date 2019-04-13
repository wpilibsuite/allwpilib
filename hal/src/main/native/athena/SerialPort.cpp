/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "hal/SerialPort.h"

#include <string>

#include "HALInitializer.h"
#include "hal/cpp/SerialHelper.h"
#include "visa/visa.h"

static int32_t resourceManagerHandle{0};
static HAL_SerialPort portHandles[4];

namespace hal {
namespace init {
void InitializeSerialPort() {}
}  // namespace init
}  // namespace hal

extern "C" {

void HAL_InitializeSerialPort(HAL_SerialPort port, int32_t* status) {
  hal::init::CheckInit();
  std::string portName;

  if (resourceManagerHandle == 0)
    viOpenDefaultRM(reinterpret_cast<ViSession*>(&resourceManagerHandle));

  hal::SerialHelper serialHelper;

  portName = serialHelper.GetVISASerialPortName(port, status);

  if (*status < 0) {
    return;
  }

  *status = viOpen(resourceManagerHandle, const_cast<char*>(portName.c_str()),
                   VI_NULL, VI_NULL,
                   reinterpret_cast<ViSession*>(&portHandles[port]));
  if (*status > 0) *status = 0;
}

void HAL_InitializeSerialPortDirect(HAL_SerialPort port, const char* portName,
                                    int32_t* status) {
  *status = viOpen(resourceManagerHandle, const_cast<char*>(portName), VI_NULL,
                   VI_NULL, reinterpret_cast<ViSession*>(&portHandles[port]));
  if (*status > 0) *status = 0;
}

void HAL_SetSerialBaudRate(HAL_SerialPort port, int32_t baud, int32_t* status) {
  *status = viSetAttribute(portHandles[port], VI_ATTR_ASRL_BAUD, baud);
  if (*status > 0) *status = 0;
}

void HAL_SetSerialDataBits(HAL_SerialPort port, int32_t bits, int32_t* status) {
  *status = viSetAttribute(portHandles[port], VI_ATTR_ASRL_DATA_BITS, bits);
  if (*status > 0) *status = 0;
}

void HAL_SetSerialParity(HAL_SerialPort port, int32_t parity, int32_t* status) {
  *status = viSetAttribute(portHandles[port], VI_ATTR_ASRL_PARITY, parity);
  if (*status > 0) *status = 0;
}

void HAL_SetSerialStopBits(HAL_SerialPort port, int32_t stopBits,
                           int32_t* status) {
  *status = viSetAttribute(portHandles[port], VI_ATTR_ASRL_STOP_BITS, stopBits);
  if (*status > 0) *status = 0;
}

void HAL_SetSerialWriteMode(HAL_SerialPort port, int32_t mode,
                            int32_t* status) {
  *status = viSetAttribute(portHandles[port], VI_ATTR_WR_BUF_OPER_MODE, mode);
  if (*status > 0) *status = 0;
}

void HAL_SetSerialFlowControl(HAL_SerialPort port, int32_t flow,
                              int32_t* status) {
  *status = viSetAttribute(portHandles[port], VI_ATTR_ASRL_FLOW_CNTRL, flow);
  if (*status > 0) *status = 0;
}

void HAL_SetSerialTimeout(HAL_SerialPort port, double timeout,
                          int32_t* status) {
  *status = viSetAttribute(portHandles[port], VI_ATTR_TMO_VALUE,
                           static_cast<uint32_t>(timeout * 1e3));
  if (*status > 0) *status = 0;
}

void HAL_EnableSerialTermination(HAL_SerialPort port, char terminator,
                                 int32_t* status) {
  viSetAttribute(portHandles[port], VI_ATTR_TERMCHAR_EN, VI_TRUE);
  viSetAttribute(portHandles[port], VI_ATTR_TERMCHAR, terminator);
  *status = viSetAttribute(portHandles[port], VI_ATTR_ASRL_END_IN,
                           VI_ASRL_END_TERMCHAR);
  if (*status > 0) *status = 0;
}

void HAL_DisableSerialTermination(HAL_SerialPort port, int32_t* status) {
  viSetAttribute(portHandles[port], VI_ATTR_TERMCHAR_EN, VI_FALSE);
  *status =
      viSetAttribute(portHandles[port], VI_ATTR_ASRL_END_IN, VI_ASRL_END_NONE);
  if (*status > 0) *status = 0;
}

void HAL_SetSerialReadBufferSize(HAL_SerialPort port, int32_t size,
                                 int32_t* status) {
  *status = viSetBuf(portHandles[port], VI_READ_BUF, size);
  if (*status > 0) *status = 0;
}

void HAL_SetSerialWriteBufferSize(HAL_SerialPort port, int32_t size,
                                  int32_t* status) {
  *status = viSetBuf(portHandles[port], VI_WRITE_BUF, size);
  if (*status > 0) *status = 0;
}

int32_t HAL_GetSerialBytesReceived(HAL_SerialPort port, int32_t* status) {
  int32_t bytes = 0;

  *status = viGetAttribute(portHandles[port], VI_ATTR_ASRL_AVAIL_NUM, &bytes);
  if (*status > 0) *status = 0;
  return bytes;
}

int32_t HAL_ReadSerial(HAL_SerialPort port, char* buffer, int32_t count,
                       int32_t* status) {
  uint32_t retCount = 0;

  *status =
      viRead(portHandles[port], (ViPBuf)buffer, count, (ViPUInt32)&retCount);

  if (*status == VI_ERROR_IO || *status == VI_ERROR_ASRL_OVERRUN ||
      *status == VI_ERROR_ASRL_FRAMING || *status == VI_ERROR_ASRL_PARITY) {
    int32_t localStatus = 0;
    HAL_ClearSerial(port, &localStatus);
  }

  if (*status == VI_ERROR_TMO || *status > 0) *status = 0;
  return static_cast<int32_t>(retCount);
}

int32_t HAL_WriteSerial(HAL_SerialPort port, const char* buffer, int32_t count,
                        int32_t* status) {
  uint32_t retCount = 0;

  *status =
      viWrite(portHandles[port], (ViPBuf)buffer, count, (ViPUInt32)&retCount);

  if (*status > 0) *status = 0;
  return static_cast<int32_t>(retCount);
}

void HAL_FlushSerial(HAL_SerialPort port, int32_t* status) {
  *status = viFlush(portHandles[port], VI_WRITE_BUF);
  if (*status > 0) *status = 0;
}

void HAL_ClearSerial(HAL_SerialPort port, int32_t* status) {
  *status = viClear(portHandles[port]);
  if (*status > 0) *status = 0;
}

void HAL_CloseSerial(HAL_SerialPort port, int32_t* status) {
  *status = viClose(portHandles[port]);
  if (*status > 0) *status = 0;
}

}  // extern "C"
