// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <concepts>
#include <memory>
#include <string>
#include <string_view>
#include <utility>

#include "wpi/system/Errors.hpp"
#include "wpi/telemetry/TelemetryLoggable.hpp"
#include "wpi/util/StringMap.hpp"
#include "wpi/util/mutex.hpp"

namespace wpi {

/**
 * Common base class for all Mechanism2d node types.
 *
 * To append another node, call Append with the type of node and its
 * construction parameters. None of the node types are designed to be
 * constructed directly, and are owned by their parent node/container - obtain
 * pointers from the Append function or similar factory methods.
 *
 * @see Mechanism2d.
 */
class MechanismObject2d : public wpi::TelemetryLoggable {
  friend class Mechanism2d;

 protected:
  explicit MechanismObject2d(std::string_view name);

  mutable wpi::util::mutex m_mutex;

 public:
  virtual ~MechanismObject2d() = default;

  /**
   * Retrieve the object's name.
   *
   * @return the object's name relative to its parent.
   */
  const std::string& GetName() const;

  /**
   * Append a Mechanism object that is based on this one.
   *
   * @param name the name of the new object.
   * @param args constructor arguments of the object type.
   * @return the constructed and appended object, useful for variable
   * assignments and call chaining.
   * @throw if an object with the given name already exists.
   */
  template <typename T, typename... Args>
    requires std::convertible_to<T*, MechanismObject2d*>
  T* Append(std::string_view name, Args&&... args) {
    std::scoped_lock lock(m_mutex);
    auto& obj = m_objects[name];
    if (obj) {
      throw WPILIB_MakeError(
          err::Error,
          "MechanismObject names must be unique! `{}` was inserted twice!",
          name);
    }
    obj = std::make_unique<T>(name, std::forward<Args>(args)...);
    return static_cast<T*>(obj.get());
  }

  void UpdateTelemetry(wpi::TelemetryTable& table) const override;

 private:
  std::string m_name;
  wpi::util::StringMap<std::unique_ptr<MechanismObject2d>> m_objects;
};
}  // namespace wpi
