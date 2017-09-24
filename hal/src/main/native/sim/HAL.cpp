/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2017 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HAL/HAL.h"

#include <cstring>

#if defined(WIN32) || defined(_WIN32)
#include <windows.h>
#else
#include <dlfcn.h>
#endif

#include "ErrorsInternal.h"
#include "HAL/DriverStation.h"
#include "HAL/Errors.h"
#include "HAL/handles/HandlesInternal.h"
#include "MockData/RoboRioDataInternal.h"
#include "MockHooksInternal.h"

using namespace hal;

extern "C" {

HAL_PortHandle HAL_GetPort(int32_t channel) {
  // Dont allow a number that wouldn't fit in a uint8_t
  if (channel < 0 || channel >= 255) return HAL_kInvalidHandle;
  return createPortHandle(channel, 1);
}

/**
 * @deprecated Uses module numbers
 */
HAL_PortHandle HAL_GetPortWithModule(int32_t module, int32_t channel) {
  // Dont allow a number that wouldn't fit in a uint8_t
  if (channel < 0 || channel >= 255) return HAL_kInvalidHandle;
  if (module < 0 || module >= 255) return HAL_kInvalidHandle;
  return createPortHandle(channel, module);
}

const char* HAL_GetErrorMessage(int32_t code) {
  switch (code) {
    case 0:
      return "";
    case CTR_RxTimeout:
      return CTR_RxTimeout_MESSAGE;
    case CTR_TxTimeout:
      return CTR_TxTimeout_MESSAGE;
    case CTR_InvalidParamValue:
      return CTR_InvalidParamValue_MESSAGE;
    case CTR_UnexpectedArbId:
      return CTR_UnexpectedArbId_MESSAGE;
    case CTR_TxFailed:
      return CTR_TxFailed_MESSAGE;
    case CTR_SigNotUpdated:
      return CTR_SigNotUpdated_MESSAGE;
    case NiFpga_Status_FifoTimeout:
      return NiFpga_Status_FifoTimeout_MESSAGE;
    case NiFpga_Status_TransferAborted:
      return NiFpga_Status_TransferAborted_MESSAGE;
    case NiFpga_Status_MemoryFull:
      return NiFpga_Status_MemoryFull_MESSAGE;
    case NiFpga_Status_SoftwareFault:
      return NiFpga_Status_SoftwareFault_MESSAGE;
    case NiFpga_Status_InvalidParameter:
      return NiFpga_Status_InvalidParameter_MESSAGE;
    case NiFpga_Status_ResourceNotFound:
      return NiFpga_Status_ResourceNotFound_MESSAGE;
    case NiFpga_Status_ResourceNotInitialized:
      return NiFpga_Status_ResourceNotInitialized_MESSAGE;
    case NiFpga_Status_HardwareFault:
      return NiFpga_Status_HardwareFault_MESSAGE;
    case NiFpga_Status_IrqTimeout:
      return NiFpga_Status_IrqTimeout_MESSAGE;
    case SAMPLE_RATE_TOO_HIGH:
      return SAMPLE_RATE_TOO_HIGH_MESSAGE;
    case VOLTAGE_OUT_OF_RANGE:
      return VOLTAGE_OUT_OF_RANGE_MESSAGE;
    case LOOP_TIMING_ERROR:
      return LOOP_TIMING_ERROR_MESSAGE;
    case SPI_WRITE_NO_MOSI:
      return SPI_WRITE_NO_MOSI_MESSAGE;
    case SPI_READ_NO_MISO:
      return SPI_READ_NO_MISO_MESSAGE;
    case SPI_READ_NO_DATA:
      return SPI_READ_NO_DATA_MESSAGE;
    case INCOMPATIBLE_STATE:
      return INCOMPATIBLE_STATE_MESSAGE;
    case NO_AVAILABLE_RESOURCES:
      return NO_AVAILABLE_RESOURCES_MESSAGE;
    case RESOURCE_IS_ALLOCATED:
      return RESOURCE_IS_ALLOCATED_MESSAGE;
    case RESOURCE_OUT_OF_RANGE:
      return RESOURCE_OUT_OF_RANGE_MESSAGE;
    case HAL_INVALID_ACCUMULATOR_CHANNEL:
      return HAL_INVALID_ACCUMULATOR_CHANNEL_MESSAGE;
    case HAL_HANDLE_ERROR:
      return HAL_HANDLE_ERROR_MESSAGE;
    case NULL_PARAMETER:
      return NULL_PARAMETER_MESSAGE;
    case ANALOG_TRIGGER_LIMIT_ORDER_ERROR:
      return ANALOG_TRIGGER_LIMIT_ORDER_ERROR_MESSAGE;
    case ANALOG_TRIGGER_PULSE_OUTPUT_ERROR:
      return ANALOG_TRIGGER_PULSE_OUTPUT_ERROR_MESSAGE;
    case PARAMETER_OUT_OF_RANGE:
      return PARAMETER_OUT_OF_RANGE_MESSAGE;
    case HAL_COUNTER_NOT_SUPPORTED:
      return HAL_COUNTER_NOT_SUPPORTED_MESSAGE;
    case HAL_ERR_CANSessionMux_InvalidBuffer:
      return ERR_CANSessionMux_InvalidBuffer_MESSAGE;
    case HAL_ERR_CANSessionMux_MessageNotFound:
      return ERR_CANSessionMux_MessageNotFound_MESSAGE;
    case HAL_WARN_CANSessionMux_NoToken:
      return WARN_CANSessionMux_NoToken_MESSAGE;
    case HAL_ERR_CANSessionMux_NotAllowed:
      return ERR_CANSessionMux_NotAllowed_MESSAGE;
    case HAL_ERR_CANSessionMux_NotInitialized:
      return ERR_CANSessionMux_NotInitialized_MESSAGE;
    case VI_ERROR_SYSTEM_ERROR:
      return VI_ERROR_SYSTEM_ERROR_MESSAGE;
    case VI_ERROR_INV_OBJECT:
      return VI_ERROR_INV_OBJECT_MESSAGE;
    case VI_ERROR_RSRC_LOCKED:
      return VI_ERROR_RSRC_LOCKED_MESSAGE;
    case VI_ERROR_RSRC_NFOUND:
      return VI_ERROR_RSRC_NFOUND_MESSAGE;
    case VI_ERROR_INV_RSRC_NAME:
      return VI_ERROR_INV_RSRC_NAME_MESSAGE;
    case VI_ERROR_QUEUE_OVERFLOW:
      return VI_ERROR_QUEUE_OVERFLOW_MESSAGE;
    case VI_ERROR_IO:
      return VI_ERROR_IO_MESSAGE;
    case VI_ERROR_ASRL_PARITY:
      return VI_ERROR_ASRL_PARITY_MESSAGE;
    case VI_ERROR_ASRL_FRAMING:
      return VI_ERROR_ASRL_FRAMING_MESSAGE;
    case VI_ERROR_ASRL_OVERRUN:
      return VI_ERROR_ASRL_OVERRUN_MESSAGE;
    case VI_ERROR_RSRC_BUSY:
      return VI_ERROR_RSRC_BUSY_MESSAGE;
    case VI_ERROR_INV_PARAMETER:
      return VI_ERROR_INV_PARAMETER_MESSAGE;
    case HAL_PWM_SCALE_ERROR:
      return HAL_PWM_SCALE_ERROR_MESSAGE;
    default:
      return "Unknown error status";
  }
}

/**
 * Returns the runtime type of this HAL
 */
HAL_RuntimeType HAL_GetRuntimeType() { return HAL_Mock; }

/**
 * Return the FPGA Version number.
 * For now, expect this to be competition year.
 * @return FPGA Version number.
 */
int32_t HAL_GetFPGAVersion(int32_t* status) {
  return 2018;  // Automatically script this at some point
}

/**
 * Return the FPGA Revision number.
 * The format of the revision is 3 numbers.
 * The 12 most significant bits are the Major Revision.
 * the next 8 bits are the Minor Revision.
 * The 12 least significant bits are the Build Number.
 * @return FPGA Revision number.
 */
int64_t HAL_GetFPGARevision(int32_t* status) {
  return 0;  // TODO: Find a better number to return;
}

/**
 * Read the microsecond-resolution timer on the FPGA.
 *
 * @return The current time in microseconds according to the FPGA (since FPGA
 * reset).
 */
uint64_t HAL_GetFPGATime(int32_t* status) { return hal::GetFPGATime(); }

/**
 * Get the state of the "USER" button on the roboRIO
 * @return true if the button is currently pressed down
 */
HAL_Bool HAL_GetFPGAButton(int32_t* status) {
  return SimRoboRioData[0].GetFPGAButton();
}

HAL_Bool HAL_GetSystemActive(int32_t* status) {
  return true;  // Figure out if we need to handle this
}

HAL_Bool HAL_GetBrownedOut(int32_t* status) {
  return false;  // Figure out if we need to detect a brownout condition
}

/* A return < 0 indicates an error that should stop the HAL.
   0 is success, and > 0 is a non fatal error */
typedef int halsim_init_func_t(void);

int HAL_LoadExtraSimulation(const char* library) {
  int rc = 1;  // It is expected and reasonable not to find an extra simulation
#if defined(WIN32) || defined(_WIN32)
  HMODULE handle = LoadLibrary(library);
  if (!handle) {
    std::string library_name = std::string(library) + ".dll";
    handle = LoadLibrary(library_name.c_str());
  }
#else
  void* handle = dlopen(library, RTLD_LAZY);
  if (!handle) {
    std::string library_name = "lib" + std::string(library) + ".so";
    handle = dlopen(library_name.c_str(), RTLD_LAZY);
  }
#endif
  if (!handle) return rc;

  halsim_init_func_t* init;
#if defined(WIN32) || defined(_WIN32)
  init = reinterpret_cast<halsim_init_func_t*>(GetProcAddress(handle, "init"));
#else
  init = reinterpret_cast<halsim_init_func_t*>(dlsym(handle, "init"));
#endif

  if (init) {
    rc = (*init)();
  }
  if (rc != 0) {
#if defined(WIN32) || defined(_WIN32)
    FreeLibrary(handle);
#else
    dlclose(handle);
#endif
  }
  return rc;
}

/* Load any extra halsim libraries provided in the HALSIM_EXTRA_LIBRARIES
   environment variable. */
int HAL_LoadExtraSimulations(void) {
#if defined(WIN32) || defined(_WIN32)
  const char delim = ';';
#else
  const char delim = ':';
#endif
  int rc = 1;
  for (char* p = std::getenv("HALSIM_EXTRA_LIBRARIES"); p;) {
    char* q = std::strchr(p, delim);
    std::string lib(p, q ? q - p : std::strlen(p));
    rc = HAL_LoadExtraSimulation(lib.c_str());
    if (rc < 0) break;
    p = q ? q + 1 : q;
  }
  return rc;
}

HAL_Bool HAL_Initialize(int32_t timeout, int32_t mode) {
  if (HAL_LoadExtraSimulations() < 0) return false;
  hal::RestartTiming();
  HAL_InitializeDriverStation();
  return true;  // Add initialization if we need to at a later point
}

int64_t HAL_Report(int32_t resource, int32_t instanceNumber, int32_t context,
                   const char* feature) {
  return 0;  // Do nothing for now
}

}  // extern "C"
