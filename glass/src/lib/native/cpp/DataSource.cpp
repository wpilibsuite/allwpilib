/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "glass/DataSource.h"

#include "glass/ContextInternal.h"

using namespace glass;

wpi::sig::Signal<const char*, DataSource*> DataSource::sourceCreated;

DataSource::DataSource(const wpi::Twine& id) : m_id{id.str()} {
  auto it = gContext->sources.try_emplace(m_id, this);
  auto& srcName = it.first->getValue();
  m_name = srcName.name.get();
  if (!srcName.source) srcName.source = this;
  sourceCreated(m_id.c_str(), this);
}

DataSource::DataSource(const wpi::Twine& id, int index)
    : DataSource{id + wpi::Twine('[') + wpi::Twine(index) + wpi::Twine(']')} {}

DataSource::DataSource(const wpi::Twine& id, int index, int index2)
    : DataSource{id + wpi::Twine('[') + wpi::Twine(index) + wpi::Twine(',') +
                 wpi::Twine(index2) + wpi::Twine(']')} {}

DataSource::~DataSource() {
  if (!gContext) return;
  auto it = gContext->sources.find(m_id);
  if (it == gContext->sources.end()) return;
  auto& srcName = it->getValue();
  if (srcName.source == this) srcName.source = nullptr;
}

void DataSource::SetName(const wpi::Twine& name) { m_name->SetName(name); }

const char* DataSource::GetName() const { return m_name->GetName(); }

void DataSource::PushEditNameId(int index) { m_name->PushEditNameId(index); }

void DataSource::PushEditNameId(const char* name) {
  m_name->PushEditNameId(name);
}

bool DataSource::PopupEditName(int index) {
  return m_name->PopupEditName(index);
}

bool DataSource::PopupEditName(const char* name) {
  return m_name->PopupEditName(name);
}

bool DataSource::InputTextName(const char* label_id,
                               ImGuiInputTextFlags flags) {
  return m_name->InputTextName(label_id, flags);
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
  ImGui::LabelTextV("##input", fmt, args);
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
    ImGui::SetDragDropPayload("DataSource", &self, sizeof(self));
    const char* name = GetName();
    ImGui::TextUnformatted(name[0] == '\0' ? m_id.c_str() : name);
    ImGui::EndDragDropSource();
  }
}

DataSource* DataSource::Find(wpi::StringRef id) {
  auto it = gContext->sources.find(id);
  if (it == gContext->sources.end()) return nullptr;
  return it->getValue().source;
}
