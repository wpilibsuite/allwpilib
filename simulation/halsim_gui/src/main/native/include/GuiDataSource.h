/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <atomic>
#include <string>

#include <imgui.h>
#include <wpi/Signal.h>
#include <wpi/StringRef.h>
#include <wpi/Twine.h>
#include <wpi/spinlock.h>

namespace halsimgui {

/**
 * A data source.
 */
class GuiDataSource {
 public:
  explicit GuiDataSource(const wpi::Twine& id);
  GuiDataSource(const wpi::Twine& id, int index);
  GuiDataSource(const wpi::Twine& id, int index, int index2);
  ~GuiDataSource();

  GuiDataSource(const GuiDataSource&) = delete;
  GuiDataSource& operator=(const GuiDataSource&) = delete;

  const char* GetId() const { return m_id.c_str(); }

  void SetName(const wpi::Twine& name) { m_name = name.str(); }
  const char* GetName() const { return m_name.c_str(); }

  void SetDigital(bool digital) { m_digital = digital; }
  bool IsDigital() const { return m_digital; }

  void SetValue(double value) {
    m_value = value;
    valueChanged(value);
  }
  double GetValue() const { return m_value; }

  // drag source helpers
  void LabelText(const char* label, const char* fmt, ...) const;
  void LabelTextV(const char* label, const char* fmt, va_list args) const;
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

  wpi::sig::SignalBase<wpi::spinlock, double> valueChanged;

  static GuiDataSource* Find(wpi::StringRef id);

  static wpi::sig::Signal<const char*, GuiDataSource*> sourceCreated;

 private:
  std::string m_id;
  std::string m_name;
  bool m_digital = false;
  std::atomic<double> m_value = 0;
};

}  // namespace halsimgui

#define HALSIMGUI_DATASOURCE(cbname, id, TYPE, vtype)                         \
  class cbname##Source : public ::halsimgui::GuiDataSource {                  \
   public:                                                                    \
    cbname##Source()                                                          \
        : GuiDataSource(id),                                                  \
          m_callback{                                                         \
              HALSIM_Register##cbname##Callback(CallbackFunc, this, true)} {  \
      SetDigital(HAL_##TYPE == HAL_BOOLEAN);                                  \
    }                                                                         \
                                                                              \
    ~cbname##Source() {                                                       \
      if (m_callback != 0) HALSIM_Cancel##cbname##Callback(m_callback);       \
    }                                                                         \
                                                                              \
   private:                                                                   \
    static void CallbackFunc(const char*, void* param,                        \
                             const HAL_Value* value) {                        \
      if (value->type == HAL_##TYPE)                                          \
        static_cast<cbname##Source*>(param)->SetValue(value->data.v_##vtype); \
    }                                                                         \
                                                                              \
    int32_t m_callback;                                                       \
  }

#define HALSIMGUI_DATASOURCE_BOOLEAN(cbname, id) \
  HALSIMGUI_DATASOURCE(cbname, id, BOOLEAN, boolean)

#define HALSIMGUI_DATASOURCE_DOUBLE(cbname, id) \
  HALSIMGUI_DATASOURCE(cbname, id, DOUBLE, double)

#define HALSIMGUI_DATASOURCE_INT(cbname, id) \
  HALSIMGUI_DATASOURCE(cbname, id, INT, int)

#define HALSIMGUI_DATASOURCE_INDEXED(cbname, id, TYPE, vtype)                 \
  class cbname##Source : public ::halsimgui::GuiDataSource {                  \
   public:                                                                    \
    explicit cbname##Source(int32_t index, int channel = -1)                  \
        : GuiDataSource(id, channel < 0 ? index : channel),                   \
          m_index{index},                                                     \
          m_channel{channel < 0 ? index : channel},                           \
          m_callback{HALSIM_Register##cbname##Callback(index, CallbackFunc,   \
                                                       this, true)} {         \
      SetDigital(HAL_##TYPE == HAL_BOOLEAN);                                  \
    }                                                                         \
                                                                              \
    ~cbname##Source() {                                                       \
      if (m_callback != 0)                                                    \
        HALSIM_Cancel##cbname##Callback(m_index, m_callback);                 \
    }                                                                         \
                                                                              \
    int32_t GetIndex() const { return m_index; }                              \
                                                                              \
    int GetChannel() const { return m_channel; }                              \
                                                                              \
   private:                                                                   \
    static void CallbackFunc(const char*, void* param,                        \
                             const HAL_Value* value) {                        \
      if (value->type == HAL_##TYPE)                                          \
        static_cast<cbname##Source*>(param)->SetValue(value->data.v_##vtype); \
    }                                                                         \
                                                                              \
    int32_t m_index;                                                          \
    int m_channel;                                                            \
    int32_t m_callback;                                                       \
  }

#define HALSIMGUI_DATASOURCE_BOOLEAN_INDEXED(cbname, id) \
  HALSIMGUI_DATASOURCE_INDEXED(cbname, id, BOOLEAN, boolean)

#define HALSIMGUI_DATASOURCE_DOUBLE_INDEXED(cbname, id) \
  HALSIMGUI_DATASOURCE_INDEXED(cbname, id, DOUBLE, double)

#define HALSIMGUI_DATASOURCE_INDEXED2(cbname, id, TYPE, vtype)                \
  class cbname##Source : public ::halsimgui::GuiDataSource {                  \
   public:                                                                    \
    explicit cbname##Source(int32_t index, int32_t channel)                   \
        : GuiDataSource(id, index, channel),                                  \
          m_index{index},                                                     \
          m_channel{channel},                                                 \
          m_callback{HALSIM_Register##cbname##Callback(                       \
              index, channel, CallbackFunc, this, true)} {                    \
      SetDigital(HAL_##TYPE == HAL_BOOLEAN);                                  \
    }                                                                         \
                                                                              \
    ~cbname##Source() {                                                       \
      if (m_callback != 0)                                                    \
        HALSIM_Cancel##cbname##Callback(m_index, m_channel, m_callback);      \
    }                                                                         \
                                                                              \
    int32_t GetIndex() const { return m_index; }                              \
                                                                              \
    int32_t GetChannel() const { return m_channel; }                          \
                                                                              \
   private:                                                                   \
    static void CallbackFunc(const char*, void* param,                        \
                             const HAL_Value* value) {                        \
      if (value->type == HAL_##TYPE)                                          \
        static_cast<cbname##Source*>(param)->SetValue(value->data.v_##vtype); \
    }                                                                         \
                                                                              \
    int32_t m_index;                                                          \
    int32_t m_channel;                                                        \
    int32_t m_callback;                                                       \
  }

#define HALSIMGUI_DATASOURCE_BOOLEAN_INDEXED2(cbname, id) \
  HALSIMGUI_DATASOURCE_INDEXED2(cbname, id, BOOLEAN, boolean)

#define HALSIMGUI_DATASOURCE_DOUBLE_INDEXED2(cbname, id) \
  HALSIMGUI_DATASOURCE_INDEXED2(cbname, id, DOUBLE, double)
