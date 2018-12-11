/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "hal/SPI.h"

#include <fcntl.h>
#include <linux/spi/spidev.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <array>
#include <atomic>
#include <cstring>

#include <wpi/mutex.h>
#include <wpi/raw_ostream.h>

#include "DigitalInternal.h"
#include "HALInitializer.h"
#include "hal/DIO.h"
#include "hal/HAL.h"
#include "hal/handles/HandlesInternal.h"

using namespace hal;

static int32_t m_spiCS0Handle{0};
static int32_t m_spiCS1Handle{0};
static int32_t m_spiCS2Handle{0};
static int32_t m_spiCS3Handle{0};
static int32_t m_spiMXPHandle{0};

static constexpr int32_t kSpiMaxHandles = 5;

// Indices 0-3 are for onboard CS0-CS2. Index 4 is for MXP.
static std::array<wpi::mutex, kSpiMaxHandles> spiHandleMutexes;
static std::array<wpi::mutex, kSpiMaxHandles> spiApiMutexes;
static std::array<wpi::mutex, kSpiMaxHandles> spiAccumulatorMutexes;

// MXP SPI does not count towards this
static std::atomic<int32_t> spiPortCount{0};

static HAL_DigitalHandle digitalHandles[9]{HAL_kInvalidHandle};

static wpi::mutex spiAutoMutex;
static int32_t spiAutoPort = kSpiMaxHandles;
static std::atomic_bool spiAutoRunning{false};
static std::unique_ptr<tDMAManager> spiAutoDMA;

static bool SPIInUseByAuto(HAL_SPIPort port) {
  // SPI engine conflicts with any other chip selects on the same SPI device.
  // There are two SPI devices: one for ports 0-3 (onboard), the other for port
  // 4 (MXP).
  if (!spiAutoRunning) return false;
  std::lock_guard<wpi::mutex> lock(spiAutoMutex);
  return (spiAutoPort >= 0 && spiAutoPort <= 3 && port >= 0 && port <= 3) ||
         (spiAutoPort == 4 && port == 4);
}

namespace hal {
namespace init {
void InitializeSPI() {}
}  // namespace init
}  // namespace hal

