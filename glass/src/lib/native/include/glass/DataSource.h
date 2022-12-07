// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <string>
#include <string_view>

#include <imgui.h>
#include <wpi/Signal.h>

namespace glass {

/**
 * A data source for numeric/boolean data.
 */
class DataSource {
 public:
  explicit DataSource(std::string_view id);
  DataSource(std::string_view id, int index);
  DataSource(std::string_view id, int index, int index2);
  virtual ~DataSource();

  DataSource(const DataSource&) = delete;
  DataSource& operator=(const DataSource&) = delete;

  const char* GetId() const { return m_id.c_str(); }

  void SetName(std::string_view name) { m_name = name; }
  std::string& GetName() { return m_name; }
  const std::string& GetName() const { return m_name; }

  void SetDigital(bool digital) { m_digital = digital; }
  bool IsDigital() const { return m_digital; }

  void SetValue(double value, int64_t time = 0) {
    m_value = value;
    m_valueTime = time;
    valueChanged(value, time);
  }
  double GetValue() const { return m_value; }
  int64_t GetValueTime() const { return m_valueTime; }

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

  wpi::sig::Signal<double, int64_t> valueChanged;

  static DataSource* Find(std::string_view id);

  static wpi::sig::Signal<const char*, DataSource*> sourceCreated;

 private:
  std::string m_id;
  std::string& m_name;
  bool m_digital = false;
  double m_value = 0;
  int64_t m_valueTime = 0;
};

}  // namespace glass
