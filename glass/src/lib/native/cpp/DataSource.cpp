// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/DataSource.h"

#include <fmt/format.h>

#include "glass/ContextInternal.h"

using namespace glass;

wpi::sig::Signal<const char*, DataSource*> DataSource::sourceCreated;

DataSource::DataSource(std::string_view id)
    : m_id{id}, m_name{gContext->sourceNameStorage.GetString(m_id)} {
  gContext->sources.try_emplace(m_id, this);
  sourceCreated(m_id.c_str(), this);
}

DataSource::DataSource(std::string_view id, int index)
    : DataSource{fmt::format("{}[{}]", id, index)} {}

DataSource::DataSource(std::string_view id, int index, int index2)
    : DataSource{fmt::format("{}[{},{}]", id, index, index2)} {}

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
    auto self = this;
    ImGui::SetDragDropPayload("DataSource", &self, sizeof(self));  // NOLINT
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
