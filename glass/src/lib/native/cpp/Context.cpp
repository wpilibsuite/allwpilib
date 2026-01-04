// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/glass/Context.hpp"

#include <filesystem>
#include <memory>
#include <string>
#include <utility>

#include <fmt/format.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_stdlib.h>

#include "wpi/glass/ContextInternal.hpp"
#include "wpi/gui/wpigui.hpp"
#include "wpi/gui/wpigui_internal.hpp"
#include "wpi/util/MemoryBuffer.hpp"
#include "wpi/util/StringExtras.hpp"
#include "wpi/util/fs.hpp"
#include "wpi/util/json.hpp"
#include "wpi/util/raw_ostream.hpp"
#include "wpi/util/timestamp.hpp"

using namespace wpi::glass;

Context* wpi::glass::gContext;

static void WorkspaceResetImpl() {
  // call reset functions
  for (auto&& reset : gContext->workspaceReset) {
    if (reset) {
      reset();
    }
  }

  // clear storage
  for (auto&& root : gContext->storageRoots) {
    root.second.Clear();
  }

  // ImGui reset
  ImGui::ClearIniSettings();
}

static void WorkspaceInit() {
  for (auto&& init : gContext->workspaceInit) {
    if (init) {
      init();
    }
  }

  for (auto&& root : gContext->storageRoots) {
    root.second.Apply();
  }
}

static bool JsonToWindow(const wpi::util::json& jfile, const char* filename) {
  if (!jfile.is_object()) {
    ImGui::LogText("%s top level is not object", filename);
    return false;
  }

  // loop over JSON and generate ini format
  std::string iniStr;
  wpi::util::raw_string_ostream ini{iniStr};

  for (auto&& jsection : jfile.items()) {
    if (jsection.key() == "Docking") {
      continue;
    }
    if (!jsection.value().is_object()) {
      ImGui::LogText("%s section %s is not object", filename,
                     jsection.key().c_str());
      return false;
    }
    for (auto&& jsubsection : jsection.value().items()) {
      if (!jsubsection.value().is_object()) {
        ImGui::LogText("%s section %s subsection %s is not object", filename,
                       jsection.key().c_str(), jsubsection.key().c_str());
        return false;
      }
      ini << '[' << jsection.key() << "][" << jsubsection.key() << "]\n";
      for (auto&& jkv : jsubsection.value().items()) {
        try {
          auto& value = jkv.value().get_ref<const std::string&>();
          ini << jkv.key() << '=' << value << "\n";
        } catch (wpi::util::json::exception&) {
          ImGui::LogText("%s section %s subsection %s value %s is not string",
                         filename, jsection.key().c_str(),
                         jsubsection.key().c_str(), jkv.key().c_str());
          return false;
        }
      }
      ini << '\n';
    }
  }

  // emit Docking section last
  auto docking = jfile.find("Docking");
  if (docking != jfile.end()) {
    for (auto&& jsubsection : docking->items()) {
      if (!jsubsection.value().is_array()) {
        ImGui::LogText("%s section %s subsection %s is not array", filename,
                       "Docking", jsubsection.key().c_str());
        return false;
      }
      ini << "[Docking][" << jsubsection.key() << "]\n";
      for (auto&& jv : jsubsection.value()) {
        try {
          auto& value = jv.get_ref<const std::string&>();
          ini << value << "\n";
        } catch (wpi::util::json::exception&) {
          ImGui::LogText("%s section %s subsection %s value is not string",
                         filename, "Docking", jsubsection.key().c_str());
          return false;
        }
      }
      ini << '\n';
    }
  }

  ini.flush();

  ImGui::LoadIniSettingsFromMemory(iniStr.data(), iniStr.size());
  return true;
}

static bool LoadWindowStorageImpl(const std::string& filename) {
  auto fileBuffer = wpi::util::MemoryBuffer::GetFile(filename);
  if (!fileBuffer) {
    ImGui::LogText("error opening %s: %s", filename.c_str(),
                   fileBuffer.error().message().c_str());
    return false;
  }
  try {
    return JsonToWindow(
        wpi::util::json::parse(fileBuffer.value()->GetCharBuffer()),
        filename.c_str());
  } catch (wpi::util::json::parse_error& e) {
    ImGui::LogText("Error loading %s: %s", filename.c_str(), e.what());
    return false;
  }
}

