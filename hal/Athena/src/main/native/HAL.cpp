
#include "HAL/HAL.h"

#include "Port.h"
#include "HAL/Errors.h"
#include "ChipObject.h"

// XXX: What to do with solenoids? const uint32_t solenoid_kNumDO7_0Elements = tSolenoid::kNumDO7_0Elements;
const uint32_t dio_kNumSystems = tDIO::kNumSystems;
const uint32_t interrupt_kNumSystems = tInterrupt::kNumSystems;
const uint32_t kSystemClockTicksPerMicrosecond = 40;

void* getPort(uint8_t pin) {
  Port* port = new Port();
  port->pin = pin;
  port->module = 1;
  return port;
}

/**
 * @deprecated Uses module numbers
 */
void* getPortWithModule(uint8_t module, uint8_t pin) {
  Port* port = new Port();
  port->pin = pin;
  port->module = module;
  return port;
}

const char* getHALErrorMessage(int32_t code) {
  if (code == 0) return "";
  else if (code == SAMPLE_RATE_TOO_HIGH) return SAMPLE_RATE_TOO_HIGH_MESSAGE;
  else if (code == VOLTAGE_OUT_OF_RANGE) return VOLTAGE_OUT_OF_RANGE_MESSAGE;
  else if (code == LOOP_TIMING_ERROR) return LOOP_TIMING_ERROR_MESSAGE;
  else if (code == SPI_WRITE_NO_MOSI) return SPI_WRITE_NO_MOSI_MESSAGE;
  else if (code == SPI_READ_NO_MISO) return SPI_READ_NO_MISO_MESSAGE;
  else if (code == SPI_READ_NO_DATA) return SPI_READ_NO_DATA_MESSAGE;
  else if (code == INCOMPATIBLE_STATE) return INCOMPATIBLE_STATE_MESSAGE;
  else if (code == NO_AVAILABLE_RESOURCES) return NO_AVAILABLE_RESOURCES_MESSAGE;
  else if (code == NULL_PARAMETER) return NULL_PARAMETER_MESSAGE;
  else if (code == ANALOG_TRIGGER_LIMIT_ORDER_ERROR) return ANALOG_TRIGGER_LIMIT_ORDER_ERROR_MESSAGE;
  else if (code == ANALOG_TRIGGER_PULSE_OUTPUT_ERROR) return ANALOG_TRIGGER_PULSE_OUTPUT_ERROR_MESSAGE;
  else if (code == PARAMETER_OUT_OF_RANGE) return PARAMETER_OUT_OF_RANGE_MESSAGE;
  else return "";
}

/**
 * Return the FPGA Version number.
 * For now, expect this to be competition year.
 * @return FPGA Version number.
 */
uint16_t getFPGAVersion(int32_t *status) {
  tGlobal *global = tGlobal::create(status);
  uint16_t version = global->readVersion(status);
  delete global;
  return version;
}

/**
 * Return the FPGA Revision number.
 * The format of the revision is 3 numbers.
 * The 12 most significant bits are the Major Revision.
 * the next 8 bits are the Minor Revision.
 * The 12 least significant bits are the Build Number.
 * @return FPGA Revision number.
 */
uint32_t getFPGARevision(int32_t *status) {
  tGlobal *global = tGlobal::create(status);
  uint32_t revision = global->readRevision(status);
  delete global;
  return revision;
}

/**
 * Read the microsecond-resolution timer on the FPGA.
 * 
 * @return The current time in microseconds according to the FPGA (since FPGA reset).
 */
uint32_t getFPGATime(int32_t *status) {
  tGlobal *global = tGlobal::create(status);
  uint32_t time = global->readLocalTime(status);
  delete global;
  return time;
}


/**
 * Set the state of the FPGA status LED on the cRIO.
 */
void setFPGALED(uint32_t state, int32_t *status) {
  // XXX: Not supported?
  // tGlobal *global = tGlobal::create(status);
  // global->writeFPGA_LED(state, status);
  // delete global;
}

/**
 * Get the current state of the FPGA status LED on the cRIO.
 * @return The curent state of the FPGA LED.
 */
int32_t getFPGALED(int32_t *status) {
  // XXX: Not supported?
  // tGlobal *global = tGlobal::create(status);
  // bool ledValue = global->readFPGA_LED(status);
  // delete global;
  // return ledValue;
  return 0; // XXX: Dummy value
}

// TODO: HACKS
void NumericArrayResize() {}
void RTSetCleanupProc() {}
void EDVR_CreateReference() {}
void Occur() {}

void imaqGetErrorText() {}
void imaqGetLastError() {}
void niTimestamp64() {}

#include "NetworkCommunication/LoadOut.h"
namespace nLoadOut {
  bool getModulePresence(tModuleType moduleType, uint8_t moduleNumber) {
  	return true;
  }
}
