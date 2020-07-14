#include <thread>

#include <hal/DriverStation.h>
#include <hal/HALBase.h>
#include <hal/Main.h>
#include <wpi/Format.h>
#include <wpi/raw_ostream.h>

#include <iostream>

extern "C" int HALSIM_InitExtension(void);

int main() {
  HAL_Initialize(500, 0);
  HALSIM_InitExtension();

  // HAL_ObserveUserProgramStarting();

  HAL_RunMain();

  int cycleCount = 0;
  while (cycleCount < 1000) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    cycleCount++;
    std::cout << "Count: " << cycleCount << std::endl;
  }

  std::cout << "DONE" << std::endl;
  HAL_ExitMain();
}