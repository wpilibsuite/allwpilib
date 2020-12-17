/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "glass/Context.h"

#include <algorithm>
#include <cinttypes>
#include <cstdio>

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_stdlib.h>
#include <wpi/StringRef.h>
#include <wpi/timestamp.h>
#include <wpigui.h>

#include "glass/ContextInternal.h"

using namespace glass;

Context* glass::gContext;

static bool ConvertInt(Storage::Value* value) {
  value->type = Storage::Value::kInt;
  if (value->stringVal.empty()) {
    return false;
  } else {
    if (wpi::StringRef{value->stringVal}.getAsInteger(10, value->intVal))
      return false;
  }
  return true;
}

static bool ConvertInt64(Storage::Value* value) {
  value->type = Storage::Value::kInt64;
  if (value->stringVal.empty()) {
    return false;
  } else {
    if (wpi::StringRef{value->stringVal}.getAsInteger(10, value->int64Val))
      return false;
  }
  return true;
}

static bool ConvertBool(Storage::Value* value) {
  value->type = Storage::Value::kBool;
  if (value->stringVal.empty()) {
    return false;
  } else {
    int val;
    if (wpi::StringRef{value->stringVal}.getAsInteger(10, val)) {
      return false;
    }
    value->boolVal = (val != 0);
  }
  return true;
}

static bool ConvertFloat(Storage::Value* value) {
  value->type = Storage::Value::kFloat;
  if (value->stringVal.empty()) {
    return false;
  } else {
    if (std::sscanf(value->stringVal.c_str(), "%f", &value->floatVal) != 1)
      return false;
  }
  return true;
}

static bool ConvertDouble(Storage::Value* value) {
  value->type = Storage::Value::kDouble;
  if (value->stringVal.empty()) {
    return false;
  } else {
    if (std::sscanf(value->stringVal.c_str(), "%lf", &value->doubleVal) != 1)
      return false;
  }
  return true;
}

static void* GlassStorageReadOpen(ImGuiContext*, ImGuiSettingsHandler* handler,
                                  const char* name) {
  auto ctx = static_cast<Context*>(handler->UserData);
  auto& storage = ctx->storage[name];
  if (!storage) storage = std::make_unique<Storage>();
  return storage.get();
}

static void GlassStorageReadLine(ImGuiContext*, ImGuiSettingsHandler*,
                                 void* entry, const char* line) {
  auto storage = static_cast<Storage*>(entry);
  auto [key, val] = wpi::StringRef{line}.split('=');
  auto& keys = storage->GetKeys();
  auto& values = storage->GetValues();
  auto it = std::find(keys.begin(), keys.end(), key);
  if (it == keys.end()) {
    keys.emplace_back(key);
    values.emplace_back(std::make_unique<Storage::Value>(val));
  } else {
    auto& value = *values[it - keys.begin()];
    value.stringVal = val;
    switch (value.type) {
      case Storage::Value::kInt:
        ConvertInt(&value);
        break;
      case Storage::Value::kInt64:
        ConvertInt64(&value);
        break;
      case Storage::Value::kBool:
        ConvertBool(&value);
        break;
      case Storage::Value::kFloat:
        ConvertFloat(&value);
        break;
      case Storage::Value::kDouble:
        ConvertDouble(&value);
        break;
      default:
        break;
    }
  }
}

static void GlassStorageWriteAll(ImGuiContext*, ImGuiSettingsHandler* handler,
                                 ImGuiTextBuffer* out_buf) {
  auto ctx = static_cast<Context*>(handler->UserData);

  // sort for output
  std::vector<wpi::StringMapConstIterator<std::unique_ptr<Storage>>> sorted;
  for (auto it = ctx->storage.begin(); it != ctx->storage.end(); ++it) {
    sorted.emplace_back(it);
  }
  std::sort(sorted.begin(), sorted.end(), [](const auto& a, const auto& b) {
    return a->getKey() < b->getKey();
  });

  for (auto&& entryIt : sorted) {
    auto& entry = *entryIt;
    out_buf->append("[GlassStorage][");
    out_buf->append(entry.first().begin(), entry.first().end());
    out_buf->append("]\n");
    auto& keys = entry.second->GetKeys();
    auto& values = entry.second->GetValues();
    for (size_t i = 0; i < keys.size(); ++i) {
      out_buf->append(keys[i].data(), keys[i].data() + keys[i].size());
      out_buf->append("=");
      auto& value = *values[i];
      switch (value.type) {
        case Storage::Value::kInt:
          out_buf->appendf("%d\n", value.intVal);
          break;
        case Storage::Value::kInt64:
          out_buf->appendf("%" PRId64 "\n", value.int64Val);
          break;
        case Storage::Value::kBool:
          out_buf->appendf("%d\n", value.boolVal ? 1 : 0);
          break;
        case Storage::Value::kFloat:
          out_buf->appendf("%f\n", value.floatVal);
          break;
        case Storage::Value::kDouble:
          out_buf->appendf("%f\n", value.doubleVal);
          break;
        case Storage::Value::kNone:
        case Storage::Value::kString:
          out_buf->append(value.stringVal.data(),
                          value.stringVal.data() + value.stringVal.size());
          out_buf->append("\n");
          break;
      }
    }
    out_buf->append("\n");
  }
}

