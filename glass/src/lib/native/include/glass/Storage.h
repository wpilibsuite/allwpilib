// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <functional>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>

#include <wpi/StringExtras.h>
#include <wpi/StringMap.h>
#include <wpi/iterator_range.h>
#include <wpi/json_fwd.h>

namespace glass {

namespace detail {
template <typename IteratorType>
class ChildIterator;
}  // namespace detail

/**
 * Storage provides both persistent and non-persistent nested key/value storage
 * for widgets.
 *
 * Keys are always strings.  The storage also provides non-persistent arbitrary
 * data storage (via std::shared_ptr<void>).
 *
 * Storage is automatically indexed internally by the ID stack.  Note it is
 * necessary to use the glass wrappers for PushID et al to preserve naming in
 * the save file (unnamed values are still stored, but this is non-ideal for
 * users trying to hand-edit the save file).
 */
class Storage {
 public:
  using Child = std::shared_ptr<Storage>;

  class Value {
   public:
    using ValueData =
        std::variant<std::monostate, int, bool, float, double, std::string,
                     Child, std::vector<int>, std::vector<float>,
                     std::vector<double>, std::vector<std::string>,
                     std::vector<Child>>;

    Value() = default;
    explicit Value(ValueData&& data) : data{std::move(data)} {}

    template <typename T, typename R = T>
    R Read(T&& defaultVal)
      requires(std::convertible_to<T, R> && std::assignable_from<ValueData&, R>)
    {
      if (auto d = std::get_if<R>(&data)) {
        return *d;
      }
      if (auto d = Convert<R>(&data)) {
        Convert<R>(&dataDefault);
        return *d;
      }
      return Set<R>(std::forward<T>(defaultVal));
    }

    template <typename T, typename... Args>
    T& Set(Args&&... args)
      requires(std::assignable_from<ValueData&, T>)
    {
      dataDefault.emplace<T>(args...);
      return data.emplace<T>(std::forward<Args>(args)...);
    }

    ValueData data;
    ValueData dataDefault;
  };

  using ValueMap = wpi::StringMap<Value>;
  template <typename Iterator>
  using ChildIterator = detail::ChildIterator<Iterator>;

  // The "Read" functions don't create or overwrite the value
  template <typename T, typename R = T>
  R Read(std::string_view key, T&& defaultVal = {}) const
    requires(std::convertible_to<T, R> &&
             std::assignable_from<Value::ValueData&, R>)
  {
    if (auto value = FindValue(key)) {
      return value->Read<R>(std::forward<T>(defaultVal));
    } else {
      return defaultVal;
    }
  }

  template <typename T>
  void Set(std::string_view key, T&& val)
    requires(std::assignable_from<Value::ValueData&, T>)
  {
    GetValue(key).Set<T>(std::forward<T>(val));
  }

  template <typename T>
  void Set(std::string_view key, const T& val)
    requires(std::assignable_from<Value::ValueData&, T>)
  {
    GetValue(key).Set<T>(val);
  }

  template <typename T>
  void Set(std::string_view key, std::span<const typename T::value_type> val)
    requires(std::assignable_from<Value::ValueData&, T>)
  {
    GetValue(key).Set<T>(val.begin(), val.end());
  }

  void Set(std::string_view key, std::string_view val) {
    GetValue(key).Set<std::string>(std::string{val});
  }

  // The "Get" functions create or override the current value type.
  // If the value is not set, it is set to the provided default.
  template <typename T>
  T& Get(std::string_view key, T&& defaultVal = {})
    requires(!std::same_as<T, Child> &&
             std::assignable_from<Value::ValueData&, T>)
  {
    Value& value = GetValue(key);
    if (auto data = std::get_if<T>(&value.data)) {
      return *data;
    }
    return value.Set<T>(defaultVal);
  }

  template <typename T>
  T& Get(std::string_view key, const T& defaultVal)
    requires(!std::same_as<T, Child> &&
             std::assignable_from<Value::ValueData&, T>)
  {
    Value& value = GetValue(key);
    if (auto data = std::get_if<T>(&value.data)) {
      return *data;
    }
    return value.Set<T>(defaultVal);
  }

  template <typename T>
  T& Get(std::string_view key,
         std::span<const typename T::value_type> defaultVal)
    requires(!std::same_as<T, Child> &&
             std::assignable_from<Value::ValueData&, T>)
  {
    Value& value = GetValue(key);
    if (auto data = std::get_if<T>(&value.data)) {
      return *data;
    }
    return value.Set<T>(defaultVal.begin(), defaultVal.end());
  }

  std::string& Get(std::string_view key, std::string_view defaultVal) {
    Value& value = GetValue(key);
    if (auto data = std::get_if<std::string>(&value.data)) {
      return *data;
    }
    return value.Set<std::string>(defaultVal);
  }

  Value* FindValue(std::string_view key) const {
    auto it = m_values.find(key);
    if (it == m_values.end()) {
      return nullptr;
    }
    return &it->second;
  }

  Value& GetValue(std::string_view key) {
    return m_values.try_emplace(key).first->second;
  }
  Storage& GetChild(std::string_view label_id);

  std::vector<Child>& GetChildArray(std::string_view key) {
    return Get<std::vector<Child>>(key);
  }

  void SetData(std::shared_ptr<void>&& data) { m_data = std::move(data); }

  template <typename T>
  T* GetData() const {
    return static_cast<T*>(m_data.get());
  }

  template <typename T, typename... Args>
  T& GetOrNewData(Args&&... args) {
    if (!m_data) {
      m_data = std::make_shared<T>(std::forward<Args>(args)...);
    }
    return *static_cast<T*>(m_data.get());
  }