extern "C" {

static void CommonSPIPortInit(int32_t* status) {
  // All false cases will set
  if (spiPortCount.fetch_add(1) == 0) {
    // Have not been initialized yet
    initializeDigital(status);
    if (*status != 0) return;
    // MISO
    if ((digitalHandles[3] = HAL_InitializeDIOPort(createPortHandleForSPI(29),
                                                   false, status)) ==
        HAL_kInvalidHandle) {
      std::printf("Failed to allocate DIO 29 (MISO)\n");
      return;
    }
    // MOSI
    if ((digitalHandles[4] = HAL_InitializeDIOPort(createPortHandleForSPI(30),
                                                   false, status)) ==
        HAL_kInvalidHandle) {
      std::printf("Failed to allocate DIO 30 (MOSI)\n");
      HAL_FreeDIOPort(digitalHandles[3]);  // free the first port allocated
      return;
    }
  }
}

static void CommonSPIPortFree(void) {
  if (spiPortCount.fetch_sub(1) == 1) {
    // Clean up SPI Handles
    HAL_FreeDIOPort(digitalHandles[3]);
    HAL_FreeDIOPort(digitalHandles[4]);
  }
}

void HAL_InitializeSPI(HAL_SPIPort port, int32_t* status) {
  hal::init::CheckInit();
  if (port < 0 || port >= kSpiMaxHandles) {
    *status = PARAMETER_OUT_OF_RANGE;
    return;
  }

  int handle;
  if (HAL_GetSPIHandle(port) != 0) return;
  switch (port) {
    case HAL_SPI_kOnboardCS0:
      CommonSPIPortInit(status);
      if (*status != 0) return;
      // CS0 is not a DIO port, so nothing to allocate
      handle = open("/dev/spidev0.0", O_RDWR);
      if (handle < 0) {
        std::printf("Failed to open SPI port %d: %s\n", port,
                    std::strerror(errno));
        CommonSPIPortFree();
        return;
      }
      HAL_SetSPIHandle(HAL_SPI_kOnboardCS0, handle);
      break;
    case HAL_SPI_kOnboardCS1:
      CommonSPIPortInit(status);
      if (*status != 0) return;
      // CS1, Allocate
      if ((digitalHandles[0] = HAL_InitializeDIOPort(createPortHandleForSPI(26),
                                                     false, status)) ==
          HAL_kInvalidHandle) {
        std::printf("Failed to allocate DIO 26 (CS1)\n");
        CommonSPIPortFree();
        return;
      }
      handle = open("/dev/spidev0.1", O_RDWR);
      if (handle < 0) {
        std::printf("Failed to open SPI port %d: %s\n", port,
                    std::strerror(errno));
        CommonSPIPortFree();
        HAL_FreeDIOPort(digitalHandles[0]);
        return;
      }
      HAL_SetSPIHandle(HAL_SPI_kOnboardCS1, handle);
      break;
    case HAL_SPI_kOnboardCS2:
      CommonSPIPortInit(status);
      if (*status != 0) return;
      // CS2, Allocate
      if ((digitalHandles[1] = HAL_InitializeDIOPort(createPortHandleForSPI(27),
                                                     false, status)) ==
          HAL_kInvalidHandle) {
        std::printf("Failed to allocate DIO 27 (CS2)\n");
        CommonSPIPortFree();
        return;
      }
      handle = open("/dev/spidev0.2", O_RDWR);
      if (handle < 0) {
        std::printf("Failed to open SPI port %d: %s\n", port,
                    std::strerror(errno));
        CommonSPIPortFree();
        HAL_FreeDIOPort(digitalHandles[1]);
        return;
      }
      HAL_SetSPIHandle(HAL_SPI_kOnboardCS2, handle);
      break;
    case HAL_SPI_kOnboardCS3:
      CommonSPIPortInit(status);
      if (*status != 0) return;
      // CS3, Allocate
      if ((digitalHandles[2] = HAL_InitializeDIOPort(createPortHandleForSPI(28),
                                                     false, status)) ==
          HAL_kInvalidHandle) {
        std::printf("Failed to allocate DIO 28 (CS3)\n");
        CommonSPIPortFree();
        return;
      }
      handle = open("/dev/spidev0.3", O_RDWR);
      if (handle < 0) {
        std::printf("Failed to open SPI port %d: %s\n", port,
                    std::strerror(errno));
        CommonSPIPortFree();
        HAL_FreeDIOPort(digitalHandles[2]);
        return;
      }
      HAL_SetSPIHandle(HAL_SPI_kOnboardCS3, handle);
      break;
    case HAL_SPI_kMXP:
      initializeDigital(status);
      if (*status != 0) return;
      if ((digitalHandles[5] = HAL_InitializeDIOPort(createPortHandleForSPI(14),
                                                     false, status)) ==
          HAL_kInvalidHandle) {
        wpi::outs() << "Failed to allocate DIO 14\n";
        return;
      }
      if ((digitalHandles[6] = HAL_InitializeDIOPort(createPortHandleForSPI(15),
                                                     false, status)) ==
          HAL_kInvalidHandle) {
        wpi::outs() << "Failed to allocate DIO 15\n";
        HAL_FreeDIOPort(digitalHandles[5]);  // free the first port allocated
        return;
      }
      if ((digitalHandles[7] = HAL_InitializeDIOPort(createPortHandleForSPI(16),
                                                     false, status)) ==
          HAL_kInvalidHandle) {
        wpi::outs() << "Failed to allocate DIO 16\n";
        HAL_FreeDIOPort(digitalHandles[5]);  // free the first port allocated
        HAL_FreeDIOPort(digitalHandles[6]);  // free the second port allocated
        return;
      }
      if ((digitalHandles[8] = HAL_InitializeDIOPort(createPortHandleForSPI(17),
                                                     false, status)) ==
          HAL_kInvalidHandle) {
        wpi::outs() << "Failed to allocate DIO 17\n";
        HAL_FreeDIOPort(digitalHandles[5]);  // free the first port allocated
        HAL_FreeDIOPort(digitalHandles[6]);  // free the second port allocated
        HAL_FreeDIOPort(digitalHandles[7]);  // free the third port allocated
        return;
      }
      digitalSystem->writeEnableMXPSpecialFunction(
          digitalSystem->readEnableMXPSpecialFunction(status) | 0x00F0, status);
      handle = open("/dev/spidev1.0", O_RDWR);
      if (handle < 0) {
        std::printf("Failed to open SPI port %d: %s\n", port,
                    std::strerror(errno));
        HAL_FreeDIOPort(digitalHandles[5]);  // free the first port allocated
        HAL_FreeDIOPort(digitalHandles[6]);  // free the second port allocated
        HAL_FreeDIOPort(digitalHandles[7]);  // free the third port allocated
        HAL_FreeDIOPort(digitalHandles[8]);  // free the fourth port allocated
        return;
      }
      HAL_SetSPIHandle(HAL_SPI_kMXP, handle);
      break;
    default:
      *status = PARAMETER_OUT_OF_RANGE;
      break;
  }
}

int32_t HAL_TransactionSPI(HAL_SPIPort port, const uint8_t* dataToSend,
                           uint8_t* dataReceived, int32_t size) {
  if (port < 0 || port >= kSpiMaxHandles) {
    return -1;
  }

  if (SPIInUseByAuto(port)) return -1;

  struct spi_ioc_transfer xfer;
  std::memset(&xfer, 0, sizeof(xfer));
  xfer.tx_buf = (__u64)dataToSend;
  xfer.rx_buf = (__u64)dataReceived;
  xfer.len = size;

  std::lock_guard<wpi::mutex> lock(spiApiMutexes[port]);
  return ioctl(HAL_GetSPIHandle(port), SPI_IOC_MESSAGE(1), &xfer);
}

int32_t HAL_WriteSPI(HAL_SPIPort port, const uint8_t* dataToSend,
                     int32_t sendSize) {
  if (port < 0 || port >= kSpiMaxHandles) {
    return -1;
  }

  if (SPIInUseByAuto(port)) return -1;

  struct spi_ioc_transfer xfer;
  std::memset(&xfer, 0, sizeof(xfer));
  xfer.tx_buf = (__u64)dataToSend;
  xfer.len = sendSize;

  std::lock_guard<wpi::mutex> lock(spiApiMutexes[port]);
  return ioctl(HAL_GetSPIHandle(port), SPI_IOC_MESSAGE(1), &xfer);
}

int32_t HAL_ReadSPI(HAL_SPIPort port, uint8_t* buffer, int32_t count) {
  if (port < 0 || port >= kSpiMaxHandles) {
    return -1;
  }

  if (SPIInUseByAuto(port)) return -1;

  struct spi_ioc_transfer xfer;
  std::memset(&xfer, 0, sizeof(xfer));
  xfer.rx_buf = (__u64)buffer;
  xfer.len = count;

  std::lock_guard<wpi::mutex> lock(spiApiMutexes[port]);
  return ioctl(HAL_GetSPIHandle(port), SPI_IOC_MESSAGE(1), &xfer);
}

void HAL_CloseSPI(HAL_SPIPort port) {
  if (port < 0 || port >= kSpiMaxHandles) {
    return;
  }

  int32_t status = 0;
  HAL_FreeSPIAuto(port, &status);

  {
    std::lock_guard<wpi::mutex> lock(spiApiMutexes[port]);
    close(HAL_GetSPIHandle(port));
  }

  HAL_SetSPIHandle(port, 0);
  if (port < 4) {
    CommonSPIPortFree();
  }

  switch (port) {
    // Case 0 does not need to do anything
    case 1:
      HAL_FreeDIOPort(digitalHandles[0]);
      break;
    case 2:
      HAL_FreeDIOPort(digitalHandles[1]);
      break;
    case 3:
      HAL_FreeDIOPort(digitalHandles[2]);
      break;
    case 4:
      HAL_FreeDIOPort(digitalHandles[5]);
      HAL_FreeDIOPort(digitalHandles[6]);
      HAL_FreeDIOPort(digitalHandles[7]);
      HAL_FreeDIOPort(digitalHandles[8]);
      break;
    default:
      break;
  }
}

void HAL_SetSPISpeed(HAL_SPIPort port, int32_t speed) {
  if (port < 0 || port >= kSpiMaxHandles) {
    return;
  }

  std::lock_guard<wpi::mutex> lock(spiApiMutexes[port]);
  ioctl(HAL_GetSPIHandle(port), SPI_IOC_WR_MAX_SPEED_HZ, &speed);
}

void HAL_SetSPIOpts(HAL_SPIPort port, HAL_Bool msbFirst,
                    HAL_Bool sampleOnTrailing, HAL_Bool clkIdleHigh) {
  if (port < 0 || port >= kSpiMaxHandles) {
    return;
  }

  uint8_t mode = 0;
  mode |= (!msbFirst ? 8 : 0);
  mode |= (clkIdleHigh ? 2 : 0);
  mode |= (sampleOnTrailing ? 1 : 0);

  std::lock_guard<wpi::mutex> lock(spiApiMutexes[port]);
  ioctl(HAL_GetSPIHandle(port), SPI_IOC_WR_MODE, &mode);
}

void HAL_SetSPIChipSelectActiveHigh(HAL_SPIPort port, int32_t* status) {
  if (port < 0 || port >= kSpiMaxHandles) {
    *status = PARAMETER_OUT_OF_RANGE;
    return;
  }

  std::lock_guard<wpi::mutex> lock(spiApiMutexes[port]);
  if (port < 4) {
    spiSystem->writeChipSelectActiveHigh_Hdr(
        spiSystem->readChipSelectActiveHigh_Hdr(status) | (1 << port), status);
  } else {
    spiSystem->writeChipSelectActiveHigh_MXP(1, status);
  }
}

void HAL_SetSPIChipSelectActiveLow(HAL_SPIPort port, int32_t* status) {
  if (port < 0 || port >= kSpiMaxHandles) {
    *status = PARAMETER_OUT_OF_RANGE;
    return;
  }

  std::lock_guard<wpi::mutex> lock(spiApiMutexes[port]);
  if (port < 4) {
    spiSystem->writeChipSelectActiveHigh_Hdr(
        spiSystem->readChipSelectActiveHigh_Hdr(status) & ~(1 << port), status);
  } else {
    spiSystem->writeChipSelectActiveHigh_MXP(0, status);
  }
}

int32_t HAL_GetSPIHandle(HAL_SPIPort port) {
  if (port < 0 || port >= kSpiMaxHandles) {
    return 0;
  }

  std::lock_guard<wpi::mutex> lock(spiHandleMutexes[port]);
  switch (port) {
    case 0:
      return m_spiCS0Handle;
    case 1:
      return m_spiCS1Handle;
    case 2:
      return m_spiCS2Handle;
    case 3:
      return m_spiCS3Handle;
    case 4:
      return m_spiMXPHandle;
    default:
      return 0;
  }
}

void HAL_SetSPIHandle(HAL_SPIPort port, int32_t handle) {
  if (port < 0 || port >= kSpiMaxHandles) {
    return;
  }

  std::lock_guard<wpi::mutex> lock(spiHandleMutexes[port]);
  switch (port) {
    case 0:
      m_spiCS0Handle = handle;
      break;
    case 1:
      m_spiCS1Handle = handle;
      break;
    case 2:
      m_spiCS2Handle = handle;
      break;
    case 3:
      m_spiCS3Handle = handle;
      break;
    case 4:
      m_spiMXPHandle = handle;
      break;
    default:
      break;
  }
}

void HAL_InitSPIAuto(HAL_SPIPort port, int32_t bufferSize, int32_t* status) {
  if (port < 0 || port >= kSpiMaxHandles) {
    *status = PARAMETER_OUT_OF_RANGE;
    return;
  }

  std::lock_guard<wpi::mutex> lock(spiAutoMutex);
  // FPGA only has one auto SPI engine
  if (spiAutoPort != kSpiMaxHandles) {
    *status = RESOURCE_IS_ALLOCATED;
    return;
  }

  // remember the initialized port for other entry points
  spiAutoPort = port;

  // configure the correct chip select
  if (port < 4) {
    spiSystem->writeAutoSPI1Select(false, status);
    spiSystem->writeAutoChipSelect(port, status);
  } else {
    spiSystem->writeAutoSPI1Select(true, status);
    spiSystem->writeAutoChipSelect(0, status);
  }

  // configure DMA
  tDMAChannelDescriptor desc;
  spiSystem->getSystemInterface()->getDmaDescriptor(g_SpiAutoData_index, &desc);
  spiAutoDMA = std::make_unique<tDMAManager>(desc.channel, bufferSize, status);
}

void HAL_FreeSPIAuto(HAL_SPIPort port, int32_t* status) {
  if (port < 0 || port >= kSpiMaxHandles) {
    *status = PARAMETER_OUT_OF_RANGE;
    return;
  }

  std::lock_guard<wpi::mutex> lock(spiAutoMutex);
  if (spiAutoPort != port) return;
  spiAutoPort = kSpiMaxHandles;

  // disable by setting to internal clock and setting rate=0
  spiSystem->writeAutoRate(0, status);
  spiSystem->writeAutoTriggerConfig_ExternalClock(false, status);

  // stop the DMA
  spiAutoDMA->stop(status);

  spiAutoDMA.reset(nullptr);

  spiAutoRunning = false;
}

void HAL_StartSPIAutoRate(HAL_SPIPort port, double period, int32_t* status) {
  std::lock_guard<wpi::mutex> lock(spiAutoMutex);
  // FPGA only has one auto SPI engine
  if (port != spiAutoPort) {
    *status = INCOMPATIBLE_STATE;
    return;
  }

  spiAutoRunning = true;

  // start the DMA
  spiAutoDMA->start(status);

  // auto rate is in microseconds
  spiSystem->writeAutoRate(period * 1000000, status);

  // disable the external clock
  spiSystem->writeAutoTriggerConfig_ExternalClock(false, status);
}

void HAL_StartSPIAutoTrigger(HAL_SPIPort port, HAL_Handle digitalSourceHandle,
                             HAL_AnalogTriggerType analogTriggerType,
                             HAL_Bool triggerRising, HAL_Bool triggerFalling,
                             int32_t* status) {
  std::lock_guard<wpi::mutex> lock(spiAutoMutex);
  // FPGA only has one auto SPI engine
  if (port != spiAutoPort) {
    *status = INCOMPATIBLE_STATE;
    return;
  }

  spiAutoRunning = true;

  // start the DMA
  spiAutoDMA->start(status);

  // get channel routing
  bool routingAnalogTrigger = false;
  uint8_t routingChannel = 0;
  uint8_t routingModule = 0;
  if (!remapDigitalSource(digitalSourceHandle, analogTriggerType,
                          routingChannel, routingModule,
                          routingAnalogTrigger)) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  // configure external trigger and enable it
  tSPI::tAutoTriggerConfig config;
  config.ExternalClock = 1;
  config.FallingEdge = triggerFalling ? 1 : 0;
  config.RisingEdge = triggerRising ? 1 : 0;
  config.ExternalClockSource_AnalogTrigger = routingAnalogTrigger ? 1 : 0;
  config.ExternalClockSource_Module = routingModule;
  config.ExternalClockSource_Channel = routingChannel;
  spiSystem->writeAutoTriggerConfig(config, status);
}

void HAL_StopSPIAuto(HAL_SPIPort port, int32_t* status) {
  std::lock_guard<wpi::mutex> lock(spiAutoMutex);
  // FPGA only has one auto SPI engine
  if (port != spiAutoPort) {
    *status = INCOMPATIBLE_STATE;
    return;
  }

  // disable by setting to internal clock and setting rate=0
  spiSystem->writeAutoRate(0, status);
  spiSystem->writeAutoTriggerConfig_ExternalClock(false, status);

  // stop the DMA
  spiAutoDMA->stop(status);

  spiAutoRunning = false;
}

void HAL_SetSPIAutoTransmitData(HAL_SPIPort port, const uint8_t* dataToSend,
                                int32_t dataSize, int32_t zeroSize,
                                int32_t* status) {
  if (dataSize < 0 || dataSize > 16) {
    *status = PARAMETER_OUT_OF_RANGE;
    return;
  }

  if (zeroSize < 0 || zeroSize > 127) {
    *status = PARAMETER_OUT_OF_RANGE;
    return;
  }

  std::lock_guard<wpi::mutex> lock(spiAutoMutex);
  // FPGA only has one auto SPI engine
  if (port != spiAutoPort) {
    *status = INCOMPATIBLE_STATE;
    return;
  }

  // set tx data registers
  for (int32_t i = 0; i < dataSize; ++i)
    spiSystem->writeAutoTx(i >> 2, i & 3, dataToSend[i], status);

  // set byte counts
  tSPI::tAutoByteCount config;
  config.ZeroByteCount = static_cast<unsigned>(zeroSize) & 0x7f;
  config.TxByteCount = static_cast<unsigned>(dataSize) & 0xf;
  spiSystem->writeAutoByteCount(config, status);
}

void HAL_ForceSPIAutoRead(HAL_SPIPort port, int32_t* status) {
  std::lock_guard<wpi::mutex> lock(spiAutoMutex);
  // FPGA only has one auto SPI engine
  if (port != spiAutoPort) {
    *status = INCOMPATIBLE_STATE;
    return;
  }

  spiSystem->strobeAutoForceOne(status);
}

int32_t HAL_ReadSPIAutoReceivedData(HAL_SPIPort port, uint32_t* buffer,
                                    int32_t numToRead, double timeout,
                                    int32_t* status) {
  std::lock_guard<wpi::mutex> lock(spiAutoMutex);
  // FPGA only has one auto SPI engine
  if (port != spiAutoPort) {
    *status = INCOMPATIBLE_STATE;
    return 0;
  }

  size_t numRemaining = 0;
  // timeout is in ms
  spiAutoDMA->read(buffer, numToRead, timeout * 1000, &numRemaining, status);
  return numRemaining;
}

int32_t HAL_GetSPIAutoDroppedCount(HAL_SPIPort port, int32_t* status) {
  std::lock_guard<wpi::mutex> lock(spiAutoMutex);
  // FPGA only has one auto SPI engine
  if (port != spiAutoPort) {
    *status = INCOMPATIBLE_STATE;
    return 0;
  }

  return spiSystem->readTransferSkippedFullCount(status);
}

}  // extern "C"
