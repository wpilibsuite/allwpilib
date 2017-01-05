/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2017. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HAL/OSSerialPort.h"

#include <fcntl.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#include <chrono>
#include <cstring>
#include <string>

#include "HAL/Errors.h"
#include "HAL/cpp/SerialHelper.h"

static int portHandles[4]{-1, -1, -1, -1};
static std::chrono::milliseconds portTimeouts[4]{
    std::chrono::milliseconds(0), std::chrono::milliseconds(0),
    std::chrono::milliseconds(0), std::chrono::milliseconds(0)};

extern "C" {

void HAL_InitializeOSSerialPort(HAL_SerialPort port, int32_t* status) {
  std::string portName;

  hal::SerialHelper serialHelper;

  portName = serialHelper.GetOSSerialPortName(port, status);

  if (*status < 0) {
    return;
  }

  int fs = open(portName.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
  if (fs == -1) {
    *status = HAL_SERIAL_PORT_OPEN_ERROR;
    return;
  }
  portHandles[port] = fs;

  struct termios options;
  tcgetattr(fs, &options);
  options.c_cflag = B9600 | CS8 | CLOCAL | CREAD;
  options.c_iflag = 0;
  options.c_oflag = 0;
  options.c_lflag = 0;
  tcflush(fs, TCIFLUSH);
  tcsetattr(fs, TCSANOW, &options);
}

void HAL_SetOSSerialBaudRate(HAL_SerialPort port, int32_t baud,
                             int32_t* status) {
  int baudRate = -1;
  switch (baud) {
    case 9600:
      baudRate = B9600;
      break;
    case 19200:
      baudRate = B19200;
      break;
    case 38400:
      baudRate = B38400;
      break;
    case 57600:
      baudRate = B57600;
      break;
    case 115200:
      baudRate = B115200;
      break;
    default:
      *status = PARAMETER_OUT_OF_RANGE;
      return;
  }

  struct termios options;
  tcgetattr(portHandles[port], &options);
  auto set = cfsetospeed(&options, baudRate);
  if (set != 0) {
    *status = HAL_SERIAL_PORT_ERROR;
    return;
  }
  set = tcsetattr(portHandles[port], TCSANOW, &options);
  if (set != 0) {
    *status = HAL_SERIAL_PORT_ERROR;
    return;
  }
}

void HAL_SetOSSerialDataBits(HAL_SerialPort port, int32_t bits,
                             int32_t* status) {
  int numBits = -1;
  switch (bits) {
    case 5:
      numBits = CS5;
      break;
    case 6:
      numBits = CS6;
      break;
    case 7:
      numBits = CS7;
      break;
    case 8:
      numBits = CS8;
      break;
    default:
      *status = PARAMETER_OUT_OF_RANGE;
      return;
  }

  struct termios options;
  tcgetattr(portHandles[port], &options);
  options.c_cflag &= ~CSIZE;
  options.c_cflag |= numBits;
  auto set = tcsetattr(portHandles[port], TCSANOW, &options);
  if (set != 0) {
    *status = HAL_SERIAL_PORT_ERROR;
    return;
  }
}

void HAL_SetOSSerialParity(HAL_SerialPort port, int32_t parity,
                           int32_t* status) {
  // Just set none parity
  struct termios options;
  tcgetattr(portHandles[port], &options);
  options.c_cflag &= ~PARENB;
  auto set = tcsetattr(portHandles[port], TCSANOW, &options);
  if (set != 0) {
    *status = HAL_SERIAL_PORT_ERROR;
    return;
  }
}

void HAL_SetOSSerialStopBits(HAL_SerialPort port, int32_t stopBits,
                             int32_t* status) {
  // Force 1 stop bit
  struct termios options;
  tcgetattr(portHandles[port], &options);
  options.c_cflag &= ~CSTOPB;
  auto set = tcsetattr(portHandles[port], TCSANOW, &options);
  if (set != 0) {
    *status = HAL_SERIAL_PORT_ERROR;
    return;
  }
}

void HAL_SetOSSerialWriteMode(HAL_SerialPort port, int32_t mode,
                              int32_t* status) {
  // No op
}

void HAL_SetOSSerialFlowControl(HAL_SerialPort port, int32_t flow,
                                int32_t* status) {
  // No op
}

void HAL_SetOSSerialTimeout(HAL_SerialPort port, double timeout,
                            int32_t* status) {
  // Convert to millis
  int t = timeout / 1000;
  portTimeouts[port] = std::chrono::milliseconds(t);
}

void HAL_EnableOSSerialTermination(HAL_SerialPort port, char terminator,
                                   int32_t* status) {
  // \n is hardcoded for now. Will fix later
  // Seems like a VISA only setting, need to check
}

void HAL_DisableOSSerialTermination(HAL_SerialPort port, int32_t* status) {
  // Seems like a VISA only setting, need to check
}

void HAL_SetOSSerialReadBufferSize(HAL_SerialPort port, int32_t size,
                                   int32_t* status) {
  // No op
}

void HAL_SetOSSerialWriteBufferSize(HAL_SerialPort port, int32_t size,
                                    int32_t* status) {
  // No op
}

int32_t HAL_GetOSSerialBytesReceived(HAL_SerialPort port, int32_t* status) {
  int bytes = 0;
  ioctl(portHandles[port], FIONREAD, &bytes);
  return bytes;
}

int32_t HAL_ReadOSSerial(HAL_SerialPort port, char* buffer, int32_t count,
                         int32_t* status) {
  auto endTime = std::chrono::steady_clock::now() + portTimeouts[port];

  int bytesRead = 0;

  unsigned char buf[256];

  do {
    int rx = read(portHandles[port], buf, count - bytesRead);
    std::memcpy(&buffer[bytesRead], buf, rx);
    bytesRead += rx;
    if (bytesRead >= count) break;
    llvm::StringRef tmp(buffer, bytesRead);
    auto loc = tmp.find('\n');
    if (loc != llvm::StringRef::npos) {
      bytesRead = loc;
      break;
    }
  } while (std::chrono::steady_clock::now() < endTime);
  return bytesRead;
}

int32_t HAL_WriteOSSerial(HAL_SerialPort port, const char* buffer,
                          int32_t count, int32_t* status) {
  return write(portHandles[port], buffer, count);
}
void HAL_FlushOSSerial(HAL_SerialPort port, int32_t* status) {
  tcdrain(portHandles[port]);
}
void HAL_ClearOSSerial(HAL_SerialPort port, int32_t* status) {
  tcflush(portHandles[port], TCIOFLUSH);
}
void HAL_CloseOSSerial(HAL_SerialPort port, int32_t* status) {
  close(portHandles[port]);
}
}
