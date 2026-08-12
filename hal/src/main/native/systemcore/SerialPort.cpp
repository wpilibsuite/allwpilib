// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hal/SerialPort.h"

#include <fcntl.h>
#include <poll.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#include <algorithm>
#include <cerrno>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <limits>
#include <memory>
#include <mutex>

#include "HALInitializer.hpp"
#include "wpi/hal/Errors.h"
#include "wpi/hal/handles/UnlimitedHandleResource.hpp"

using namespace wpi::hal;

namespace {

struct SerialPortStorage {
  explicit SerialPortStorage(int fd) : fd{fd} {}

  ~SerialPortStorage() { close(fd); }

  int fd;
  double timeout = 5.0;
  bool terminationEnabled = false;
  char terminator = '\n';
  bool flushOnAccess = true;
  std::mutex mutex;
};

UnlimitedHandleResource<HAL_SerialPortHandle, SerialPortStorage,
                        HAL_HandleEnum::SERIAL_PORT>* serialPortHandles;

std::shared_ptr<SerialPortStorage> GetSerialPort(HAL_SerialPortHandle handle,
                                                 int32_t* status) {
  auto port = serialPortHandles->Get(handle);
  if (!port) {
    *status = HAL_HANDLE_ERROR;
  }
  return port;
}

bool GetBaudRate(int32_t baud, speed_t* speed) {
  switch (baud) {
    case 50:
      *speed = B50;
      return true;
    case 75:
      *speed = B75;
      return true;
    case 110:
      *speed = B110;
      return true;
    case 134:
      *speed = B134;
      return true;
    case 150:
      *speed = B150;
      return true;
    case 200:
      *speed = B200;
      return true;
    case 300:
      *speed = B300;
      return true;
    case 600:
      *speed = B600;
      return true;
    case 1200:
      *speed = B1200;
      return true;
    case 1800:
      *speed = B1800;
      return true;
    case 2400:
      *speed = B2400;
      return true;
    case 4800:
      *speed = B4800;
      return true;
    case 9600:
      *speed = B9600;
      return true;
    case 19200:
      *speed = B19200;
      return true;
    case 38400:
      *speed = B38400;
      return true;
    case 57600:
      *speed = B57600;
      return true;
    case 115200:
      *speed = B115200;
      return true;
    case 230400:
      *speed = B230400;
      return true;
#ifdef B460800
    case 460800:
      *speed = B460800;
      return true;
#endif
#ifdef B500000
    case 500000:
      *speed = B500000;
      return true;
#endif
#ifdef B576000
    case 576000:
      *speed = B576000;
      return true;
#endif
#ifdef B921600
    case 921600:
      *speed = B921600;
      return true;
#endif
#ifdef B1000000
    case 1000000:
      *speed = B1000000;
      return true;
#endif
#ifdef B1152000
    case 1152000:
      *speed = B1152000;
      return true;
#endif
#ifdef B1500000
    case 1500000:
      *speed = B1500000;
      return true;
#endif
#ifdef B2000000
    case 2000000:
      *speed = B2000000;
      return true;
#endif
#ifdef B2500000
    case 2500000:
      *speed = B2500000;
      return true;
#endif
#ifdef B3000000
    case 3000000:
      *speed = B3000000;
      return true;
#endif
#ifdef B3500000
    case 3500000:
      *speed = B3500000;
      return true;
#endif
#ifdef B4000000
    case 4000000:
      *speed = B4000000;
      return true;
#endif
    default:
      return false;
  }
}

bool ApplyTermios(SerialPortStorage& port, termios& settings, int32_t* status) {
  if (tcsetattr(port.fd, TCSANOW, &settings) != 0) {
    *status = HAL_SERIAL_PORT_ERROR;
    return false;
  }
  return true;
}

bool GetTermios(SerialPortStorage& port, termios* settings, int32_t* status) {
  if (tcgetattr(port.fd, settings) != 0) {
    *status = HAL_SERIAL_PORT_ERROR;
    return false;
  }
  return true;
}

int GetPollTimeout(std::chrono::steady_clock::time_point deadline) {
  auto remaining = deadline - std::chrono::steady_clock::now();
  if (remaining <= std::chrono::steady_clock::duration::zero()) {
    return 0;
  }

  auto milliseconds = std::chrono::ceil<std::chrono::milliseconds>(remaining);
  return static_cast<int>(
      std::min<int64_t>(milliseconds.count(), std::numeric_limits<int>::max()));
}

int Poll(int fd, short events, int timeout) {
  pollfd descriptor{fd, events, 0};
  int result;
  do {
    result = poll(&descriptor, 1, timeout);
  } while (result < 0 && errno == EINTR);

  if (result > 0 && (descriptor.revents & (POLLERR | POLLHUP | POLLNVAL))) {
    errno = EIO;
    return -1;
  }
  return result;
}

}  // namespace

