// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/halsim/ws_core/WSProvider_Joystick.hpp"

#include <algorithm>
#include <atomic>
#include <vector>

#include "wpi/hal/Ports.h"
#include "wpi/hal/simulation/DriverStationData.h"

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
        wpi::util::json payload;

        // Axes data
        HAL_JoystickAxes axes{};
        std::vector<double> axesValues;
        HALSIM_GetJoystickAxes(provider->GetChannel(), &axes);

        int axesCount = 16 - std::countl_zero(axes.available);

        for (int i = 0; i < axesCount; i++) {
          axesValues.push_back(axes.axes[i]);
        }

        // POVs data
        HAL_JoystickPOVs povs{};
        std::vector<int16_t> povsValues;
        HALSIM_GetJoystickPOVs(provider->GetChannel(), &povs);

        int povsCount = 8 - std::countl_zero(povs.available);

        for (int i = 0; i < povsCount; i++) {
          povsValues.push_back(povs.povs[i]);
        }

        // Button data
        HAL_JoystickButtons buttons{};
        std::vector<bool> buttonsValues;

        int buttonsCount = 64 - std::countl_zero(buttons.available);

        for (int i = 0; i < buttonsCount; i++) {
          buttonsValues.push_back(((buttons.buttons >> i) & 0x1) == 1);
        }

        // Rumble data
        int32_t leds = 0;
        int32_t leftRumble = 0;
        int32_t rightRumble = 0;
        int32_t leftTriggerRumble = 0;
        int32_t rightTriggerRumble = 0;
        HALSIM_GetJoystickRumbles(provider->GetChannel(), &leftRumble,
                                  &rightRumble, &leftTriggerRumble,
                                  &rightTriggerRumble);
        HALSIM_GetJoystickLeds(provider->GetChannel(), &leds);

        payload[">axes"] = axesValues;
        payload[">povs"] = povsValues;
        payload[">buttons"] = buttonsValues;
        payload["<leds"] = leds;
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

void HALSimWSProviderJoystick::OnNetValueChanged(const wpi::util::json& json) {
  // ignore if DS connected
  if (gDSSocketConnected && *gDSSocketConnected) {
    return;
  }

  wpi::util::json::const_iterator it;
  if ((it = json.find(">axes")) != json.end()) {
    HAL_JoystickAxes axes{};
    wpi::util::json::size_type axesCount =
        std::min(it.value().size(),
                 static_cast<wpi::util::json::size_type>(HAL_kMaxJoystickAxes));
    axes.available = (1 << axesCount) - 1;
    for (wpi::util::json::size_type i = 0; i < axesCount; i++) {
      axes.axes[i] = it.value()[i];
    }

    HALSIM_SetJoystickAxes(m_channel, &axes);
  }

  if ((it = json.find(">buttons")) != json.end()) {
    HAL_JoystickButtons buttons{};
    wpi::util::json::size_type buttonsCount = std::min(
        it.value().size(), static_cast<wpi::util::json::size_type>(64));
    if (buttonsCount < 64) {
      buttons.available = (1ULL << buttonsCount) - 1;
    } else {
      buttons.available = (std::numeric_limits<uint64_t>::max)();
    }
    for (wpi::util::json::size_type i = 0; i < buttonsCount; i++) {
      if (it.value()[i]) {
        buttons.buttons |= 1llu << i;
      }
    }

    HALSIM_SetJoystickButtons(m_channel, &buttons);
  }

  if ((it = json.find(">povs")) != json.end()) {
    HAL_JoystickPOVs povs{};
    wpi::util::json::size_type povsCount =
        std::min(it.value().size(),
                 static_cast<wpi::util::json::size_type>(HAL_kMaxJoystickPOVs));
    povs.available = (1 << povsCount) - 1;
    for (wpi::util::json::size_type i = 0; i < povsCount; i++) {
      povs.povs[i] = it.value()[i];
    }

    HALSIM_SetJoystickPOVs(m_channel, &povs);
  }
  // We won't send any updates to user code until the new data message
  // is received by the driver station provider
}

}  // namespace wpilibws
