/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <thread>

#include <hal/HAL.h>
#include <wpi/raw_ostream.h>
#include <wpi/Format.h>

extern "C" int HALSIM_InitExtension(void);

int main() {
  HAL_Initialize(500, 0);
  HALSIM_InitExtension();

  HAL_ObserveUserProgramStarting();

  HAL_JoystickAxes axes;
  int32_t status = 0;

  while (true) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    HAL_GetJoystickAxes(0, &axes);
    HAL_SetJoystickOutputs(0, 0, 0, (int)((axes.axes[1] + 1.0f) * 65535 / 2));

    wpi::outs() << wpi::format("%4.5f", HAL_GetMatchTime(&status)) << "\n";
    wpi::outs().flush();
  }
}
