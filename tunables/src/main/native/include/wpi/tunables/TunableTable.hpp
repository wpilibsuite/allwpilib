// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <memory>
#include <string>
#include <string_view>
#include <utility>

#include "wpi/tunables/CustomTunable.hpp"

namespace wpi::tunables {

template <typename T, typename... I>
class Tunable;

namespace detail {
class TunableBase;
class TunableMemberBase;
}  // namespace detail

class ComplexTunable;
struct TunableConfig;

/**
 * Tunables are used to allow values in the robot program to be changed from
 * dashboards or debug tools.
 *
 * <p>For more advanced use cases, use NetworkTables directly.
 */
class TunableTable final {
 public:
  /**
   * Constructs a tunable table.
   *
   * @param path path with trailing "/".
   */
  explicit TunableTable(std::string path) : m_path{std::move(path)} {}

  /**
   * Gets the table path.
   *
   * @return path with trailing "/"
   */
  const std::string& GetPath() const { return m_path; }

  /**
   * Gets a child tunable table.
   *
   * @param name table name
   * @return table
   */
  TunableTable GetTable(std::string_view name);

  /**
   * Publishes a tunable.
   *
   * @param name the name
   * @param tunable the tunable
   * @return true if the backend accepted the tunable
   */
  bool Publish(std::string_view name, detail::TunableBase& tunable);

  /**
   * Publishes a complex tunable.
   *
   * @param name the name
   * @param tunable the complex tunable object
   * @return true if the backend accepted the tunable
   */
  bool Publish(std::string_view name, ComplexTunable& tunable);

  /**
   * Publishes a custom tunable.
   *
   * @param name the name
   * @param tunable the custom tunable
   * @return true if the backend accepted the tunable
   */
  template <typename T, typename... I>
    requires detail::IsCustomTunable<T, I...>
  bool Publish(std::string_view name, Tunable<T, I...>& tunable) {
    return Publish(name, tunable.GetInnerTunable());
  }

  /**
   * Publishes a tunable for a member variable of a complex tunable.
   *
   * @param name the name
   * @param tunable the complex tunable object
   * @param member the member tunable for the member variable
   * @return true if the backend accepted the tunable
   */
  bool Publish(std::string_view name, ComplexTunable* tunable,
               std::unique_ptr<detail::TunableMemberBase> member);

  /**
   * Publishes a tunable for a member variable of a complex tunable.
   *
   * @param name the name
   * @param tunable the complex tunable object
   * @param member the member variable pointer
   * @param info additional information forwarded to the member tunable wrapper
   * @return true if the backend accepted the tunable
   */
  template <typename T, std::derived_from<ComplexTunable> Class, typename... I>
    requires std::convertible_to<
        std::unique_ptr<detail::TunableMemberBase>,
        decltype(MakeTunableMember(std::declval<T Class::*>()))>
  bool Publish(std::string_view name, Class* tunable, T Class::* member,
               I&&... info) {
    return Publish(name, tunable,
                   MakeTunableMember(member, std::forward<I>(info)...));
  }

  /**
   * Publishes a tunable for a member variable of a complex tunable.
   *
   * @param name the name
   * @param tunable the complex tunable object
   * @param member the member variable pointer
   * @param config the tunable config
   * @param info additional information forwarded to the member tunable wrapper
   * @return true if the backend accepted the tunable
   */
  template <typename T, std::derived_from<ComplexTunable> Class, typename... I>
    requires std::convertible_to<
        std::unique_ptr<detail::TunableMemberBase>,
        decltype(MakeTunableMember(std::declval<T Class::*>(),
                                   std::declval<TunableConfig>()))>
  bool Publish(std::string_view name, Class* tunable, T Class::* member,
               const TunableConfig& config, I&&... info) {
    return Publish(name, tunable,
                   MakeTunableMember(member, config, std::forward<I>(info)...));
  }

  /**
   * Removes a tunable and any descendant tunables.
   * @param name name
   */
  void Remove(std::string_view name);

 private:
  std::string m_path;
};

}  // namespace wpi::tunables
