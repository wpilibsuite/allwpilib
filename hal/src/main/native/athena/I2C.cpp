/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "DigitalInternal.h"
#include "HALInitializer.h"
#include "I2CInternal.h"
#include "hal/DIO.h"
#include "hal/HAL.h"
#include "hal/I2C.h"

using namespace hal;

static HAL_Bool (*OpenMXPI2CBusSystem)(void*, int32_t, int32_t*);
static void (*CloseMXPI2CBusSystem)(void*, int32_t, int32_t*);
static HAL_DigitalHandle i2CMXPDigitalHandle1{HAL_kInvalidHandle};
static HAL_DigitalHandle i2CMXPDigitalHandle2{HAL_kInvalidHandle};

static HAL_Bool OpenMXPI2CBus(void* data, int32_t bus, int32_t* status) {
  initializeDigital(status);
  if (*status != 0) return false;

  if ((i2CMXPDigitalHandle1 = HAL_InitializeDIOPort(
           HAL_GetPort(24), false, status)) == HAL_kInvalidHandle) {
    return false;
  }
  if ((i2CMXPDigitalHandle2 = HAL_InitializeDIOPort(
           HAL_GetPort(25), false, status)) == HAL_kInvalidHandle) {
    HAL_FreeDIOPort(i2CMXPDigitalHandle1);  // free the first port allocated
    return false;
  }
  digitalSystem->writeEnableMXPSpecialFunction(
      digitalSystem->readEnableMXPSpecialFunction(status) | 0xC000, status);

  return OpenMXPI2CBusSystem(data, bus, status);
}

static void CloseMXPI2CBus(void* data, int32_t bus, int32_t* status) {
  CloseMXPI2CBusSystem(data, bus, status);
  HAL_FreeDIOPort(i2CMXPDigitalHandle1);
  HAL_FreeDIOPort(i2CMXPDigitalHandle2);
}

namespace hal {
namespace init {
void InitializeI2C() {
  void* data;

  // onboard
  {
    auto& handlers = GetSystemI2CBusHandlers(HAL_I2C_RIO_kOnboard, &data);
    RegisterI2CBus(HAL_I2C_RIO_kOnboard, data, handlers);
  }

  // for MXP, need to do a couple other things when opening
  {
    auto handlers = GetSystemI2CBusHandlers(HAL_I2C_RIO_kMXP, &data);
    OpenMXPI2CBusSystem = handlers.OpenBus;
    CloseMXPI2CBusSystem = handlers.CloseBus;
    handlers.OpenBus = OpenMXPI2CBus;
    handlers.CloseBus = CloseMXPI2CBus;
    RegisterI2CBus(HAL_I2C_RIO_kMXP, data, handlers);
  }
}
}  // namespace init
}  // namespace hal