  Storage() = default;
  Storage(const Storage&) = delete;
  Storage& operator=(const Storage&) = delete;

  void Insert(std::string_view key, Value&& value) {
    m_values.try_emplace(std::string{key}, std::move(value));
  }

  void Erase(std::string_view key) { m_values.erase(key); }

  void EraseAll() { m_values.clear(); }

  ValueMap& GetValues() { return m_values; }
  const ValueMap& GetValues() const { return m_values; }

  wpi::iterator_range<ChildIterator<ValueMap::iterator>> GetChildren();

  /**
   * Erases all children (at top level).
   */
  void EraseChildren();

  bool FromJson(const wpi::json& json, const char* filename);
  wpi::json ToJson() const;

  /**
   * Clear settings (set to default).  Calls custom clear function (if set),
   * otherwise calls ClearValues().
   */
  void Clear() {
    if (m_clear) {
      m_clear();
    } else {
      ClearValues();
    }
  }

  /**
   * Clear values (and values of children) only (set to default).  Does not
   * call custom clear function.
   */
  void ClearValues();

  /**
   * Apply settings (called after all settings have been loaded).  Calls
   * custom apply function (if set), otherwise calls ApplyChildren().
   */
  void Apply() {
    if (m_apply) {
      m_apply();
    } else {
      ApplyChildren();
    }
  }

  /**
   * Apply settings to children.  Does not call custom apply function.
   */
  void ApplyChildren();

  /**
   * Sets custom JSON handlers (replaces FromJson and ToJson).
   *
   * @param fromJson replacement for FromJson()
   * @param toJson replacement for ToJson()
   */
  void SetCustomJson(
      std::function<bool(const wpi::json& json, const char* filename)> fromJson,
      std::function<wpi::json()> toJson) {
    m_fromJson = std::move(fromJson);
    m_toJson = std::move(toJson);
  }

  void SetCustomClear(std::function<void()> clear) {
    m_clear = std::move(clear);
  }

  void SetCustomApply(std::function<void()> apply) {
    m_apply = std::move(apply);
  }

 private:
  mutable ValueMap m_values;
  std::shared_ptr<void> m_data;
  std::function<bool(const wpi::json& json, const char* filename)> m_fromJson;
  std::function<wpi::json()> m_toJson;
  std::function<void()> m_clear;
  std::function<void()> m_apply;

  template <typename T>
  static std::optional<T> ParseString(std::string_view str) {
    if constexpr (std::same_as<T, bool>) {
      if (str == "true") {
        return true;
      } else if (str == "false") {
        return false;
      } else if (auto val = wpi::parse_integer<int>(str, 10)) {
        return val.value() != 0;
      } else {
        return std::nullopt;
      }
    } else if constexpr (std::floating_point<T>) {
      return wpi::parse_float<T>(str);
    } else {
      return wpi::parse_integer<T>(str, 10);
    }
    return std::nullopt;
  }

  template <typename T>
  static T* Convert(Value::ValueData* data) {
    if constexpr (std::same_as<T, bool>) {
      if (auto d = std::get_if<std::string>(data)) {
        if (auto val = ParseString<T>(*d)) {
          return &data->emplace<T>(*val);
        }
      }
    } else if constexpr (std::same_as<T, int> || std::same_as<T, float> ||
                         std::same_as<T, double>) {
      if (auto d = std::get_if<int>(data)) {
        return &data->emplace<T>(*d);
      } else if (auto d = std::get_if<float>(data)) {
        return &data->emplace<T>(*d);
      } else if (auto d = std::get_if<double>(data)) {
        return &data->emplace<T>(*d);
      } else if (auto d = std::get_if<std::string>(data)) {
        if (auto val = ParseString<T>(*d)) {
          return &data->emplace<T>(*val);
        }
      }
    } else if constexpr (std::same_as<T, std::vector<int>> ||
                         std::same_as<T, std::vector<float>> ||
                         std::same_as<T, std::vector<double>>) {
      if (auto d = std::get_if<std::vector<int>>(data)) {
        return &data->emplace(T{d->begin(), d->end()});
      } else if (auto d = std::get_if<std::vector<float>>(data)) {
        return &data->emplace(T{d->begin(), d->end()});
      } else if (auto d = std::get_if<std::vector<double>>(data)) {
        return &data->emplace(T{d->begin(), d->end()});
      }
    }
    return nullptr;
  }
};

namespace detail {

/// proxy class for the GetChildren() function
template <typename IteratorType>
class ChildIterator {
 private:
  /// the iterator
  IteratorType anchor;
  IteratorType end;

 public:
  ChildIterator(IteratorType it, IteratorType end) noexcept
      : anchor(it), end(end) {
    while (anchor != end &&
           !std::holds_alternative<Storage::Child>(anchor->second.data)) {
      ++anchor;
    }
  }

  /// dereference operator (needed for range-based for)
  ChildIterator& operator*() { return *this; }

  /// increment operator (needed for range-based for)
  ChildIterator& operator++() {
    ++anchor;
    while (anchor != end &&
           !std::holds_alternative<Storage::Child>(anchor->second.data)) {
      ++anchor;
    }
    return *this;
  }

  /// inequality operator (needed for range-based for)
  bool operator!=(const ChildIterator& o) const noexcept {
    return anchor != o.anchor;
  }

  /// return key of the iterator
  std::string_view key() const { return anchor->first; }

  /// return value of the iterator
  Storage& value() const {
    return *std::get<Storage::Child>(anchor->second.data);
  }
};

}  // namespace detail

inline auto Storage::GetChildren()
    -> wpi::iterator_range<ChildIterator<ValueMap::iterator>> {
  return {{m_values.begin(), m_values.end()}, {m_values.end(), m_values.end()}};
}

}  // namespace glass
