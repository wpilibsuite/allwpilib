#pragma once

#include "hal/Types.h"
#include "wpi/mutex.h"
#include "hal/ChipObject.h"
#include "FRC_FPGA_ChipObject/fpgainterfacecapi/NiFpga.h"

namespace hal {
class InterruptManager {
public:
    static InterruptManager& Get();
    static int32_t Initialize(tSystemInterface* baseSystem);

    NiFpga_IrqContext GetContext() noexcept;
    void ReleaseContext(NiFpga_IrqContext context) noexcept;

    uint32_t WaitForInterrupt(NiFpga_IrqContext context, uint32_t mask, bool ignorePrevious, uint32_t timeoutInMs, int32_t* status);
private:
    InterruptManager() = default;

    wpi::priority_mutex currentMaskMutex;
    uint32_t currentMask;
    NiFpga_Session fpgaSession;
};
}
