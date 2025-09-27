// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <functional>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

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
  struct Value {
    enum Type {
      kNone,
      kInt,
      kInt64,
      kBool,
      kFloat,
      kDouble,
      kString,
      kChild,
      kIntArray,
      kInt64Array,
      kBoolArray,
      kFloatArray,
      kDoubleArray,
      kStringArray,
      kChildArray
    };

    Value() = default;
    explicit Value(Type type) : type{type} {}
    Value(const Value&) = delete;
    Value& operator=(const Value&) = delete;
    ~Value() { Reset(kNone); }

    Type type = kNone;
    union {
      int intVal;
      int64_t int64Val;
      bool boolVal;
      float floatVal;
      double doubleVal;
      Storage* child;
      std::vector<int>* intArray;
      std::vector<int64_t>* int64Array;
      std::vector<int>* boolArray;
      std::vector<float>* floatArray;
      std::vector<double>* doubleArray;
      std::vector<std::string>* stringArray;
      std::vector<std::unique_ptr<Storage>>* childArray;
    };
    std::string stringVal;

    union {
      int intDefault;
      int64_t int64Default;
      bool boolDefault;
      float floatDefault;
      double doubleDefault;
      // pointers may be nullptr to indicate empty
      std::vector<int>* intArrayDefault;
      std::vector<int64_t>* int64ArrayDefault;
      std::vector<int>* boolArrayDefault;
      std::vector<float>* floatArrayDefault;
      std::vector<double>* doubleArrayDefault;
      std::vector<std::string>* stringArrayDefault;
    };
    std::string stringDefault;

    bool hasDefault = false;

    void Reset(Type newType);
  };

  using ValueMap = wpi::StringMap<std::unique_ptr<Value>>;
  template <typename Iterator>
  using ChildIterator = detail::ChildIterator<Iterator>;

  // The "Read" functions don't create or overwrite the value
  int ReadInt(std::string_view key, int defaultVal = 0) const;
  int64_t ReadInt64(std::string_view key, int64_t defaultVal = 0) const;
  bool ReadBool(std::string_view key, bool defaultVal = false) const;
  float ReadFloat(std::string_view key, float defaultVal = 0.0f) const;
  double ReadDouble(std::string_view key, double defaultVal = 0.0) const;
  std::string ReadString(std::string_view key,
                         std::string_view defaultVal = {}) const;

  void SetInt(std::string_view key, int val);
  void SetInt64(std::string_view key, int64_t val);
  void SetBool(std::string_view key, bool val);
  void SetFloat(std::string_view key, float val);
  void SetDouble(std::string_view key, double val);
  void SetString(std::string_view key, std::string_view val);

  // The "Get" functions create or override the current value type.
  // If the value is not set, it is set to the provided default.
  int& GetInt(std::string_view key, int defaultVal = 0);
  int64_t& GetInt64(std::string_view key, int64_t defaultVal = 0);
  bool& GetBool(std::string_view key, bool defaultVal = false);
  float& GetFloat(std::string_view key, float defaultVal = 0.0f);
  double& GetDouble(std::string_view key, double defaultVal = 0.0);
  std::string& GetString(std::string_view key,
                         std::string_view defaultVal = {});

  std::vector<int>& GetIntArray(std::string_view key,
                                std::span<const int> defaultVal = {});
  std::vector<int64_t>& GetInt64Array(std::string_view key,
                                      std::span<const int64_t> defaultVal = {});
  std::vector<int>& GetBoolArray(std::string_view key,
                                 std::span<const int> defaultVal = {});
  std::vector<float>& GetFloatArray(std::string_view key,
                                    std::span<const float> defaultVal = {});
  std::vector<double>& GetDoubleArray(std::string_view key,
                                      std::span<const double> defaultVal = {});
  std::vector<std::string>& GetStringArray(
      std::string_view key, std::span<const std::string> defaultVal = {});
  std::vector<std::unique_ptr<Storage>>& GetChildArray(std::string_view key);

  Value* FindValue(std::string_view key);
  Value& GetValue(std::string_view key);
  Storage& GetChild(std::string_view label_id);

  void SetData(std::shared_ptr<void>&& data) { m_data = std::move(data); }

  template <typename T>
  T* GetData() const {
    return static_cast<T*>(m_data.get());
  }

  Storage() = default;
  Storage(const Storage&) = delete;
  Storage& operator=(const Storage&) = delete;

  void Insert(std::string_view key, std::unique_ptr<Value> value) {
    m_values[key] = std::move(value);
  }

  std::unique_ptr<Value> Erase(std::string_view key);

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
  void Clear();

  /**
   * Clear values (and values of children) only (set to default).  Does not
   * call custom clear function.
   */
  void ClearValues();

  /**
   * Apply settings (called after all settings have been loaded).  Calls
   * custom apply function (if set), otherwise calls ApplyChildren().
   */
  void Apply();

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
    while (anchor != end && anchor->second->type != Storage::Value::kChild) {
      ++anchor;
    }
  }

  /// dereference operator (needed for range-based for)
  ChildIterator& operator*() { return *this; }

  /// increment operator (needed for range-based for)
  ChildIterator& operator++() {
    ++anchor;
    while (anchor != end && anchor->second->type != Storage::Value::kChild) {
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
  Storage& value() const { return *anchor->second->child; }
};

}  // namespace detail

inline auto Storage::GetChildren()
    -> wpi::iterator_range<ChildIterator<ValueMap::iterator>> {
  return {{m_values.begin(), m_values.end()}, {m_values.end(), m_values.end()}};
}

}  // namespace glass