static bool LoadStorageRootImpl(Context* ctx, const std::string& filename,
                                std::string_view rootName) {
  auto fileBuffer = wpi::util::MemoryBuffer::GetFile(filename);
  if (!fileBuffer) {
    ImGui::LogText("error opening %s: %s", filename.c_str(),
                   fileBuffer.error().message().c_str());
    return false;
  }
  auto [it, createdStorage] = ctx->storageRoots.try_emplace(rootName);
  try {
    it->second.FromJson(
        wpi::util::json::parse(fileBuffer.value()->GetCharBuffer()),
        filename.c_str());
  } catch (wpi::util::json::parse_error& e) {
    ImGui::LogText("Error loading %s: %s", filename.c_str(), e.what());
    if (createdStorage) {
      ctx->storageRoots.erase(it);
    }
    return false;
  }
  return true;
}

static bool LoadStorageImpl(Context* ctx, std::string_view dir,
                            std::string_view name) {
  bool rv = true;
  for (auto&& root : ctx->storageRoots) {
    std::string filename;
    auto& rootName = root.first;
    if (rootName.empty()) {
      filename = (fs::path{dir} / fmt::format("{}.json", name)).string();
    } else {
      filename =
          (fs::path{dir} / fmt::format("{}-{}.json", name, rootName)).string();
    }
    if (!LoadStorageRootImpl(ctx, filename, rootName)) {
      rv = false;
    }
  }

  WorkspaceInit();
  return rv;
}

static wpi::util::json WindowToJson() {
  size_t iniLen;
  const char* iniData = ImGui::SaveIniSettingsToMemory(&iniLen);
  std::string_view ini{iniData, iniLen};

  // parse the ini data and build JSON
  // JSON format:
  // {
  //   "Section": {
  //     "Subsection": {
  //       "Key": "Value"  // all values are saved as strings
  //     }
  //   }
  // }

  wpi::util::json out = wpi::util::json::object();
  wpi::util::json* curSection = nullptr;
  while (!ini.empty()) {
    std::string_view line;
    std::tie(line, ini) = wpi::util::split(ini, '\n');
    line = wpi::util::trim(line);
    if (line.empty()) {
      continue;
    }
    if (line[0] == '[') {
      // new section
      auto [section, subsection] = wpi::util::split(line, ']');
      section =
          wpi::util::drop_front(section);  // drop '['; ']' was dropped by split
      subsection =
          wpi::util::drop_back(wpi::util::drop_front(subsection));  // drop []
      auto& jsection = out[section];
      if (jsection.is_null()) {
        jsection = wpi::util::json::object();
      }
      curSection = &jsection[subsection];
      if (curSection->is_null()) {
        if (section == "Docking") {
          *curSection = wpi::util::json::array();
        } else {
          *curSection = wpi::util::json::object();
        }
      }
    } else {
      // value
      if (!curSection) {
        continue;  // shouldn't happen, but just in case
      }
      auto [name, value] = wpi::util::split(line, '=');
      if (curSection->is_object()) {
        (*curSection)[name] = value;
      } else if (curSection->is_array()) {
        curSection->emplace_back(line);
      }
    }
  }

  return out;
}

bool SaveWindowStorageImpl(const std::string& filename) {
  std::error_code ec;
  wpi::util::raw_fd_ostream os{filename, ec};
  if (ec) {
    ImGui::LogText("error opening %s: %s", filename.c_str(),
                   ec.message().c_str());
    return false;
  }
  WindowToJson().dump(os, 2);
  os << '\n';
  return true;
}

static bool SaveStorageRootImpl(Context* ctx, const std::string& filename,
                                const Storage& storage) {
  std::error_code ec;
  wpi::util::raw_fd_ostream os{filename, ec};
  if (ec) {
    ImGui::LogText("error opening %s: %s", filename.c_str(),
                   ec.message().c_str());
    return false;
  }
  storage.ToJson().dump(os, 2);
  os << '\n';
  return true;
}

