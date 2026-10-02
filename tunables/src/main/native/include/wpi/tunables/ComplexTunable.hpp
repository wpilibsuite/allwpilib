// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string_view>
#include <utility>

#include "wpi/tunables/detail/TunableBase.hpp"
#include "wpi/util/SymbolExports.hpp"

namespace wpi::tunables {

class TunableTable;

/** The base interface for complex (internally mutable) tunable objects. */
class WPILIB_DLLEXPORT ComplexTunable : public detail::TunableBase {
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
   *
   * This function may be called when the tunable is initially published or
   * republished during backend migration, and it must not throw. If an
   * exception escapes, registry and backend state is not guaranteed to be
   * restored.
   *
   * @param table table
   */
  virtual void PublishTunable(TunableTable& table) = 0;

  /**
   * Updates this complex tunable from its child tunables.
   *
   * This function must not throw. If an exception escapes, registry and
   * backend state is not guaranteed to be restored.
   */
  virtual void UpdateTunable() const {}

 protected:
  constexpr ComplexTunable() : TunableBase{detail::TunableTypeValue::COMPLEX} {}
  constexpr ComplexTunable(const ComplexTunable&) = default;
  constexpr ComplexTunable(ComplexTunable&& other)
      : detail::TunableBase{std::move(other)} {
    if !consteval {
      TunableRegistry::MoveTunableCallbackParent(&other, this);
    }
  }
  constexpr ComplexTunable& operator=(const ComplexTunable&) = default;
  constexpr ComplexTunable& operator=(ComplexTunable&& other) {
    if (this != &other) {
      if !consteval {
        TunableRegistry::PrepareComplexMoveAssignment(*this, other);
      }
      detail::TunableBase::operator=(std::move(other));
      if !consteval {
        TunableRegistry::MoveTunableCallbackParent(&other, this);
      }
    }
    return *this;
  }

  explicit ComplexTunable(const ForceTunableRegister& fr)
      : TunableBase{detail::TunableTypeValue::COMPLEX, fr} {}

  /**
   * Publishes a child tunable under all currently published paths for this
   * complex tunable.
   *
   * If this complex tunable is not currently published, this function does
   * nothing. Complex tunables should still publish their current children from
   * PublishTunable() so backend migration and full republish operations can
   * recreate the complete child set.
   *
   * @param name the child name
   * @param tunable the child tunable
   */
  void PublishChildTunable(std::string_view name, detail::TunableBase& tunable);

  /**
   * Publishes a child complex tunable under all currently published paths for
   * this complex tunable.
   *
   * If this complex tunable is not currently published, this function does
   * nothing. Complex tunables should still publish their current children from
   * PublishTunable() so backend migration and full republish operations can
   * recreate the complete child set.
   *
   * @param name the child name
   * @param tunable the child complex tunable
   */
  void PublishChildTunable(std::string_view name, ComplexTunable& tunable);

  /**
   * Removes a child tunable from all currently published paths for this complex
   * tunable.
   *
   * If this complex tunable is not currently published, this function does
   * nothing.
   *
   * @param name the child name
   */
  void RemoveChildTunable(std::string_view name);

  /**
   * Marks a child tunable as locally changed.
   *
   * Use this for child tunables published from plain member variables when the
   * member is mutated outside of the generated tunable wrapper.
   *
   * If this complex tunable or the child tunable is not currently published,
   * this function does nothing.
   *
   * @param name the child name
   */
  void SetChildTunableChanged(std::string_view name);

  virtual ~ComplexTunable() = default;
};

}  // namespace wpi::tunables
