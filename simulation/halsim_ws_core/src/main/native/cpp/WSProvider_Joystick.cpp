// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "WSProvider_Joystick.h"

#include <algorithm>
#include <atomic>
#include <vector>

#include <hal/Ports.h>
#include <hal/simulation/DriverStationData.h>

namespace wpilibws {

extern std::atomic<bool>* gDSSocketConnected;

void HALSimWSProviderJoystick::Initialize(WSRegisterFunc webregisterFunc) {
  CreateProviders<HALSimWSProviderJoystick>("Joystick", HAL_kMaxJoysticks,
                                            webregisterFunc);
}

HALSimWSProviderJoystick::~HALSimWSProviderJoystick() {
  DoCancelCallbacks();
}

void HALSimWSProviderJoystick::RegisterCallbacks() {
  m_dsNewDataCbKey = HALSIM_RegisterDriverStationNewDataCallback(
      [](const char* name, void* param, const struct HAL_Value* value) {
        auto provider = static_cast<HALSimWSProviderJoystick*>(param);

        // Grab all joystick data and send it at once
        wpi::json payload;

        // Axes data
        HAL_JoystickAxes axes{};
        std::vector<double> axesValues;
        HALSIM_GetJoystickAxes(provider->GetChannel(), &axes);

        for (int i = 0; i < axes.count; i++) {
          axesValues.push_back(axes.axes[i]);
        }

        // POVs data
        HAL_JoystickPOVs povs{};
        std::vector<int16_t> povsValues;
        HALSIM_GetJoystickPOVs(provider->GetChannel(), &povs);

        for (int i = 0; i < povs.count; i++) {
          povsValues.push_back(povs.povs[i]);
        }

        // Button data
        HAL_JoystickButtons buttons{};
        std::vector<bool> buttonsValues;

        for (int i = 0; i < buttons.count; i++) {
          buttonsValues.push_back(((buttons.buttons >> i) & 0x1) == 1);
        }

        // Rumble data
        int64_t outputs = 0;
        int32_t leftRumble = 0;
        int32_t rightRumble = 0;
        HALSIM_GetJoystickOutputs(provider->GetChannel(), &outputs, &leftRumble,
                                  &rightRumble);

        payload[">axes"] = axesValues;
        payload[">povs"] = povsValues;
        payload[">buttons"] = buttonsValues;
        payload["<outputs"] = outputs;
        payload["<rumble_left"] = leftRumble;
        payload["<rumble_right"] = rightRumble;

        provider->ProcessHalCallback(payload);
      },
      this, true);
}

void HALSimWSProviderJoystick::CancelCallbacks() {
  DoCancelCallbacks();
}

void HALSimWSProviderJoystick::DoCancelCallbacks() {
  HALSIM_CancelDriverStationNewDataCallback(m_dsNewDataCbKey);

  m_dsNewDataCbKey = 0;
}

void HALSimWSProviderJoystick::OnNetValueChanged(const wpi::json& json) {
  // ignore if DS connected
  if (gDSSocketConnected && *gDSSocketConnected) {
    return;
  }

  wpi::json::const_iterator it;
  if ((it = json.find(">axes")) != json.end()) {
    HAL_JoystickAxes axes{};
    axes.count =
        std::min(it.value().size(),
                 static_cast<wpi::json::size_type>(HAL_kMaxJoystickAxes));
    for (int i = 0; i < axes.count; i++) {
      axes.axes[i] = it.value()[i];
    }

    HALSIM_SetJoystickAxes(m_channel, &axes);
  }

  if ((it = json.find(">buttons")) != json.end()) {
    HAL_JoystickButtons buttons{};
    buttons.count =
        std::min(it.value().size(), static_cast<wpi::json::size_type>(32));
    for (int i = 0; i < buttons.count; i++) {
      if (it.value()[i]) {
        buttons.buttons |= 1 << i;
      }
    }

    HALSIM_SetJoystickButtons(m_channel, &buttons);
  }

  if ((it = json.find(">povs")) != json.end()) {
    HAL_JoystickPOVs povs{};
    povs.count =
        std::min(it.value().size(),
                 static_cast<wpi::json::size_type>(HAL_kMaxJoystickPOVs));
    for (int i = 0; i < povs.count; i++) {
      povs.povs[i] = it.value()[i];
    }

    HALSIM_SetJoystickPOVs(m_channel, &povs);
  }
  // We won't send any updates to user code until the new data message
  // is received by the driver station provider
}

}  // namespace wpilibws