namespace wpi::hal::init {
void InitializeSerialPort() {
  static UnlimitedHandleResource<HAL_SerialPortHandle, SerialPortStorage,
                                 HAL_HandleEnum::SERIAL_PORT>
      handles;
  serialPortHandles = &handles;
}
}  // namespace wpi::hal::init

extern "C" {
HAL_SerialPortHandle HAL_InitializeSerialPort(const char* portName,
                                              int32_t* status) {
  wpi::hal::init::CheckInit();

  if (!portName) {
    *status = HAL_NULL_PARAMETER;
    return HAL_INVALID_HANDLE;
  }

  int fd = open(portName, O_RDWR | O_NOCTTY | O_NONBLOCK | O_CLOEXEC);
  if (fd < 0) {
    *status = errno == ENOENT || errno == ENODEV ? HAL_SERIAL_PORT_NOT_FOUND
                                                 : HAL_SERIAL_PORT_OPEN_ERROR;
    return HAL_INVALID_HANDLE;
  }

  auto port = std::make_shared<SerialPortStorage>(fd);
  termios settings;
  if (!GetTermios(*port, &settings, status)) {
    return HAL_INVALID_HANDLE;
  }

  cfmakeraw(&settings);
  settings.c_cflag |= CLOCAL | CREAD;
  settings.c_cflag &= ~CSIZE;
  settings.c_cflag |= CS8;
  cfsetispeed(&settings, B9600);
  cfsetospeed(&settings, B9600);
  if (!ApplyTermios(*port, settings, status)) {
    return HAL_INVALID_HANDLE;
  }

  auto handle = serialPortHandles->Allocate(port);
  if (handle == HAL_INVALID_HANDLE) {
    *status = HAL_NO_AVAILABLE_RESOURCES;
  }
  return handle;
}

void HAL_CloseSerial(HAL_SerialPortHandle handle) {
  serialPortHandles->Free(handle);
}

int HAL_GetSerialFD(HAL_SerialPortHandle handle, int32_t* status) {
  auto port = GetSerialPort(handle, status);
  return port ? port->fd : -1;
}

void HAL_SetSerialBaudRate(HAL_SerialPortHandle handle, int32_t baud,
                           int32_t* status) {
  auto port = GetSerialPort(handle, status);
  if (!port) {
    return;
  }

  speed_t speed;
  if (!GetBaudRate(baud, &speed)) {
    *status = HAL_PARAMETER_OUT_OF_RANGE;
    return;
  }

  std::scoped_lock lock{port->mutex};
  termios settings;
  if (!GetTermios(*port, &settings, status)) {
    return;
  }
  cfsetispeed(&settings, speed);
  cfsetospeed(&settings, speed);
  ApplyTermios(*port, settings, status);
}

void HAL_SetSerialDataBits(HAL_SerialPortHandle handle, int32_t bits,
                           int32_t* status) {
  auto port = GetSerialPort(handle, status);
  if (!port) {
    return;
  }

  tcflag_t dataBits;
  switch (bits) {
    case 5:
      dataBits = CS5;
      break;
    case 6:
      dataBits = CS6;
      break;
    case 7:
      dataBits = CS7;
      break;
    case 8:
      dataBits = CS8;
      break;
    default:
      *status = HAL_PARAMETER_OUT_OF_RANGE;
      return;
  }

  std::scoped_lock lock{port->mutex};
  termios settings;
  if (!GetTermios(*port, &settings, status)) {
    return;
  }
  settings.c_cflag &= ~CSIZE;
  settings.c_cflag |= dataBits;
  ApplyTermios(*port, settings, status);
}

void HAL_SetSerialParity(HAL_SerialPortHandle handle, int32_t parity,
                         int32_t* status) {
  auto port = GetSerialPort(handle, status);
  if (!port) {
    return;
  }

  if (parity < 0 || parity > 4) {
    *status = HAL_PARAMETER_OUT_OF_RANGE;
    return;
  }

#ifndef CMSPAR
  if (parity == 3 || parity == 4) {
    *status = HAL_PARAMETER_OUT_OF_RANGE;
    return;
  }
#endif

  std::scoped_lock lock{port->mutex};
  termios settings;
  if (!GetTermios(*port, &settings, status)) {
    return;
  }

  settings.c_cflag &= ~(PARENB | PARODD);
  settings.c_iflag &= ~INPCK;
#ifdef CMSPAR
  settings.c_cflag &= ~CMSPAR;
#endif
  if (parity != 0) {
    settings.c_cflag |= PARENB;
    settings.c_iflag |= INPCK;
  }
  if (parity == 1) {
    settings.c_cflag |= PARODD;
  }
#ifdef CMSPAR
  if (parity == 3 || parity == 4) {
    settings.c_cflag |= CMSPAR;
    if (parity == 3) {
      settings.c_cflag |= PARODD;
    }
  }
#endif
  ApplyTermios(*port, settings, status);
}

void HAL_SetSerialStopBits(HAL_SerialPortHandle handle, int32_t stopBits,
                           int32_t* status) {
  auto port = GetSerialPort(handle, status);
  if (!port) {
    return;
  }
  if (stopBits != 10 && stopBits != 15 && stopBits != 20) {
    *status = HAL_PARAMETER_OUT_OF_RANGE;
    return;
  }

  std::scoped_lock lock{port->mutex};
  termios settings;
  if (!GetTermios(*port, &settings, status)) {
    return;
  }
  if (stopBits == 10) {
    settings.c_cflag &= ~CSTOPB;
  } else {
    settings.c_cflag |= CSTOPB;
  }
  ApplyTermios(*port, settings, status);
}

void HAL_SetSerialWriteMode(HAL_SerialPortHandle handle, int32_t mode,
                            int32_t* status) {
  auto port = GetSerialPort(handle, status);
  if (!port) {
    return;
  }
  if (mode != 1 && mode != 2) {
    *status = HAL_PARAMETER_OUT_OF_RANGE;
    return;
  }

  std::scoped_lock lock{port->mutex};
  port->flushOnAccess = mode == 1;
}

void HAL_SetSerialFlowControl(HAL_SerialPortHandle handle, int32_t flow,
                              int32_t* status) {
  auto port = GetSerialPort(handle, status);
  if (!port) {
    return;
  }
  if (flow < 0 || flow > 2) {
    *status = HAL_PARAMETER_OUT_OF_RANGE;
    return;
  }

  std::scoped_lock lock{port->mutex};
  termios settings;
  if (!GetTermios(*port, &settings, status)) {
    return;
  }
  settings.c_iflag &= ~(IXON | IXOFF | IXANY);
  settings.c_cflag &= ~CRTSCTS;
  if (flow == 1) {
    settings.c_iflag |= IXON | IXOFF;
  } else if (flow == 2) {
    settings.c_cflag |= CRTSCTS;
  }
  ApplyTermios(*port, settings, status);
}

void HAL_SetSerialTimeout(HAL_SerialPortHandle handle, double timeout,
                          int32_t* status) {
  auto port = GetSerialPort(handle, status);
  if (!port) {
    return;
  }
  if (!std::isfinite(timeout) || timeout < 0.0) {
    *status = HAL_PARAMETER_OUT_OF_RANGE;
    return;
  }

  std::scoped_lock lock{port->mutex};
  port->timeout = timeout;
}

void HAL_EnableSerialTermination(HAL_SerialPortHandle handle, char terminator,
                                 int32_t* status) {
  auto port = GetSerialPort(handle, status);
  if (!port) {
    return;
  }

  std::scoped_lock lock{port->mutex};
  port->terminator = terminator;
  port->terminationEnabled = true;
}

void HAL_DisableSerialTermination(HAL_SerialPortHandle handle,
                                  int32_t* status) {
  auto port = GetSerialPort(handle, status);
  if (!port) {
    return;
  }

  std::scoped_lock lock{port->mutex};
  port->terminationEnabled = false;
}

void HAL_SetSerialReadBufferSize(HAL_SerialPortHandle handle, int32_t size,
                                 int32_t* status) {
  if (!GetSerialPort(handle, status)) {
    return;
  }
  if (size < 1) {
    *status = HAL_PARAMETER_OUT_OF_RANGE;
  }
}

void HAL_SetSerialWriteBufferSize(HAL_SerialPortHandle handle, int32_t size,
                                  int32_t* status) {
  if (!GetSerialPort(handle, status)) {
    return;
  }
  if (size < 1) {
    *status = HAL_PARAMETER_OUT_OF_RANGE;
  }
}

int32_t HAL_GetSerialBytesReceived(HAL_SerialPortHandle handle,
                                   int32_t* status) {
  auto port = GetSerialPort(handle, status);
  if (!port) {
    return -1;
  }

  std::scoped_lock lock{port->mutex};
  int count;
  if (ioctl(port->fd, FIONREAD, &count) != 0) {
    *status = HAL_SERIAL_PORT_ERROR;
    return -1;
  }
  return count;
}

int32_t HAL_ReadSerial(HAL_SerialPortHandle handle, char* buffer, int32_t count,
                       int32_t* status) {
  auto port = GetSerialPort(handle, status);
  if (!port) {
    return -1;
  }
  if (!buffer && count != 0) {
    *status = HAL_NULL_PARAMETER;
    return -1;
  }
  if (count < 0) {
    *status = HAL_PARAMETER_OUT_OF_RANGE;
    return -1;
  }

  std::scoped_lock lock{port->mutex};
  auto deadline =
      std::chrono::steady_clock::now() +
      std::chrono::duration_cast<std::chrono::steady_clock::duration>(
          std::chrono::duration<double>{port->timeout});
  int32_t total = 0;
  while (total < count) {
    int pollResult = Poll(port->fd, POLLIN, GetPollTimeout(deadline));
    if (pollResult == 0) {
      break;
    }
    if (pollResult < 0) {
      *status = HAL_SERIAL_PORT_ERROR;
      return total == 0 ? -1 : total;
    }

    auto bytesToRead = port->terminationEnabled ? 1 : count - total;
    ssize_t bytesRead = read(port->fd, buffer + total, bytesToRead);
    if (bytesRead < 0) {
      if (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK) {
        continue;
      }
      *status = HAL_SERIAL_PORT_ERROR;
      return total == 0 ? -1 : total;
    }
    if (bytesRead == 0) {
      break;
    }

    total += static_cast<int32_t>(bytesRead);
    if (port->terminationEnabled && buffer[total - 1] == port->terminator) {
      break;
    }
  }
  return total;
}

int32_t HAL_WriteSerial(HAL_SerialPortHandle handle, const char* buffer,
                        int32_t count, int32_t* status) {
  auto port = GetSerialPort(handle, status);
  if (!port) {
    return -1;
  }
  if (!buffer && count != 0) {
    *status = HAL_NULL_PARAMETER;
    return -1;
  }
  if (count < 0) {
    *status = HAL_PARAMETER_OUT_OF_RANGE;
    return -1;
  }

  std::scoped_lock lock{port->mutex};
  auto deadline =
      std::chrono::steady_clock::now() +
      std::chrono::duration_cast<std::chrono::steady_clock::duration>(
          std::chrono::duration<double>{port->timeout});
  int32_t total = 0;
  while (total < count) {
    int pollResult = Poll(port->fd, POLLOUT, GetPollTimeout(deadline));
    if (pollResult == 0) {
      break;
    }
    if (pollResult < 0) {
      *status = HAL_SERIAL_PORT_ERROR;
      return total == 0 ? -1 : total;
    }

    ssize_t bytesWritten = write(port->fd, buffer + total, count - total);
    if (bytesWritten < 0) {
      if (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK) {
        continue;
      }
      *status = HAL_SERIAL_PORT_ERROR;
      return total == 0 ? -1 : total;
    }
    total += static_cast<int32_t>(bytesWritten);
  }

  if (port->flushOnAccess && tcdrain(port->fd) != 0) {
    *status = HAL_SERIAL_PORT_ERROR;
    return total == 0 ? -1 : total;
  }
  return total;
}

void HAL_FlushSerial(HAL_SerialPortHandle handle, int32_t* status) {
  auto port = GetSerialPort(handle, status);
  if (!port) {
    return;
  }

  std::scoped_lock lock{port->mutex};
  if (tcdrain(port->fd) != 0) {
    *status = HAL_SERIAL_PORT_ERROR;
  }
}

void HAL_ClearSerial(HAL_SerialPortHandle handle, int32_t* status) {
  auto port = GetSerialPort(handle, status);
  if (!port) {
    return;
  }

  std::scoped_lock lock{port->mutex};
  if (tcflush(port->fd, TCIOFLUSH) != 0) {
    *status = HAL_SERIAL_PORT_ERROR;
  }
}
}  // extern "C"