static bool SaveStorageImpl(Context* ctx, std::string_view dir,
                            std::string_view name, bool exiting) {
  fs::path dirPath{dir};

  std::error_code ec;
  fs::create_directories(dirPath, ec);
  if (ec) {
    return false;
  }

  // handle erasing save files on exit if requested
  if (exiting && wpi::gui::gContext->resetOnExit) {
    fs::remove(dirPath / fmt::format("{}-window.json", name), ec);
    for (auto&& root : ctx->storageRoots) {
      auto& rootName = root.first;
      if (rootName.empty()) {
        fs::remove(dirPath / fmt::format("{}.json", name), ec);
      } else {
        fs::remove(dirPath / fmt::format("{}-{}.json", name, rootName), ec);
      }
    }
  }

  bool rv = SaveWindowStorageImpl(
      (dirPath / fmt::format("{}-window.json", name)).string());

  for (auto&& root : ctx->storageRoots) {
    auto& rootName = root.first;
    std::string filename;
    if (rootName.empty()) {
      filename = (dirPath / fmt::format("{}.json", name)).string();
    } else {
      filename = (dirPath / fmt::format("{}-{}.json", name, rootName)).string();
    }
    if (!SaveStorageRootImpl(ctx, filename, root.second)) {
      rv = false;
    }
  }
  return rv;
}

Context::Context()
    : sourceNameStorage{
          storageRoots.try_emplace("").first->second.GetChild("sourceNames")} {
  storageStack.emplace_back(&storageRoots[""]);

  // override ImGui ini saving
  wpi::gui::ConfigureCustomSaveSettings(
      [this] { LoadStorageImpl(this, storageLoadDir, storageName); },
      [this] {
        LoadWindowStorageImpl((fs::path{storageLoadDir} /
                               fmt::format("{}-window.json", storageName))
                                  .string());
      },
      [this](bool exiting) {
        SaveStorageImpl(this, storageAutoSaveDir, storageName, exiting);
      });
}

Context::~Context() {
  wpi::gui::ConfigureCustomSaveSettings(nullptr, nullptr, nullptr);
}

Context* wpi::glass::CreateContext() {
  Context* ctx = new Context;
  if (!gContext) {
    SetCurrentContext(ctx);
  }
  return ctx;
}

void wpi::glass::DestroyContext(Context* ctx) {
  if (!ctx) {
    ctx = gContext;
  }
  if (gContext == ctx) {
    SetCurrentContext(nullptr);
  }
  delete ctx;
}

Context* wpi::glass::GetCurrentContext() {
  return gContext;
}

void wpi::glass::SetCurrentContext(Context* ctx) {
  gContext = ctx;
}

void wpi::glass::ResetTime() {
  gContext->zeroTime = wpi::util::Now();
}

uint64_t wpi::glass::GetZeroTime() {
  return gContext->zeroTime;
}

void wpi::glass::WorkspaceReset() {
  WorkspaceResetImpl();
  WorkspaceInit();
}

void wpi::glass::AddWorkspaceInit(std::function<void()> init) {
  if (init) {
    gContext->workspaceInit.emplace_back(std::move(init));
  }
}

void wpi::glass::AddWorkspaceReset(std::function<void()> reset) {
  if (reset) {
    gContext->workspaceReset.emplace_back(std::move(reset));
  }
}

void wpi::glass::SetStorageName(std::string_view name) {
  gContext->storageName = name;
}

void wpi::glass::SetStorageDir(std::string_view dir) {
  if (dir.empty()) {
    gContext->storageLoadDir = ".";
    gContext->storageAutoSaveDir = ".";
  } else {
    gContext->storageLoadDir = dir;
    gContext->storageAutoSaveDir = dir;
    gContext->isPlatformSaveDir = (dir == wpi::gui::GetPlatformSaveFileDir());
  }
}

std::string wpi::glass::GetStorageDir() {
  return gContext->storageAutoSaveDir;
}

bool wpi::glass::LoadStorage(std::string_view dir) {
  SaveStorage();
  SetStorageDir(dir);
  WorkspaceResetImpl();
  LoadWindowStorageImpl((fs::path{gContext->storageLoadDir} /
                         fmt::format("{}-window.json", gContext->storageName))
                            .string());
  return LoadStorageImpl(gContext, dir, gContext->storageName);
}

bool wpi::glass::SaveStorage() {
  return SaveStorageImpl(gContext, gContext->storageAutoSaveDir,
                         gContext->storageName, false);
}

