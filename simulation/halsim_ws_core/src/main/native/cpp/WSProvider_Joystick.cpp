/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "WSProvider_Joystick.h"

#include <hal/Ports.h>
#include <hal/simulation/DriverStationData.h>

namespace wpilibws {

void HALSimWSProviderJoystick::Initialize(WSRegisterFunc webregisterFunc) {
  CreateProviders<HALSimWSProviderJoystick>("Joystick", HAL_kMaxJoysticks,
                                            webregisterFunc);
}

void HALSimWSProviderJoystick::RegisterCallbacks() {
  m_axesCbKey = HALSIM_RegisterJoystickAxesCallback(
      m_channel,
      [](const char* name, void* param, int32_t joystickNum,
         const HAL_JoystickAxes* axes) {
        std::vector<double> axesValues;
        for (int i = 0; i < axes->count; i++) {
          axesValues.push_back(axes->axes[i]);
        }

        auto provider = static_cast<HALSimWSProviderJoystick*>(param);
        if (provider->GetChannel() != joystickNum) {
          return;
        }

        provider->ProcessHalCallback({{">axes", axesValues}});
      },
      this, true);

  m_povsCbKey = HALSIM_RegisterJoystickPOVsCallback(
      m_channel,
      [](const char* name, void* param, int32_t joystickNum,
         const HAL_JoystickPOVs* povs) {
        std::vector<int16_t> povsValues;
        for (int i = 0; i < povs->count; i++) {
          povsValues.push_back(povs->povs[i]);
        }
        auto provider = static_cast<HALSimWSProviderJoystick*>(param);
        if (provider->GetChannel() != joystickNum) {
          return;
        }

        provider->ProcessHalCallback({{">povs", povsValues}});
      },
      this, true);

  m_buttonsCbKey = HALSIM_RegisterJoystickButtonsCallback(
      m_channel,
      [](const char* name, void* param, int32_t joystickNum,
         const HAL_JoystickButtons* buttons) {
        std::vector<bool> buttonsValues;

        for (int i = 0; i < buttons->count; i++) {
          buttonsValues.push_back(((buttons->buttons >> i) & 0x1) == 1);
        }

        auto provider = static_cast<HALSimWSProviderJoystick*>(param);
        if (provider->GetChannel() != joystickNum) {
          return;
        }

        provider->ProcessHalCallback({{">buttons", buttonsValues}});
      },
      this, true);
}

void HALSimWSProviderJoystick::CancelCallbacks() {
  HALSIM_CancelJoystickAxesCallback(m_axesCbKey);
  HALSIM_CancelJoystickPOVsCallback(m_povsCbKey);
  HALSIM_CancelJoystickButtonsCallback(m_buttonsCbKey);
}

void HALSimWSProviderJoystick::OnNetValueChanged(const wpi::json& json) {
  wpi::json::const_iterator it;
  if ((it = json.find(">axes")) != json.end()) {
    HAL_JoystickAxes axes{};
    axes.count =
        std::min(it.value().size(), (wpi::json::size_type)HAL_kMaxJoystickAxes);
    for (int i = 0; i < axes.count; i++) {
      axes.axes[i] = it.value()[i];
    }

    HALSIM_SetJoystickAxes(m_channel, &axes);
  }

  if ((it = json.find(">buttons")) != json.end()) {
    HAL_JoystickButtons buttons{};
    buttons.count = std::min(it.value().size(), (wpi::json::size_type)32);
    for (int i = 0; i < buttons.count; i++) {
      if (it.value()[i]) {
        buttons.buttons |= 1 << (i - 1);
      }
    }

    HALSIM_SetJoystickButtons(m_channel, &buttons);
  }

  if ((it = json.find(">povs")) != json.end()) {
    HAL_JoystickPOVs povs{};
    povs.count =
        std::min(it.value().size(), (wpi::json::size_type)HAL_kMaxJoystickPOVs);
    for (int i = 0; i < povs.count; i++) {
      povs.povs[i] = it.value()[i];
    }

    HALSIM_SetJoystickPOVs(m_channel, &povs);
  }

  HALSIM_NotifyDriverStationNewData();
}

}  // namespace wpilibws
