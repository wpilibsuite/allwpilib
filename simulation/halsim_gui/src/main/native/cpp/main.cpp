/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <hal/Main.h>
#include <wpi/raw_ostream.h>

#include "AccelerometerGui.h"
#include "AddressableLEDGui.h"
#include "AnalogGyroGui.h"
#include "AnalogInputGui.h"
#include "AnalogOutGui.h"
#include "CompressorGui.h"
#include "DIOGui.h"
#include "DriverStationGui.h"
#include "EncoderGui.h"
#include "Field2D.h"
#include "HALSimGui.h"
#include "PDPGui.h"
#include "PWMGui.h"
#include "RelayGui.h"
#include "RoboRioGui.h"
#include "SimDeviceGui.h"
#include "SolenoidGui.h"
#include "TimingGui.h"

using namespace halsimgui;

extern "C" {
#if defined(WIN32) || defined(_WIN32)
__declspec(dllexport)
#endif
    int HALSIM_InitExtension(void) {
  HALSimGui::Add(AccelerometerGui::Initialize);
  HALSimGui::Add(AddressableLEDGui::Initialize);
  HALSimGui::Add(AnalogGyroGui::Initialize);
  HALSimGui::Add(AnalogInputGui::Initialize);
  HALSimGui::Add(AnalogOutGui::Initialize);
  HALSimGui::Add(CompressorGui::Initialize);
  HALSimGui::Add(DriverStationGui::Initialize);
  HALSimGui::Add(DIOGui::Initialize);
  HALSimGui::Add(EncoderGui::Initialize);
  HALSimGui::Add(Field2D::Initialize);
  HALSimGui::Add(PDPGui::Initialize);
  HALSimGui::Add(PWMGui::Initialize);
  HALSimGui::Add(RelayGui::Initialize);
  HALSimGui::Add(RoboRioGui::Initialize);
  HALSimGui::Add(SimDeviceGui::Initialize);
  HALSimGui::Add(SolenoidGui::Initialize);
  HALSimGui::Add(TimingGui::Initialize);

  wpi::outs() << "Simulator GUI Initializing.\n";
  if (!HALSimGui::Initialize()) return 0;
  HAL_SetMain(nullptr, HALSimGui::Main, HALSimGui::Exit);
  wpi::outs() << "Simulator GUI Initialized!\n";

  return 0;
}
}  // extern "C"
