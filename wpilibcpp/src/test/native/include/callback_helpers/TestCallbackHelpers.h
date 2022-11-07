// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <string_view>

#include <hal/Value.h>

namespace frc::sim {

template <typename ValueType>
class BaseCallbackHelper {
 public:
  bool WasTriggered() const { return m_wasTriggered; }
  ValueType GetLastValue() const { return m_lastValue; }

  void Reset() {
    m_wasTriggered = false;
    m_lastValue = 0;
  }

  virtual void HandleCallback(std::string_view name,
                              const HAL_Value* value) = 0;

  std::function<void(std::string_view, const HAL_Value*)> GetCallback() {
    return [&](std::string_view name, const HAL_Value* value) {
      HandleCallback(name, value);
    };
  }

 protected:
  bool m_wasTriggered{false};
  ValueType m_lastValue{0};
};

class BooleanCallback : public BaseCallbackHelper<bool> {
 public:
  void HandleCallback(std::string_view name, const HAL_Value* value) override;
};

class EnumCallback : public BaseCallbackHelper<int32_t> {
 public:
  void HandleCallback(std::string_view name, const HAL_Value* value) override;
};

class IntCallback : public BaseCallbackHelper<int32_t> {
 public:
  void HandleCallback(std::string_view name, const HAL_Value* value) override;
};

class LongCallback : public BaseCallbackHelper<int64_t> {
 public:
  void HandleCallback(std::string_view name, const HAL_Value* value) override;
};

class DoubleCallback : public BaseCallbackHelper<double> {
 public:
  void HandleCallback(std::string_view name, const HAL_Value* value) override;
};

}  // namespace frc::sim
