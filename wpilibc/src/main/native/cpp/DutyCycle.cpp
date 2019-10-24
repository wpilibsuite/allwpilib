#include "frc/DutyCycle.h"

#include <hal/DutyCycle.h>
#include <hal/HALBase.h>

#include "frc/smartdashboard/SendableBuilder.h"
#include "frc/DigitalSource.h"
#include "frc/WPIErrors.h"


using namespace frc;

DutyCycle::DutyCycle(DigitalSource* source) : m_source{source, NullDeleter<DigitalSource>()} {
  if (m_source == nullptr) {
    wpi_setWPIError(NullParameter);
  } else {
    InitDutyCycle();
  }
}

DutyCycle::DutyCycle(DigitalSource& source) : m_source{&source, NullDeleter<DigitalSource>()} {
  InitDutyCycle();
}

DutyCycle::DutyCycle(std::shared_ptr<DigitalSource> source) : m_source{std::move(source)} {
  if (m_source == nullptr) {
    wpi_setWPIError(NullParameter);
  } else {
    InitDutyCycle();
  }
}

DutyCycle::~DutyCycle() {
  HAL_FreeDutyCycle(m_handle);
}

int DutyCycle::GetFrequency() const {
  int32_t status = 0;
  auto retVal = HAL_GetDutyCycleFrequency(m_handle, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
  return retVal;
}

unsigned int DutyCycle::GetOutputRaw() const {
  int32_t status = 0;
  auto retVal = HAL_GetDutyCycleOutput(m_handle, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
  return retVal;
}



void DutyCycle::InitSendable(SendableBuilder& builder) {
  builder.SetSmartDashboardType("Duty Cycle");
}
