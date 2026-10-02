// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/halsim/ws_core/WSBaseProvider.hpp"

#include <utility>

namespace wpilibws {

HALSimWSBaseProvider::HALSimWSBaseProvider(std::string_view key,
                                           std::string_view type)
    : m_key(key), m_type(type) {}

void HALSimWSBaseProvider::OnNetValueChanged(const wpi::util::json& json) {
  // empty
}

}  // namespace wpilibws
