/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "hal/I2C.h"  // NOLINT(include_order)

#ifdef __linux__
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <sys/ioctl.h>
#include <unistd.h>
#endif  // __linux__

#include <wpi/SmallString.h>
#include <wpi/mutex.h>
#include <wpi/raw_ostream.h>

#include "I2CCommon.h"

using namespace hal;

namespace {
class SystemI2CBus : public I2CBus {
 public:
  explicit SystemI2CBus(int32_t bus) : m_busId(bus) {}
  ~SystemI2CBus() { CloseBus(); }

  bool OpenBus(int32_t* status) override;
  void CloseBus() override;

  bool Initialize(int32_t deviceAddress, int32_t* status) override;
  int32_t Transaction(int32_t deviceAddress, const uint8_t* dataToSend,
                      int32_t sendSize, uint8_t* dataReceived,
                      int32_t receiveSize) override;
  int32_t Write(int32_t deviceAddress, const uint8_t* dataToSend,
                int32_t sendSize) override;
  int32_t Read(int32_t deviceAddress, uint8_t* buffer, int32_t count) override;

 private:
  wpi::mutex m_mutex;
  int m_handle = -1;
  int32_t m_busId;
};
}  // namespace

bool SystemI2CBus::OpenBus(int32_t* status) {
#ifdef __linux__
  wpi::SmallString<64> path;
  wpi::raw_svector_ostream os(path);
  os << "/dev/i2c-" << m_busId;

  std::scoped_lock lock(m_mutex);
  if (m_handle >= 0) return true;

  m_handle = open(path.c_str(), O_RDWR);
  if (m_handle < 0) {
    wpi::outs() << "Failed to open i2c system bus " << m_busId << ": "
                << std::strerror(errno) << '\n';
    return false;
  }
  return true;
#else
  wpi::outs() << "No system i2c bus support\n";
  return false;
#endif
}

void SystemI2CBus::CloseBus() {
#ifdef __linux__
  std::scoped_lock lock(m_mutex);
  if (m_handle < 0) return;
  close(m_handle);
  m_handle = -1;
#endif
}

bool SystemI2CBus::Initialize(int32_t deviceAddress, int32_t* status) {
  return true;
}

int32_t SystemI2CBus::Transaction(int32_t deviceAddress,
                                  const uint8_t* dataToSend, int32_t sendSize,
                                  uint8_t* dataReceived, int32_t receiveSize) {
#ifdef __linux__
  struct i2c_msg msgs[2];
  msgs[0].addr = deviceAddress;
  msgs[0].flags = 0;
  msgs[0].len = sendSize;
  msgs[0].buf = const_cast<uint8_t*>(dataToSend);
  msgs[1].addr = deviceAddress;
  msgs[1].flags = I2C_M_RD;
  msgs[1].len = receiveSize;
  msgs[1].buf = dataReceived;

  struct i2c_rdwr_ioctl_data rdwr;
  rdwr.msgs = msgs;
  rdwr.nmsgs = 2;

  std::scoped_lock lock(m_mutex);
  return ioctl(m_handle, I2C_RDWR, &rdwr);
#else
  return -1;
#endif
}

int32_t SystemI2CBus::Write(int32_t deviceAddress, const uint8_t* dataToSend,
                            int32_t sendSize) {
#ifdef __linux__
  struct i2c_msg msg;
  msg.addr = deviceAddress;
  msg.flags = 0;
  msg.len = sendSize;
  msg.buf = const_cast<uint8_t*>(dataToSend);

  struct i2c_rdwr_ioctl_data rdwr;
  rdwr.msgs = &msg;
  rdwr.nmsgs = 1;

  std::scoped_lock lock(m_mutex);
  return ioctl(m_handle, I2C_RDWR, &rdwr);
#else
  return -1;
#endif
}

int32_t SystemI2CBus::Read(int32_t deviceAddress, uint8_t* buffer,
                           int32_t count) {
#ifdef __linux__
  struct i2c_msg msg;
  msg.addr = deviceAddress;
  msg.flags = I2C_M_RD;
  msg.len = count;
  msg.buf = buffer;

  struct i2c_rdwr_ioctl_data rdwr;
  rdwr.msgs = &msg;
  rdwr.nmsgs = 1;

  std::scoped_lock lock(m_mutex);
  return ioctl(m_handle, I2C_RDWR, &rdwr);
#else
  return -1;
#endif
}

std::unique_ptr<I2CBus> hal::GetSystemI2CBus(int32_t bus) {
  return std::make_unique<SystemI2CBus>(bus);
}

const HAL_I2CBus* HAL_GetSystemI2CBus(int32_t bus, void** data) {
  auto cppBus = GetSystemI2CBus(bus);
  if (!cppBus) return nullptr;
  *data = cppBus.release();
  return &cppI2CBusWrapper;
}
