// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <concepts>
#include <memory>
#include <string_view>
#include <utility>

#include "wpi/tunables/Tunable.hpp"
#include "wpi/tunables/TunableTable.hpp"

namespace wpi::tunables {

class ComplexTunable;

/**
 * Gets the root tunable table.
 *
 * @return table
 */
TunableTable GetTable();

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
 * Publishes a complex tunable object.
 *
 * @param name the name
 * @param args the constructor arguments for the tunable object
 * @return the created tunable object
 */
template <std::derived_from<ComplexTunable> T, typename... Args>
T AddComplex(std::string_view name, Args&&... args) {
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
Tunable<T> Add(std::string_view name, Args&&... params) {
  Tunable<T> tunable = Tunable<T>{std::forward<Args>(params)...};
  Publish(name, tunable);
  return tunable;
}

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
 *
 * @param name name
 */
void Remove(std::string_view name);

}  // namespace wpi::tunables
