// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <concepts>
#include <memory>
#include <string_view>
#include <utility>

#include "wpi/tunable/Tunable.hpp"
#include "wpi/tunable/TunableTable.hpp"

namespace wpi {

class ComplexTunable;

/**
 * Tunables are used to allow values in the robot program to be changed from
 * dashboards or debug tools.
 *
 * <p>For more advanced use cases, use NetworkTables directly.
 */
class Tunables final {
 public:
  Tunables() = delete;

  /**
   * Gets the root tunable table.
   *
   * @return table
   */
  static TunableTable GetTable();

  /**
   * Gets a child tunable table.
   *
   * @param name table name
   * @return table
   */
  static TunableTable GetTable(std::string_view name);

  /**
   * Publishes a complex tunable object.
   *
   * @param name the name
   * @param args the constructor arguments for the tunable object
   * @return the created tunable object
   */
  template <std::derived_from<ComplexTunable> T, typename... Args>
  static T AddComplex(std::string_view name, Args&&... args) {
    T tunable = T{std::forward<Args>(args)...};
    Publish(name, tunable);
    return tunable;
  }

  /**
   * Creates and publishes a tunable value.
   *
   * @param name the name
   * @param params the parameters to pass to the tunable constructor (typically
   * initial value)
   * @return tunable
   */
  template <typename T, typename... Args>
  static Tunable<T> Add(std::string_view name, Args&&... params) {
    Tunable<T> tunable = Tunable<T>{std::forward<Args>(params)...};
    Publish(name, tunable);
    return tunable;
  }

  /**
   * Publishes a tunable.
   *
   * @param name the name
   * @param tunable the tunable
   */
  static void Publish(std::string_view name, detail::TunableBase& tunable);

  /**
   * Publishes a complex tunable.
   *
   * @param name the name
   * @param tunable the complex tunable object
   */
  static void Publish(std::string_view name, ComplexTunable& tunable);

  /**
   * Publishes a custom tunable.
   *
   * @param name the name
   * @param tunable the custom tunable
   */
  template <typename T, typename... I>
    requires detail::IsCustomTunable<T, I...>
  static void Publish(std::string_view name, Tunable<T, I...>& tunable) {
    Publish(name, tunable.GetInnerTunable());
  }

  /**
   * Publishes a tunable for a member variable of a complex tunable.
   *
   * @param name the name
   * @param tunable the complex tunable object
   * @param member the member tunable for the member variable
   */
  static void Publish(std::string_view name, ComplexTunable* tunable,
                      std::unique_ptr<detail::TunableMemberBase> member);

  /**
   * Publishes a tunable for a member variable of a complex tunable.
   *
   * @param name the name
   * @param tunable the complex tunable object
   * @param member the member variable pointer
   */
  template <typename T, std::derived_from<ComplexTunable> Class, typename... I>
    requires std::convertible_to<
        std::unique_ptr<detail::TunableMemberBase>,
        decltype(MakeTunableMember(std::declval<T Class::*>()))>
  static void Publish(std::string_view name, Class* tunable, T Class::* member,
                      I&&... info) {
    Publish(name, tunable, MakeTunableMember(member, std::forward<I>(info)...));
  }

  /**
   * Publishes a tunable for a member variable of a complex tunable.
   *
   * @param name the name
   * @param tunable the complex tunable object
   * @param member the member variable pointer
   * @param config the tunable config
   */
  template <typename T, std::derived_from<ComplexTunable> Class, typename... I>
    requires std::convertible_to<
        std::unique_ptr<detail::TunableMemberBase>,
        decltype(MakeTunableMember(std::declval<T Class::*>(),
                                   std::declval<TunableConfig>()))>
  static void Publish(std::string_view name, Class* tunable, T Class::* member,
                      const TunableConfig& config, I&&... info) {
    Publish(name, tunable,
            MakeTunableMember(member, config, std::forward<I>(info)...));
  }

  /**
   * Removes a tunable.
   * @param name name
   */
  static void Remove(std::string_view name);
};

}  // namespace wpi