bool wpi::glass::SaveStorage(std::string_view dir) {
  return SaveStorageImpl(gContext, dir, gContext->storageName, false);
}

Storage& wpi::glass::GetCurStorageRoot() {
  return *gContext->storageStack.front();
}

Storage& wpi::glass::GetStorageRoot(std::string_view rootName) {
  return gContext->storageRoots[rootName];
}

void wpi::glass::ResetStorageStack(std::string_view rootName) {
  if (gContext->storageStack.size() != 1) {
    ImGui::LogText("resetting non-empty storage stack");
  }
  gContext->storageStack.clear();
  gContext->storageStack.emplace_back(&GetStorageRoot(rootName));
}

Storage& wpi::glass::GetStorage() {
  return *gContext->storageStack.back();
}

void wpi::glass::PushStorageStack(std::string_view label_id) {
  gContext->storageStack.emplace_back(
      &gContext->storageStack.back()->GetChild(label_id));
}

void wpi::glass::PushStorageStack(Storage& storage) {
  gContext->storageStack.emplace_back(&storage);
}

void wpi::glass::PopStorageStack() {
  if (gContext->storageStack.size() <= 1) {
    ImGui::LogText("attempted to pop empty storage stack, mismatch push/pop?");
    return;  // ignore
  }
  gContext->storageStack.pop_back();
}

bool wpi::glass::Begin(const char* name, bool* p_open, ImGuiWindowFlags flags) {
  PushStorageStack(name);
  return ImGui::Begin(name, p_open, flags);
}

void wpi::glass::End() {
  ImGui::End();
  PopStorageStack();
}

bool wpi::glass::BeginChild(const char* str_id, const ImVec2& size, bool border,
                            ImGuiWindowFlags flags) {
  PushStorageStack(str_id);
  return ImGui::BeginChild(str_id, size, border, flags);
}

void wpi::glass::EndChild() {
  ImGui::EndChild();
  PopStorageStack();
}

bool wpi::glass::CollapsingHeader(const char* label, ImGuiTreeNodeFlags flags) {
  bool& open = GetStorage().GetChild(label).GetBool(
      "open", (flags & ImGuiTreeNodeFlags_DefaultOpen) != 0);
  ImGui::SetNextItemOpen(open);
  open = ImGui::CollapsingHeader(label, flags);
  return open;
}

bool wpi::glass::TreeNodeEx(const char* label, ImGuiTreeNodeFlags flags) {
  PushStorageStack(label);
  bool& open = GetStorage().GetBool(
      "open", (flags & ImGuiTreeNodeFlags_DefaultOpen) != 0);
  ImGui::SetNextItemOpen(open);
  open = ImGui::TreeNodeEx(label, flags);
  if (!open) {
    PopStorageStack();
  }
  return open;
}

void wpi::glass::TreePop() {
  ImGui::TreePop();
  PopStorageStack();
}

void wpi::glass::PushID(const char* str_id) {
  PushStorageStack(str_id);
  ImGui::PushID(str_id);
}

void wpi::glass::PushID(const char* str_id_begin, const char* str_id_end) {
  PushStorageStack(std::string_view(str_id_begin, str_id_end - str_id_begin));
  ImGui::PushID(str_id_begin, str_id_end);
}

void wpi::glass::PushID(int int_id) {
  char buf[16];
  wpi::util::format_to_n_c_str(buf, sizeof(buf), "{}", int_id);

  PushStorageStack(buf);
  ImGui::PushID(int_id);
}

void wpi::glass::PopID() {
  ImGui::PopID();
  PopStorageStack();
}

bool wpi::glass::PopupEditName(const char* label, std::string* name) {
  bool rv = false;
  if (ImGui::BeginPopupContextItem(label)) {
    rv = ItemEditName(name);

    ImGui::EndPopup();
  }
  return rv;
}

bool wpi::glass::ItemEditName(std::string* name) {
  bool rv = false;

  ImGui::Text("Edit name:");
  if (ImGui::InputText("##editname", name)) {
    rv = true;
  }
  if (ImGui::Button("Close") || ImGui::IsKeyPressedMap(ImGuiKey_Enter) ||
      ImGui::IsKeyPressedMap(ImGuiKey_KeyPadEnter)) {
    ImGui::CloseCurrentPopup();
  }

  return rv;
}
