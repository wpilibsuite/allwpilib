// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <concepts>
#include <cstdio>
#include <string>
#include <string_view>
#include <utility>

#include <imgui.h>
#include "wpi/util/Signal.h"
#include "wpi/util/SmallVector.hpp"
#include "wpi/util/spinlock.hpp"

namespace glass {

/**
 * A data source for numeric/boolean data.
 */
class DataSource {
 public:
  virtual ~DataSource();

  DataSource(const DataSource&) = delete;
  DataSource& operator=(const DataSource&) = delete;
  DataSource(DataSource&&) = delete;
  DataSource& operator=(DataSource&&) = delete;

  const char* GetId() const { return m_id.c_str(); }

  virtual const char* GetType() const = 0;

  void SetName(std::string_view name) { m_name = name; }
  std::string& GetName() { return m_name; }
  const std::string& GetName() const { return m_name; }

  int64_t GetValueTime() const {
    std::scoped_lock lock{m_valueMutex};
    return m_valueTime;
  }

  // drag source helpers
  void LabelText(const char* label, const char* fmt, ...) const IM_FMTARGS(3);
  void LabelTextV(const char* label, const char* fmt, va_list args) const
      IM_FMTLIST(3);
  bool Combo(const char* label, int* current_item, const char* const items[],
             int items_count, int popup_max_height_in_items = -1) const;
  bool SliderFloat(const char* label, float* v, float v_min, float v_max,
                   const char* format = "%.3f", float power = 1.0f) const;
  bool InputDouble(const char* label, double* v, double step = 0.0,
                   double step_fast = 0.0, const char* format = "%.6f",
                   ImGuiInputTextFlags flags = 0) const;
  bool InputInt(const char* label, int* v, int step = 1, int step_fast = 100,
                ImGuiInputTextFlags flags = 0) const;
  void EmitDrag(ImGuiDragDropFlags flags = 0) const;

  static DataSource* Find(std::string_view id);

  static wpi::sig::Signal<const char*, DataSource*> sourceCreated;

 private:
  std::string m_id;
  std::string& m_name;

  static std::string& GetNameStorage(std::string_view id);

 protected:
  explicit DataSource(std::string id)
      : m_id{std::move(id)}, m_name{GetNameStorage(m_id)} {}

  void Register();

  virtual void DragDropTooltip() const;

  mutable wpi::spinlock m_valueMutex;
  int64_t m_valueTime = 0;
};

template <typename T>
  requires std::derived_from<T, DataSource> && requires {
    { T::kType } -> std::convertible_to<const char*>;
  }
static T* AcceptDragDropPayload(ImGuiDragDropFlags flags = 0) {
  char buf[32];
  std::snprintf(buf, sizeof(buf), "DataSource:%s", T::kType);
  if (auto payload = ImGui::AcceptDragDropPayload(buf, flags)) {
    return static_cast<T*>(*static_cast<DataSource**>(payload->Data));
  }
  return nullptr;
}

std::string MakeSourceId(std::string_view id, int index);
std::string MakeSourceId(std::string_view id, int index, int index2);

template <typename T>
class ValueSource : public DataSource {
 public:
  void SetValue(T value, int64_t time = 0) {
    std::scoped_lock lock{m_valueMutex};
    m_value = value;
    m_valueTime = time;
    valueChanged(value, time);
  }

  T GetValue() const {
    std::scoped_lock lock{m_valueMutex};
    return m_value;
  }

  wpi::sig::SignalBase<wpi::spinlock, T, int64_t> valueChanged;

 private:
  T m_value = 0;

 protected:
  explicit ValueSource(std::string id) : DataSource{std::move(id)} {}
};

class BooleanSource : public ValueSource<bool> {
 public:
  static constexpr const char* kType = "boolean";

  explicit BooleanSource(std::string id) : ValueSource{std::move(id)} {
    Register();
  }

  const char* GetType() const override;
};

class DoubleSource : public ValueSource<double> {
 public:
  static constexpr const char* kType = "boolean";

  explicit DoubleSource(std::string id) : ValueSource{std::move(id)} {
    Register();
  }

  const char* GetType() const override;
};

class FloatSource : public ValueSource<float> {
 public:
  static constexpr const char* kType = "float";

  explicit FloatSource(std::string id) : ValueSource{std::move(id)} {
    Register();
  }

  const char* GetType() const override;
};

class IntegerSource : public ValueSource<int64_t> {
 public:
  static constexpr const char* kType = "integer";

  explicit IntegerSource(std::string id) : ValueSource{std::move(id)} {
    Register();
  }

  const char* GetType() const override;
};

class StringSource : public DataSource {
 public:
  static constexpr const char* kType = "string";

  explicit StringSource(std::string id) : DataSource{std::move(id)} {
    Register();
  }

  const char* GetType() const override;

  void SetValue(std::string_view value, int64_t time = 0) {
    std::scoped_lock lock{m_valueMutex};
    m_value = value;
    m_valueTime = time;
    valueChanged(m_value, time);
  }

  void SetValue(std::string&& value, int64_t time = 0) {
    std::scoped_lock lock{m_valueMutex};
    m_value = std::move(value);
    m_valueTime = time;
    valueChanged(m_value, time);
  }

  void SetValue(const char* value, int64_t time = 0) {
    std::scoped_lock lock{m_valueMutex};
    m_value = value;
    m_valueTime = time;
    valueChanged(m_value, time);
  }

  std::string GetValue() const {
    std::scoped_lock lock{m_valueMutex};
    return m_value;
  }

  std::string_view GetValue(wpi::SmallVectorImpl<char>& buf) const {
    std::scoped_lock lock{m_valueMutex};
    buf.assign(m_value.begin(), m_value.end());
    return {buf.begin(), buf.end()};
  }

  wpi::sig::SignalBase<wpi::spinlock, std::string_view, int64_t> valueChanged;

 private:
  std::string m_value;
};

}  // namespace glass
