// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/DataSource.h"

#include <string>

#include <fmt/format.h>

#include "glass/ContextInternal.h"
#include "imgui.h"

using namespace glass;

wpi::sig::Signal<const char*, DataSource*> DataSource::sourceCreated;

std::string glass::MakeSourceId(std::string_view id, int index) {
  return fmt::format("{}[{}]", id, index);
}

std::string glass::MakeSourceId(std::string_view id, int index, int index2) {
  return fmt::format("{}[{},{}]", id, index, index2);
}

DataSource::DataSource(Kind kind, std::string_view id)
    : m_id{id},
      m_name{gContext->sourceNameStorage.GetString(m_id)},
      m_kind{kind} {}

DataSource::~DataSource() {
  if (!gContext) {
    return;
  }
  gContext->sources.erase(m_id);
}

void DataSource::LabelText(const char* label, const char* fmt, ...) const {
  va_list args;
  va_start(args, fmt);
  LabelTextV(label, fmt, args);
  va_end(args);
}

// Add a label+text combo aligned to other label+value widgets
void DataSource::LabelTextV(const char* label, const char* fmt,
                            va_list args) const {
  ImGui::PushID(label);
  ImGui::LabelTextV("##input", fmt, args);  // NOLINT
  ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
  ImGui::Selectable(label);
  ImGui::PopID();
  EmitDrag();
}

bool DataSource::Combo(const char* label, int* current_item,
                       const char* const items[], int items_count,
                       int popup_max_height_in_items) const {
  ImGui::PushID(label);
  bool rv = ImGui::Combo("##input", current_item, items, items_count,
                         popup_max_height_in_items);
  ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
  ImGui::Selectable(label);
  EmitDrag();
  ImGui::PopID();
  return rv;
}

bool DataSource::SliderFloat(const char* label, float* v, float v_min,
                             float v_max, const char* format,
                             float power) const {
  ImGui::PushID(label);
  bool rv = ImGui::SliderFloat("##input", v, v_min, v_max, format, power);
  ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
  ImGui::Selectable(label);
  EmitDrag();
  ImGui::PopID();
  return rv;
}

bool DataSource::InputDouble(const char* label, double* v, double step,
                             double step_fast, const char* format,
                             ImGuiInputTextFlags flags) const {
  ImGui::PushID(label);
  bool rv = ImGui::InputDouble("##input", v, step, step_fast, format, flags);
  ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
  ImGui::Selectable(label);
  EmitDrag();
  ImGui::PopID();
  return rv;
}

bool DataSource::InputInt(const char* label, int* v, int step, int step_fast,
                          ImGuiInputTextFlags flags) const {
  ImGui::PushID(label);
  bool rv = ImGui::InputInt("##input", v, step, step_fast, flags);
  ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
  ImGui::Selectable(label);
  EmitDrag();
  ImGui::PopID();
  return rv;
}

void DataSource::EmitDrag(ImGuiDragDropFlags flags) const {
  if (ImGui::BeginDragDropSource(flags)) {
    SetDragDropPayload();
    const char* name = GetName().c_str();
    ImGui::TextUnformatted(name[0] == '\0' ? m_id.c_str() : name);
    ImGui::EndDragDropSource();
  }
}

DataSource* DataSource::Find(std::string_view id) {
  auto it = gContext->sources.find(id);
  if (it == gContext->sources.end()) {
    return nullptr;
  }
  return it->second;
}

void DataSource::Register() {
  gContext->sources.insert_or_assign(m_id, this);
  sourceCreated(m_id.c_str(), this);
}

BooleanSource* BooleanSource::AcceptDragDropPayload(ImGuiDragDropFlags flags) {
  if (auto payload = ImGui::AcceptDragDropPayload("BooleanSource", flags)) {
    return *static_cast<BooleanSource**>(payload->Data);
  }
  return nullptr;
}

void BooleanSource::SetDragDropPayload() const {
  auto self = this;
  ImGui::SetDragDropPayload("BooleanSource", &self, sizeof(self));  // NOLINT
}

DoubleSource* DoubleSource::AcceptDragDropPayload(ImGuiDragDropFlags flags) {
  if (auto payload = ImGui::AcceptDragDropPayload("DoubleSource", flags)) {
    return *static_cast<DoubleSource**>(payload->Data);
  }
  return nullptr;
}

void DoubleSource::SetDragDropPayload() const {
  auto self = this;
  ImGui::SetDragDropPayload("DoubleSource", &self, sizeof(self));  // NOLINT
}

FloatSource* FloatSource::AcceptDragDropPayload(ImGuiDragDropFlags flags) {
  if (auto payload = ImGui::AcceptDragDropPayload("FloatSource", flags)) {
    return *static_cast<FloatSource**>(payload->Data);
  }
  return nullptr;
}

void FloatSource::SetDragDropPayload() const {
  auto self = this;
  ImGui::SetDragDropPayload("FloatSource", &self, sizeof(self));  // NOLINT
}

IntegerSource* IntegerSource::AcceptDragDropPayload(ImGuiDragDropFlags flags) {
  if (auto payload = ImGui::AcceptDragDropPayload("IntegerSource", flags)) {
    return *static_cast<IntegerSource**>(payload->Data);
  }
  return nullptr;
}

void IntegerSource::SetDragDropPayload() const {
  auto self = this;
  ImGui::SetDragDropPayload("IntegerSource", &self, sizeof(self));  // NOLINT
}

StringSource* StringSource::AcceptDragDropPayload(ImGuiDragDropFlags flags) {
  if (auto payload = ImGui::AcceptDragDropPayload("StringSource", flags)) {
    return *static_cast<StringSource**>(payload->Data);
  }
  return nullptr;
}

void StringSource::SetDragDropPayload() const {
  auto self = this;
  ImGui::SetDragDropPayload("StringSource", &self, sizeof(self));  // NOLINT
}