static void Initialize(Context* ctx) {
  wpi::gui::AddInit([=] {
    ImGuiSettingsHandler ini_handler;
    ini_handler.TypeName = "GlassStorage";
    ini_handler.TypeHash = ImHashStr("GlassStorage");
    ini_handler.ReadOpenFn = GlassStorageReadOpen;
    ini_handler.ReadLineFn = GlassStorageReadLine;
    ini_handler.WriteAllFn = GlassStorageWriteAll;
    ini_handler.UserData = ctx;
    ImGui::GetCurrentContext()->SettingsHandlers.push_back(ini_handler);

    ctx->sources.Initialize();
  });
}

static void Shutdown(Context* ctx) {}

Context* glass::CreateContext() {
  Context* ctx = new Context;
  if (!gContext) SetCurrentContext(ctx);
  Initialize(ctx);
  return ctx;
}

void glass::DestroyContext(Context* ctx) {
  if (!ctx) ctx = gContext;
  Shutdown(ctx);
  if (gContext == ctx) SetCurrentContext(nullptr);
  delete ctx;
}

Context* glass::GetCurrentContext() { return gContext; }

void glass::SetCurrentContext(Context* ctx) { gContext = ctx; }

void glass::ResetTime() { gContext->zeroTime = wpi::Now(); }

uint64_t glass::GetZeroTime() { return gContext->zeroTime; }

Storage::Value& Storage::GetValue(wpi::StringRef key) {
  auto it = std::find(m_keys.begin(), m_keys.end(), key);
  if (it == m_keys.end()) {
    m_keys.emplace_back(key);
    m_values.emplace_back(std::make_unique<Value>());
    return *m_values.back();
  } else {
    return *m_values[it - m_keys.begin()];
  }
}

#define DEFUN(CapsName, LowerName, CType)                                    \
  CType Storage::Get##CapsName(wpi::StringRef key, CType defaultVal) const { \
    auto it = std::find(m_keys.begin(), m_keys.end(), key);                  \
    if (it == m_keys.end()) return defaultVal;                               \
    Value& value = *m_values[it - m_keys.begin()];                           \
    if (value.type != Value::k##CapsName) {                                  \
      if (!Convert##CapsName(&value)) value.LowerName##Val = defaultVal;     \
    }                                                                        \
    return value.LowerName##Val;                                             \
  }                                                                          \
                                                                             \
  void Storage::Set##CapsName(wpi::StringRef key, CType val) {               \
    auto it = std::find(m_keys.begin(), m_keys.end(), key);                  \
    if (it == m_keys.end()) {                                                \
      m_keys.emplace_back(key);                                              \
      m_values.emplace_back(std::make_unique<Value>());                      \
      m_values.back()->type = Value::k##CapsName;                            \
      m_values.back()->LowerName##Val = val;                                 \
    } else {                                                                 \
      Value& value = *m_values[it - m_keys.begin()];                         \
      value.type = Value::k##CapsName;                                       \
      value.LowerName##Val = val;                                            \
    }                                                                        \
  }                                                                          \
                                                                             \
  CType* Storage::Get##CapsName##Ref(wpi::StringRef key, CType defaultVal) { \
    auto it = std::find(m_keys.begin(), m_keys.end(), key);                  \
    if (it == m_keys.end()) {                                                \
      m_keys.emplace_back(key);                                              \
      m_values.emplace_back(std::make_unique<Value>());                      \
      m_values.back()->type = Value::k##CapsName;                            \
      m_values.back()->LowerName##Val = defaultVal;                          \
      return &m_values.back()->LowerName##Val;                               \
    } else {                                                                 \
      Value& value = *m_values[it - m_keys.begin()];                         \
      if (value.type != Value::k##CapsName) {                                \
        if (!Convert##CapsName(&value)) value.LowerName##Val = defaultVal;   \
      }                                                                      \
      return &value.LowerName##Val;                                          \
    }                                                                        \
  }

DEFUN(Int, int, int)
DEFUN(Int64, int64, int64_t)
DEFUN(Bool, bool, bool)
DEFUN(Float, float, float)
DEFUN(Double, double, double)

std::string Storage::GetString(wpi::StringRef key,
                               const std::string& defaultVal) const {
  auto it = std::find(m_keys.begin(), m_keys.end(), key);
  if (it == m_keys.end()) return defaultVal;
  Value& value = *m_values[it - m_keys.begin()];
  value.type = Value::kString;
  return value.stringVal;
}

