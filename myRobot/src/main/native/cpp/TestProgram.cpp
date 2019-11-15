#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#pragma GCC diagnostic ignored "-Wignored-qualifiers"

#include "FRC_FPGA_ChipObject/nRoboRIO_FPGANamespace/tGlobal.h"
#include "FRC_FPGA_ChipObject/nRoboRIO_FPGANamespace/tSysWatchdog.h"
#include "FRC_FPGA_ChipObject/nRoboRIO_FPGANamespace/tLED.h"
#include "FRC_FPGA_ChipObject/nRoboRIO_FPGANamespace/nInterfaceGlobals.h"
#include "FRC_NetworkCommunication/LoadOut.h"

#pragma GCC diagnostic pop

#include <iostream>

extern "C" {
/**
 * Open a host memory buffer.
 *
 * @param session [in] handle to a currently open session
 * @param memoryName [in] name of the HMB memory configured in the project
 * @param virtualAddress [out] virtual address that the host will use to access the memory
 * @param size [out] size in bytes of the memory area
 * @return result of the call
 */
NiFpga_Status NiFpga_OpenHostMemoryBuffer(NiFpga_Session session,
                                          const char* memoryName,
                                          void** virtualAddress,
                                          size_t* size);

void NumericArrayResize(void) {}
void RTSetCleanupProc(void) {}
void EDVR_CreateReference(void) {}



}

using namespace nFPGA::nFRC_2020_20_1_0;
int main() {
    g_currentTargetClass =
      nLoadOut::kTargetClass_RoboRIO;

    int32_t status = 0;
    tGlobal* global = tGlobal::create(&status);
    std::cout << "Global: " << status << std::endl;
    tSysWatchdog* watchdog = tSysWatchdog::create(&status);
    std::cout << "Watchdog: " << status << std::endl;
    (void)global;
    (void)watchdog;

    tLED* led = tLED::create(&status);
    (void)led;

    std::cout << "LED: " << status << std::endl;

      uint32_t session = global->getSystemInterface()->getHandle();
    std::cout << "Session Handle: " << session << std::endl;

      uint32_t* data = nullptr;
      size_t size = 0;
      status = NiFpga_OpenHostMemoryBuffer(session, "HMB_0_LED", reinterpret_cast<void**>(&data), &size);

      std::cout << "HMB Open: "  << status << std::endl;
}
