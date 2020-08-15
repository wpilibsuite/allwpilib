/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "GuiDataSource.h"

#include <wpi/StringMap.h>

using namespace halsimgui;

wpi::sig::Signal<const char*, GuiDataSource*> GuiDataSource::sourceCreated;

static wpi::StringMap<GuiDataSource*> gSources;

GuiDataSource::GuiDataSource(const wpi::Twine& id) : m_id{id.str()} {
  gSources.try_emplace(m_id, this);
  sourceCreated(m_id.c_str(), this);
}

GuiDataSource::GuiDataSource(const wpi::Twine& id, int index)
    : GuiDataSource{id + wpi::Twine('[') + wpi::Twine(index) +
                    wpi::Twine(']')} {}

GuiDataSource::GuiDataSource(const wpi::Twine& id, int index, int index2)
    : GuiDataSource{id + wpi::Twine('[') + wpi::Twine(index) + wpi::Twine(',') +
                    wpi::Twine(index2) + wpi::Twine(']')} {}

GuiDataSource::~GuiDataSource() {
  auto it = gSources.find(m_id);
  if (it == gSources.end()) return;
  if (it->getValue() == this) gSources.erase(it);
}

void GuiDataSource::LabelText(const char* label, const char* fmt, ...) const {
  va_list args;
  va_start(args, fmt);
  LabelTextV(label, fmt, args);
  va_end(args);
}

// Add a label+text combo aligned to other label+value widgets
void GuiDataSource::LabelTextV(const char* label, const char* fmt,
                               va_list args) const {
  ImGui::PushID(label);
  ImGui::LabelTextV("##input", fmt, args);
  ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
  ImGui::Selectable(label);
  ImGui::PopID();
  EmitDrag();
}

bool GuiDataSource::Combo(const char* label, int* current_item,
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

bool GuiDataSource::SliderFloat(const char* label, float* v, float v_min,
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

bool GuiDataSource::InputDouble(const char* label, double* v, double step,
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

bool GuiDataSource::InputInt(const char* label, int* v, int step, int step_fast,
                             ImGuiInputTextFlags flags) const {
  ImGui::PushID(label);
  bool rv = ImGui::InputInt("##input", v, step, step_fast, flags);
  ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
  ImGui::Selectable(label);
  EmitDrag();
  ImGui::PopID();
  return rv;
}

void GuiDataSource::EmitDrag(ImGuiDragDropFlags flags) const {
  if (ImGui::BeginDragDropSource(flags)) {
    auto self = this;
    ImGui::SetDragDropPayload("DataSource", &self, sizeof(self));
    ImGui::TextUnformatted(m_name.empty() ? m_id.c_str() : m_name.c_str());
    ImGui::EndDragDropSource();
  }
}

GuiDataSource* GuiDataSource::Find(wpi::StringRef id) {
  auto it = gSources.find(id);
  if (it == gSources.end()) return nullptr;
  return it->getValue();
}
