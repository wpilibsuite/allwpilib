#include "frc/counters/ExternalDirectionCounter.h"

#include <hal/Counter.h>

#include "frc/Base.h"
#include "frc/DigitalSource.h"
#include "frc/WPIErrors.h"
#include "frc/smartdashboard/SendableBuilder.h"

using namespace frc;

ExternalDirectionCounter::ExternalDirectionCounter(DigitalSource* countSource, DigitalSource* directionSource)
    : m_countSource{countSource, NullDeleter<DigitalSource>()}, m_directionSource{directionSource, NullDeleter<DigitalSource>()} {
    if (countSource == nullptr || directionSource == nullptr) {
        wpi_setWPIError(NullParameter);
    } else {
        InitExternalDirectionCounter();
    }
}

ExternalDirectionCounter::~ExternalDirectionCounter() {
    int32_t status = 0;
    HAL_FreeCounter(m_handle, &status);
}

void ExternalDirectionCounter::InitExternalDirectionCounter() {
    int32_t status = 0;
    int32_t index = 0;
    m_handle = HAL_InitializeCounter(HAL_Counter_Mode::HAL_Counter_kExternalDirection, &index, &status);

    wpi_setHALError(status);
    if (status != 0) {
        return;
    }

    HAL_SetCounterUpSource(m_handle, m_countSource->GetPortHandleForRouting(), (HAL_AnalogTriggerType)m_countSource->GetAnalogTriggerTypeForRouting(), &status);
    wpi_setHALError(status);

    HAL_SetCounterUpSourceEdge(m_handle, true, false, &status);
    wpi_setHALError(status);

    HAL_SetCounterDownSource(m_handle, m_directionSource->GetPortHandleForRouting(), (HAL_AnalogTriggerType)m_directionSource->GetAnalogTriggerTypeForRouting(), &status);
    wpi_setHALError(status);

    HAL_SetCounterDownSourceEdge(m_handle, false, true, &status);
    wpi_setHALError(status);

    HAL_ResetCounter(m_handle, &status);
    wpi_setHALError(status);

    // TODO Usage reporting

    SendableRegistry::GetInstance().AddLW(this, "External Direction Counter", index);
}

int ExternalDirectionCounter::GetCount() const {
    int32_t status = 0;
    int val = HAL_GetCounter(m_handle, &status);
    wpi_setHALError(status);
    return val;
}
