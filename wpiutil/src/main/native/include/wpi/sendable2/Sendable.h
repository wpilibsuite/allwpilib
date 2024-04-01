// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <string_view>
#include <type_traits>

namespace wpi {
class MoveTrackerBase;
}  // namespace wpi

namespace wpi2 {

class SendableTable;

namespace detail {

template <typename T>
concept MoveTracked = std::derived_from<T, wpi::MoveTrackerBase>;

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
 * deserialization via a raw pointer. This requires the type be derived from
 * wpi::MoveTracker so that moves can be tracked.
 *
 * Implementations must define a template specialization for wpi::Sendable with
 * T being the type that is being serialized/deserialized, with the following
 * static members (as enforced by this concept):
 * - std::string_view GetTypeString(): function that returns the dashboard type
 *   string
 * - void Init(T* obj, SendableTable& table)
 * - void Close(T* obj, SendableTable& table)
 *
 * If possible, the GetTypeString() function should be marked constexpr.
 * GetTypeString() may return a type other than std::string_view, as long as the
 * return value is convertible to std::string_view.
 */
template <typename T, typename... I>
concept SendableSerializableMoveTracked =
    detail::MoveTracked<T> &&
    requires(T* obj, SendableTable& table, const I&... info) {
      typename Sendable<typename std::remove_cvref_t<T>,
                        typename std::remove_cvref_t<I>...>;
      {
        Sendable<typename std::remove_cvref_t<T>,
                 typename std::remove_cvref_t<I>...>::GetTypeString(info...)
      } -> std::convertible_to<std::string_view>;
      Sendable<typename std::remove_cvref_t<T>,
               typename std::remove_cvref_t<I>...>::Init(obj, table, info...);
      Sendable<typename std::remove_cvref_t<T>,
               typename std::remove_cvref_t<I>...>::Close(obj, table, info...);
    };

/**
 * Specifies that a type is capable of sendable serialization and
 * deserialization when wrapped in a std::shared_ptr. This works with any type.
 *
 * Implementations must define a template specialization for wpi::Sendable with
 * T being the type that is being serialized/deserialized, with the following
 * static members (as enforced by this concept):
 * - std::string_view GetTypeString(): function that returns the dashboard type
 *   string
 * - void Init(std::shared_ptr<T> obj, SendableTable& table)
 * - void Close(std::shared_ptr<T> obj, SendableTable& table)
 *
 * If possible, the GetTypeString() function should be marked constexpr.
 * GetTypeString() may return a type other than std::string_view, as long as the
 * return value is convertible to std::string_view.
 */
template <typename T, typename... I>
concept SendableSerializableSharedPointer =
    requires(std::shared_ptr<T> obj, SendableTable& table, const I&... info) {
      typename Sendable<typename std::remove_cvref_t<T>,
                        typename std::remove_cvref_t<I>...>;
      {
        Sendable<typename std::remove_cvref_t<T>,
                 typename std::remove_cvref_t<I>...>::GetTypeString(info...)
      } -> std::convertible_to<std::string_view>;
      Sendable<typename std::remove_cvref_t<T>,
               typename std::remove_cvref_t<I>...>::Init(obj, table, info...);
      Sendable<typename std::remove_cvref_t<T>,
               typename std::remove_cvref_t<I>...>::Close(obj, table, info...);
    };

/**
 * Specifies that a type is capable of sendable serialization and
 * deserialization as either a raw pointer type or via std::shared_ptr.
 */
template <typename T, typename... I>
concept SendableSerializable = SendableSerializableMoveTracked<T, I...> ||
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

template <typename T, typename... I>
  requires SendableSerializableMoveTracked<T, I...>
inline void InitSendable(T* obj, SendableTable& table, const I&... info) {
  using S = Sendable<T, typename std::remove_cvref_t<I>...>;
  S::Init(obj, table, info...);
}

template <typename T, typename... I>
  requires SendableSerializable<T, I...>
inline void InitSendable(std::shared_ptr<T> obj, SendableTable& table,
                         const I&... info) {
  using S = Sendable<T, typename std::remove_cvref_t<I>...>;
  if constexpr (SendableSerializableSharedPointer<T, I...>) {
    S::Init(std::move(obj), table, info...);
  } else if constexpr (SendableSerializableMoveTracked<T, I...>) {
    S::Init(obj.get(), table, info...);
  }
}

template <typename T, typename... I>
  requires SendableSerializableMoveTracked<T, I...>
inline void CloseSendable(T* obj, SendableTable& table, const I&... info) {
  using S = Sendable<T, typename std::remove_cvref_t<I>...>;
  S::Close(obj, table, info...);
}

template <typename T, typename... I>
  requires SendableSerializable<T, I...>
inline void CloseSendable(std::shared_ptr<T> obj, SendableTable& table,
                          const I&... info) {
  using S = Sendable<T, typename std::remove_cvref_t<I>...>;
  if constexpr (SendableSerializableSharedPointer<T, I...>) {
    S::Close(std::move(obj), table, info...);
  } else if constexpr (SendableSerializableMoveTracked<T, I...>) {
    S::Close(obj.get(), table, info...);
  }
}

}  // namespace wpi2
