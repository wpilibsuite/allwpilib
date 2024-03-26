// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <string_view>
#include <type_traits>

namespace wpi2 {

class SendableHelper;
class SendableSet;
class SendableTable;

namespace detail {

template <typename T>
struct is_shared_ptr : std::false_type {};
template <typename T>
struct is_shared_ptr<std::shared_ptr<T>> : std::true_type {};
template <typename T>
concept IsSharedPtr = is_shared_ptr<T>::value;

template <typename T>
concept not_movable =
    !std::move_constructible<T> && !std::assignable_from<T&, T>;

template <typename T>
concept SendableHelpedOrNotMovable =
    not_movable<T> || std::derived_from<T, SendableHelper>;

}  // namespace detail

/**
 * Sendable serialization template. Unspecialized class has no members; only
 * specializations of this class are useful, and only if they meet the
 * SendableSerializable concept.
 *
 * @tparam T type to serialize/deserialize
 * @tparam I optional sendable type info
 */
template <typename T, typename... I>
struct Sendable {};

/**
 * Specifies that a type is capable of sendable serialization and
 * deserialization.
 *
 * Implementations must define a template specialization for wpi::Sendable with
 * T being the type that is being serialized/deserialized, with the following
 * static members (as enforced by this concept):
 * - std::string_view GetTypeString(): function that returns the dashboard type
 *   string
 * - void InitSendable(T* obj, SendableTable& table)
 * - void CloseSendable(T* obj)
 *
 * If possible, the GetTypeString(), GetSize(), and GetSchema() functions should
 * be marked constexpr. GetTypeString() may return a type other than
 * std::string_view, as long as the return value is convertible to
 * std::string_view.
 */
template <typename T, typename... I>
concept SendableSerializableRawPointer =
    detail::SendableHelpedOrNotMovable<T> &&
    requires(T* obj, SendableTable& table, const I&... info) {
      typename Sendable<typename std::remove_cvref_t<T>,
                        typename std::remove_cvref_t<I>...>;
      {
        Sendable<typename std::remove_cvref_t<T>,
                 typename std::remove_cvref_t<I>...>::GetTypeString(info...)
      } -> std::convertible_to<std::string_view>;
      Sendable<typename std::remove_cvref_t<T>,
               typename std::remove_cvref_t<I>...>::InitSendable(obj, table,
                                                                 info...);
      Sendable<typename std::remove_cvref_t<T>,
               typename std::remove_cvref_t<I>...>::CloseSendable(obj, info...);
    };

template <typename T, typename... I>
concept SendableSerializableSharedPointer =
    detail::IsSharedPtr<T> &&
    requires(T objPtr, SendableTable& table, const I&... info) {
      typename Sendable<typename std::remove_cvref_t<T>,
                        typename std::remove_cvref_t<I>...>;
      {
        Sendable<typename std::remove_cvref_t<T>,
                 typename std::remove_cvref_t<I>...>::GetTypeString(info...)
      } -> std::convertible_to<std::string_view>;
      Sendable<typename std::remove_cvref_t<T>,
               typename std::remove_cvref_t<I>...>::InitSendable(objPtr, table,
                                                                 info...);
      Sendable<typename std::remove_cvref_t<T>,
               typename std::remove_cvref_t<I>...>::CloseSendable(objPtr,
                                                                  info...);
    };

template <typename T, typename... I>
concept SendableSerializable = SendableSerializableRawPointer<T, I...> ||
                               SendableSerializableSharedPointer<T, I...>;

/**
 * Get the type string for a sendable serializable type
 *
 * @tparam T serializable type
 * @param info optional struct type info
 * @return type string
 */
template <typename T, typename... I>
  requires SendableSerializable<T, I...>
constexpr auto GetSendableTypeString(const I&... info) {
  using S = Sendable<T, typename std::remove_cvref_t<I>...>;
  return S::GetTypeString(info...);
}

}  // namespace wpi2
