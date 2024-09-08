// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/SerialPort.h"

#include <fcntl.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#include <cerrno>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <string>
#include <thread>

#include <fmt/format.h>

#include "HALInternal.h"
#include "hal/cpp/SerialHelper.h"
#include "hal/handles/HandlesInternal.h"
#include "hal/handles/IndexedHandleResource.h"

namespace {
struct SerialPort {
  int portId;
  struct termios tty;
  int baudRate;

  double timeout = 0;

  bool termination = false;
  char terminationChar = '\n';
};
}  // namespace

namespace hal {
IndexedHandleResource<HAL_SerialPortHandle, SerialPort, 4,
                      HAL_HandleEnum::SerialPort>* serialPortHandles;
}  // namespace hal

namespace hal::init {
void InitializeSerialPort() {
  static IndexedHandleResource<HAL_SerialPortHandle, SerialPort, 4,
                               HAL_HandleEnum::SerialPort>
      spH;
  serialPortHandles = &spH;
}
}  // namespace hal::init

using namespace hal;

extern "C" {
HAL_SerialPortHandle HAL_InitializeSerialPort(HAL_SerialPort port,
                                              int32_t* status) {
  // hal::init::CheckInit();

  hal::SerialHelper serialHelper;

  std::string portName = serialHelper.GetOSSerialPortName(port, status);

  if (*status < 0) {
    return HAL_kInvalidHandle;
  }

  return HAL_InitializeSerialPortDirect(port, portName.c_str(), status);
}
HAL_SerialPortHandle HAL_InitializeSerialPortDirect(HAL_SerialPort port,
                                                    const char* portName,
                                                    int32_t* status) {
  HAL_SerialPortHandle handle;
  auto serialPort =
      serialPortHandles->Allocate(static_cast<int16_t>(port), &handle, status);

  if (*status != 0) {
    return HAL_kInvalidHandle;
  }

  serialPort->portId = open(portName, O_RDWR | O_NOCTTY);
  if (serialPort->portId < 0) {
    *status = -errno;
    if (*status == EACCES) {
      *status = HAL_CONSOLE_OUT_ENABLED_ERROR;
    }
    serialPortHandles->Free(handle);
    return HAL_kInvalidHandle;
  }

  std::memset(&serialPort->tty, 0, sizeof(serialPort->tty));

  serialPort->baudRate = B9600;
  if (cfsetospeed(&serialPort->tty,
                  static_cast<speed_t>(serialPort->baudRate)) != 0) {
    *status = -errno;
    close(serialPort->portId);
    serialPortHandles->Free(handle);
    return HAL_kInvalidHandle;
  }
  if (cfsetispeed(&serialPort->tty,
                  static_cast<speed_t>(serialPort->baudRate)) != 0) {
    *status = -errno;
    close(serialPort->portId);
    serialPortHandles->Free(handle);
    return HAL_kInvalidHandle;
  }

  serialPort->tty.c_cflag &= ~PARENB;
  serialPort->tty.c_cflag &= ~CSTOPB;
  serialPort->tty.c_cflag &= ~CSIZE;
  serialPort->tty.c_cflag |= CS8;

  serialPort->tty.c_cc[VMIN] = 0;
  serialPort->tty.c_cc[VTIME] = 10;

  serialPort->tty.c_cflag |= CREAD | CLOCAL;

  serialPort->tty.c_lflag &= ~(ICANON | ECHO | ISIG);
  serialPort->tty.c_iflag &= ~(IXON | IXOFF | IXANY);
  /* Raw output mode, sends the raw and unprocessed data  (send as it is).
   * If it is in canonical mode and sending new line char then CR
   * will be added as prefix and send as CR LF
   */
  serialPort->tty.c_oflag = ~OPOST;

  if (tcflush(serialPort->portId, TCIOFLUSH) != 0) {
    *status = -errno;
    close(serialPort->portId);
    serialPortHandles->Free(handle);
    return HAL_kInvalidHandle;
  }
  if (tcsetattr(serialPort->portId, TCSANOW, &serialPort->tty) != 0) {
    *status = -errno;
    close(serialPort->portId);
    serialPortHandles->Free(handle);
    return HAL_kInvalidHandle;
  }
  return handle;
}

void HAL_CloseSerial(HAL_SerialPortHandle handle) {
  auto port = serialPortHandles->Get(handle);
  serialPortHandles->Free(handle);

  if (port) {
    close(port->portId);
  }
}

int HAL_GetSerialFD(HAL_SerialPortHandle handle, int32_t* status) {
  auto port = serialPortHandles->Get(handle);
  if (!port) {
    *status = HAL_HANDLE_ERROR;
    return -1;
  }
  return port->portId;
}

#define BAUDCASE(BAUD)        \
  case BAUD:                  \
    port->baudRate = B##BAUD; \
    break;

void HAL_SetSerialBaudRate(HAL_SerialPortHandle handle, int32_t baud,
                           int32_t* status) {
  auto port = serialPortHandles->Get(handle);
  if (!port) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  switch (baud) {
    BAUDCASE(50)
    BAUDCASE(75)
    BAUDCASE(110)
    BAUDCASE(134)
    BAUDCASE(150)
    BAUDCASE(200)
    BAUDCASE(300)
    BAUDCASE(600)
    BAUDCASE(1200)
    BAUDCASE(1800)
    BAUDCASE(2400)
    BAUDCASE(4800)
    BAUDCASE(9600)
    BAUDCASE(19200)
    BAUDCASE(38400)
    BAUDCASE(57600)
    BAUDCASE(115200)
    BAUDCASE(230400)
    BAUDCASE(460800)
    BAUDCASE(500000)
    BAUDCASE(576000)
    BAUDCASE(921600)
    BAUDCASE(1000000)
    BAUDCASE(1152000)
    BAUDCASE(1500000)
    BAUDCASE(2000000)
    BAUDCASE(2500000)
    BAUDCASE(3000000)
    BAUDCASE(3500000)
    BAUDCASE(4000000)
    default:
      *status = PARAMETER_OUT_OF_RANGE;
      hal::SetLastError(status, fmt::format("Invalid BaudRate: {}", baud));
      return;
  }
  int err = cfsetospeed(&port->tty, static_cast<speed_t>(port->baudRate));
  if (err < 0) {
    *status = errno;
    return;
  }
  err = cfsetispeed(&port->tty, static_cast<speed_t>(port->baudRate));
  if (err < 0) {
    *status = errno;
    return;
  }
  err = tcsetattr(port->portId, TCSANOW, &port->tty);
  if (err < 0) {
    *status = errno;
  }
}

void HAL_SetSerialDataBits(HAL_SerialPortHandle handle, int32_t bits,
                           int32_t* status) {
  auto port = serialPortHandles->Get(handle);
  if (!port) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  int bitFlag = -1;
  switch (bits) {
    case 5:
      bitFlag = CS5;
      break;
    case 6:
      bitFlag = CS6;
      break;
    case 7:
      bitFlag = CS7;
      break;
    case 8:
      bitFlag = CS8;
      break;
    default:
      *status = PARAMETER_OUT_OF_RANGE;
      hal::SetLastError(status, fmt::format("Invalid data bits: {}", bits));
      return;
  }

  port->tty.c_cflag &= ~CSIZE;
  port->tty.c_cflag |= bitFlag;

  int err = tcsetattr(port->portId, TCSANOW, &port->tty);
  if (err < 0) {
    *status = errno;
  }
}

void HAL_SetSerialParity(HAL_SerialPortHandle handle, int32_t parity,
                         int32_t* status) {
  auto port = serialPortHandles->Get(handle);
  if (!port) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  switch (parity) {
    case 0:  // None
      port->tty.c_cflag &= ~PARENB;
      port->tty.c_cflag &= ~CMSPAR;
      break;
    case 1:  // Odd
      port->tty.c_cflag |= PARENB;
      port->tty.c_cflag &= ~CMSPAR;
      port->tty.c_cflag &= ~PARODD;
      break;
    case 2:  // Even
      port->tty.c_cflag |= PARENB;
      port->tty.c_cflag &= ~CMSPAR;
      port->tty.c_cflag |= PARODD;
      break;
    case 3:  // Mark
      port->tty.c_cflag |= PARENB;
      port->tty.c_cflag |= CMSPAR;
      port->tty.c_cflag |= PARODD;
      break;
    case 4:  // Space
      port->tty.c_cflag |= PARENB;
      port->tty.c_cflag |= CMSPAR;
      port->tty.c_cflag &= ~PARODD;
      break;
    default:
      *status = PARAMETER_OUT_OF_RANGE;
      hal::SetLastError(status, fmt::format("Invalid parity bits: {}", parity));
      return;
  }

  int err = tcsetattr(port->portId, TCSANOW, &port->tty);
  if (err < 0) {
    *status = errno;
  }
}

void HAL_SetSerialStopBits(HAL_SerialPortHandle handle, int32_t stopBits,
                           int32_t* status) {
  auto port = serialPortHandles->Get(handle);
  if (!port) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  switch (stopBits) {
    case 10:  // 1
      port->tty.c_cflag &= ~CSTOPB;
      break;
    case 15:  // 1.5
    case 20:  // 2
      port->tty.c_cflag |= CSTOPB;
      break;
    default:
      *status = PARAMETER_OUT_OF_RANGE;
      hal::SetLastError(status, fmt::format("Invalid stop bits: {}", stopBits));
      return;
  }

  int err = tcsetattr(port->portId, TCSANOW, &port->tty);
  if (err < 0) {
    *status = errno;
  }
}

void HAL_SetSerialWriteMode(HAL_SerialPortHandle handle, int32_t mode,
                            int32_t* status) {
  // This seems to be a no op on the NI serial port driver
}

void HAL_SetSerialFlowControl(HAL_SerialPortHandle handle, int32_t flow,
                              int32_t* status) {
  auto port = serialPortHandles->Get(handle);
  if (!port) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  switch (flow) {
    case 0:
      port->tty.c_cflag &= ~CRTSCTS;
      break;
    case 1:
      port->tty.c_cflag &= ~CRTSCTS;
      port->tty.c_iflag &= IXON | IXOFF;
      break;
    case 2:
      port->tty.c_cflag |= CRTSCTS;
      break;
    default:
      *status = PARAMETER_OUT_OF_RANGE;
      hal::SetLastError(status, fmt::format("Invalid fc bits: {}", flow));
      return;
  }

  int err = tcsetattr(port->portId, TCSANOW, &port->tty);
  if (err < 0) {
    *status = errno;
  }
}

void HAL_SetSerialTimeout(HAL_SerialPortHandle handle, double timeout,
                          int32_t* status) {
  auto port = serialPortHandles->Get(handle);
  if (!port) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  port->timeout = timeout;
  port->tty.c_cc[VTIME] = static_cast<int>(timeout * 10);
  int err = tcsetattr(port->portId, TCSANOW, &port->tty);
  if (err < 0) {
    *status = errno;
  }
}

void HAL_EnableSerialTermination(HAL_SerialPortHandle handle, char terminator,
                                 int32_t* status) {
  auto port = serialPortHandles->Get(handle);
  if (!port) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  port->termination = true;
  port->terminationChar = terminator;
}

void HAL_DisableSerialTermination(HAL_SerialPortHandle handle,
                                  int32_t* status) {
  auto port = serialPortHandles->Get(handle);
  if (!port) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  port->termination = false;
}

void HAL_SetSerialReadBufferSize(HAL_SerialPortHandle handle, int32_t size,
                                 int32_t* status) {
  // NO OP currently
}

void HAL_SetSerialWriteBufferSize(HAL_SerialPortHandle handle, int32_t size,
                                  int32_t* status) {
  // NO OP currently
}

int32_t HAL_GetSerialBytesReceived(HAL_SerialPortHandle handle,
                                   int32_t* status) {
  auto port = serialPortHandles->Get(handle);
  if (!port) {
    *status = HAL_HANDLE_ERROR;
    return -1;
  }
  int bytes = 0;
  int err = ioctl(port->portId, FIONREAD, &bytes);
  if (err < 0) {
    *status = errno;
  }
  return bytes;
}

int32_t HAL_ReadSerial(HAL_SerialPortHandle handle, char* buffer, int32_t count,
                       int32_t* status) {
  // Don't do anything if 0 bytes were requested
  if (count == 0) {
    return 0;
  }

  auto port = serialPortHandles->Get(handle);
  if (!port) {
    *status = HAL_HANDLE_ERROR;
    return -1;
  }

  int n = 0, loc = 0;
  char buf = '\0';
  std::memset(buffer, '\0', count);
  *status = 0;

  do {
    n = read(port->portId, &buf, 1);
    if (n == 1) {
      buffer[loc] = buf;
      loc++;
      // If buffer is full, return
      if (loc == count) {
        return loc;
      }
      // If terminating, and termination was hit return;
      if (port->termination && buf == port->terminationChar) {
        return loc;
      }
    } else if (n == -1) {
      // ERROR
      *status = errno;
      return loc;
    } else {
      // If nothing read, timeout
      return loc;
    }
  } while (true);
}

int32_t HAL_WriteSerial(HAL_SerialPortHandle handle, const char* buffer,
                        int32_t count, int32_t* status) {
  auto port = serialPortHandles->Get(handle);
  if (!port) {
    *status = HAL_HANDLE_ERROR;
    return -1;
  }

  int written = 0, spot = 0;
  do {
    written = write(port->portId, buffer + spot, count - spot);
    if (written < 0) {
      *status = errno;
      return spot;
    }
    spot += written;
  } while (spot < count);
  return spot;
}

void HAL_FlushSerial(HAL_SerialPortHandle handle, int32_t* status) {
  auto port = serialPortHandles->Get(handle);
  if (!port) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  int err = tcdrain(port->portId);
  if (err < 0) {
    *status = errno;
  }
}
void HAL_ClearSerial(HAL_SerialPortHandle handle, int32_t* status) {
  auto port = serialPortHandles->Get(handle);
  if (!port) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  int err = tcflush(port->portId, TCIOFLUSH);
  if (err < 0) {
    *status = errno;
  }
}
}  // extern "C"
