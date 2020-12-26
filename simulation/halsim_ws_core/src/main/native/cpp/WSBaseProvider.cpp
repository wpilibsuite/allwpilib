// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "WSBaseProvider.h"

namespace wpilibws {

HALSimWSBaseProvider::HALSimWSBaseProvider(const std::string& key,
                                           const std::string& type)
    : m_key(key), m_type(type) {}

void HALSimWSBaseProvider::OnNetValueChanged(const wpi::json& json) {
  // empty
}

}  // namespace wpilibws