void Storage::SetString(wpi::StringRef key, const wpi::Twine& val) {
  auto it = std::find(m_keys.begin(), m_keys.end(), key);
  if (it == m_keys.end()) {
    m_keys.emplace_back(key);
    m_values.emplace_back(std::make_unique<Value>(val));
    m_values.back()->type = Value::kString;
  } else {
    Value& value = *m_values[it - m_keys.begin()];
    value.type = Value::kString;
    value.stringVal = val.str();
  }
}

std::string* Storage::GetStringRef(wpi::StringRef key,
                                   wpi::StringRef defaultVal) {
  auto it = std::find(m_keys.begin(), m_keys.end(), key);
  if (it == m_keys.end()) {
    m_keys.emplace_back(key);
    m_values.emplace_back(std::make_unique<Value>(defaultVal));
    m_values.back()->type = Value::kString;
    return &m_values.back()->stringVal;
  } else {
    Value& value = *m_values[it - m_keys.begin()];
    value.type = Value::kString;
    return &value.stringVal;
  }
}

Storage& glass::GetStorage() {
  auto& storage = gContext->storage[gContext->curId];
  if (!storage) storage = std::make_unique<Storage>();
  return *storage;
}

Storage& glass::GetStorage(wpi::StringRef id) {
  auto& storage = gContext->storage[id];
  if (!storage) storage = std::make_unique<Storage>();
  return *storage;
}

static void PushIDStack(wpi::StringRef label_id) {
  gContext->idStack.emplace_back(gContext->curId.size());

  auto [label, id] = wpi::StringRef{label_id}.split("###");
  // if no ###id, use label as id
  if (id.empty()) id = label;
  if (!gContext->curId.empty()) gContext->curId += "###";
  gContext->curId += id;
}

static void PopIDStack() {
  gContext->curId.resize(gContext->idStack.back());
  gContext->idStack.pop_back();
}

bool glass::Begin(const char* name, bool* p_open, ImGuiWindowFlags flags) {
  PushIDStack(name);
  return ImGui::Begin(name, p_open, flags);
}

void glass::End() {
  ImGui::End();
  PopIDStack();
}

bool glass::BeginChild(const char* str_id, const ImVec2& size, bool border,
                       ImGuiWindowFlags flags) {
  PushIDStack(str_id);
  return ImGui::BeginChild(str_id, size, border, flags);
}

void glass::EndChild() {
  ImGui::EndChild();
  PopIDStack();
}

bool glass::CollapsingHeader(const char* label, ImGuiTreeNodeFlags flags) {
  wpi::SmallString<64> openKey;
  auto [name, id] = wpi::StringRef{label}.split("###");
  // if no ###id, use name as id
  if (id.empty()) id = name;
  openKey = id;
  openKey += "###open";

  bool* open = GetStorage().GetBoolRef(openKey);
  *open = ImGui::CollapsingHeader(
      label, flags | (*open ? ImGuiTreeNodeFlags_DefaultOpen : 0));
  return *open;
}

bool glass::TreeNodeEx(const char* label, ImGuiTreeNodeFlags flags) {
  PushIDStack(label);
  bool* open = GetStorage().GetBoolRef("open");
  *open = ImGui::TreeNodeEx(
      label, flags | (*open ? ImGuiTreeNodeFlags_DefaultOpen : 0));
  if (!*open) PopIDStack();
  return *open;
}

void glass::TreePop() {
  ImGui::TreePop();
  PopIDStack();
}

void glass::PushID(const char* str_id) {
  PushIDStack(str_id);
  ImGui::PushID(str_id);
}

void glass::PushID(const char* str_id_begin, const char* str_id_end) {
  PushIDStack(wpi::StringRef(str_id_begin, str_id_end - str_id_begin));
  ImGui::PushID(str_id_begin, str_id_end);
}

void glass::PushID(int int_id) {
  char buf[16];
  std::snprintf(buf, sizeof(buf), "%d", int_id);
  PushIDStack(buf);
  ImGui::PushID(int_id);
}

void glass::PopID() {
  ImGui::PopID();
  PopIDStack();
}

bool glass::PopupEditName(const char* label, std::string* name) {
  bool rv = false;
  if (ImGui::BeginPopupContextItem(label)) {
    ImGui::Text("Edit name:");
    if (ImGui::InputText("##editname", name)) {
      rv = true;
    }
    if (ImGui::Button("Close") || ImGui::IsKeyPressedMap(ImGuiKey_Enter) ||
        ImGui::IsKeyPressedMap(ImGuiKey_KeyPadEnter)) {
      ImGui::CloseCurrentPopup();
    }
    ImGui::EndPopup();
  }
  return rv;
}
