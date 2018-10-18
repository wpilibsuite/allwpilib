/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <iostream>
#include <thread>

#include "../../native/windows/WindowsMessagePump.h"

#include "cscore.h"

#pragma comment(lib, "User32.lib")

int main() {
  int32_t status = 0;
  // auto parameter = cs::EnumerateUsbCameras(&status);
  // for (auto&& cam : parameter) {
  //   std::cout << cam.name << " \n" << cam.path << "\n";
  // }

  auto handle = cs::CreateUsbCameraPath("test", "\\\\?\\USB#VID_046D&PID_0825&MI_00#6&900429&0&0000#{65e8773d-8f56-11d0-a3b9-00a0c9223196}\\{bbefb6c7-2fc4-4139-bb8b-a58bba724083}", &status);

  std::this_thread::sleep_for(std::chrono::seconds(1));
  std::cout << (int)handle << "\n";
  system("PAUSE");
  cs::ReleaseSource(handle, &status);
}
