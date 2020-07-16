/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "WSBaseProvider.h"

namespace wpilibws {

HALSimWSBaseProvider::HALSimWSBaseProvider(const std::string& key)
    : m_key(key) {}

void HALSimWSBaseProvider::OnNetValueChanged(const wpi::json& json) {
  // empty
}

}  // namespace wpilibws
