// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string_view>

#include "wpi/tunable/detail/TunableBase.hpp"

namespace wpi {

class TunableTable;

/** The base interface for complex (internally mutable) tunable objects. */
class ComplexTunable : public detail::TunableBase {
  void anchor();  // provide a place for the vtable

 public:
  /**
   * Gets the tunable table type. Default is no specified table type (null).
   *
   * @return Table type
   */
  virtual std::string_view GetTunableType() const { return {}; }

  /**
   * Publishes the tunable to the given table.
   * @param table table
   */
  virtual void PublishTunable(TunableTable& table) = 0;

  virtual void UpdateTunable() const {}

 protected:
  constexpr ComplexTunable() : TunableBase{detail::TunableTypeValue::COMPLEX} {}
  explicit ComplexTunable(const ForceTunableRegister& fr)
      : TunableBase{detail::TunableTypeValue::COMPLEX, fr} {}

  virtual ~ComplexTunable() = default;
};

}  // namespace wpi
