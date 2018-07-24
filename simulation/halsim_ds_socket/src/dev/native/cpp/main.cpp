/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <thread>

#include <hal/HAL.h>
#include <wpi/Format.h>
#include <wpi/raw_ostream.h>

extern "C" int HALSIM_InitExtension(void);

int main() {
  HAL_Initialize(500, 0);
  HALSIM_InitExtension();

  HAL_ObserveUserProgramStarting();

  HAL_JoystickAxes axes;
  HAL_JoystickButtons buttons;
  HAL_JoystickPOVs povs;
  int32_t status = 0;

  while (true) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    HAL_GetJoystickAxes(0, &axes);
    HAL_GetJoystickButtons(0, &buttons);
    HAL_GetJoystickPOVs(0, &povs);

    wpi::outs() << wpi::format("%1.5f", axes.axes[0]) << " " << buttons.buttons << " " << (int)povs.povs[0] << "\n";
    wpi::outs().flush();
    //HAL_SetJoystickOutputs(1, 0, 0, (int)((axes.axes[1] + 1.0f) * 65535 / 2));
  }
}
