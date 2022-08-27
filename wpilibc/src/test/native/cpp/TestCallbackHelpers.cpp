// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "callback_helpers/TestCallbackHelpers.h"

#include <fmt/format.h>

namespace frc::sim {

void BooleanCallback::HandleCallback(std::string_view name,
                                     const HAL_Value* value) {
  if (!value) {
    throw std::invalid_argument("Null value");
  }
  if (value->type != HAL_BOOLEAN) {
    throw std::invalid_argument(fmt::format("Wrong type '{}' for boolean",
                                            static_cast<int>(value->type))
                                    .c_str());
  }
  m_wasTriggered = true;
  m_lastValue = value->data.v_boolean;
}

void EnumCallback::HandleCallback(std::string_view name,
                                  const HAL_Value* value) {
  if (!value) {
    throw std::invalid_argument("Null value");
  }
  if (value->type != HAL_ENUM) {
    throw std::invalid_argument(
        fmt::format("Wrong type '{}' for enum", static_cast<int>(value->type))
            .c_str());
  }

  m_wasTriggered = true;
  m_lastValue = value->data.v_enum;
}

void IntCallback::HandleCallback(std::string_view name,
                                 const HAL_Value* value) {
  if (!value) {
    throw std::invalid_argument("Null value");
  }
  if (value->type != HAL_INT) {
    throw std::invalid_argument(fmt::format("Wrong type '{}' for integer",
                                            static_cast<int>(value->type))
                                    .c_str());
  }

  m_wasTriggered = true;
  m_lastValue = value->data.v_int;
}

void LongCallback::HandleCallback(std::string_view name,
                                  const HAL_Value* value) {
  if (!value) {
    throw std::invalid_argument("Null value");
  }
  if (value->type != HAL_LONG) {
    throw std::invalid_argument(
        fmt::format("Wrong type '{}' for long", static_cast<int>(value->type))
            .c_str());
  }

  m_wasTriggered = true;
  m_lastValue = value->data.v_long;
}

void DoubleCallback::HandleCallback(std::string_view name,
                                    const HAL_Value* value) {
  if (!value) {
    throw std::invalid_argument("Null value");
  }
  if (value->type != HAL_DOUBLE) {
    throw std::invalid_argument(
        fmt::format("Wrong type '{}' for double", static_cast<int>(value->type))
            .c_str());
  }

  m_wasTriggered = true;
  m_lastValue = value->data.v_double;
}

}  // namespace frc::sim
