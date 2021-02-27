// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "WSBaseProvider.h"

#include <utility>

namespace wpilibws {

HALSimWSBaseProvider::HALSimWSBaseProvider(std::string key, std::string type)
    : m_key(std::move(key)), m_type(std::move(type)) {}

void HALSimWSBaseProvider::OnNetValueChanged(const wpi::json& json) {
  // empty
}

}  // namespace wpilibws
