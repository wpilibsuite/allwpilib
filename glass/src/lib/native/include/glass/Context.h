// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <string>
#include <string_view>

#include <imgui.h>

namespace glass {

class Context;
class Storage;

Context* CreateContext();
void DestroyContext(Context* ctx = nullptr);
Context* GetCurrentContext();
void SetCurrentContext(Context* ctx);

/**
 * Resets zero time to current time.
 */
void ResetTime();

/**
 * Gets the zero time.
 */
uint64_t GetZeroTime();

/**
 * Resets the workspace (all storage except window storage).
 * Operates effectively like calling LoadStorage() on a path with no existing
 * storage files. Note this will result in auto-saving of the reset state to
 * storage.
 */
void WorkspaceReset();

/**
 * Adds function to be called during workspace (storage) initialization/load.
 * This should set up any initial default state, restore stored
 * settings/windows, etc. This will be called after the storage is initialized.
 * This must be called prior to WorkspaceInit() for proper automatic startup
 * loading.
 *
 * @param init initialization function
 */
void AddWorkspaceInit(std::function<void()> init);

/**
 * Adds function to be called during workspace (storage) reset.  This should
 * bring back the state to startup state (e.g. remove any storage references,
 * destroy windows, etc). This will be called prior to the storage being
 * destroyed.
 *
 * @param reset reset function
 */
void AddWorkspaceReset(std::function<void()> reset);

/**
 * Sets storage load and auto-save name.
 * Call this prior to calling wpi::gui::Initialize() for automatic startup
 * loading.
 *
 * @param name base name, suffix will be generated
 */
void SetStorageName(std::string_view name);

/**
 * Sets storage load and auto-save directory. For more customized behavior, set
 * Context::storageLoadPath and Context::storageAutoSavePath directly.
 * Call this prior to calling wpi::gui::Initialize() for automatic startup
 * loading.
 *
 * @param dir path to directory
 */
void SetStorageDir(std::string_view dir);

/**
 * Gets storage auto-save directory.
 *
 * @return Path to directory
 */
std::string GetStorageDir();

/**
 * Explicitly load storage. Set Context::storageLoadDir prior to calling
 * wpi::gui::Initialize() for automatic startup loading.
 *
 * Non-empty root names are not loaded unless GetStorageRoot() is called during
 * initialization (or before this function is called).
 *
 * @param dir path to directory
 */
bool LoadStorage(std::string_view dir);

/**
 * Save storage to automatic on-change save location.
 */
bool SaveStorage();

/**
 * Explicitly save storage. Set Context::storageAutoSaveDir prior to calling
 * wpi::gui::Initialize() for automatic on-change saving.
 *
 * @param dir path to directory
 */
bool SaveStorage(std::string_view dir);

/**
 * Gets the storage root for the current ID stack (e.g. the last call to
 * ResetStorageStack).
 *
 * @return Storage object
 */
Storage& GetCurStorageRoot();

/**
 * Gets an arbitrary storage root.
 *
 * Non-empty root names are saved but not loaded unless GetStorageRoot()
 * is called during initialization (or before LoadStorage is called).
 *
 * @param rootName root name
 * @return Storage object
 */
Storage& GetStorageRoot(std::string_view rootName = {});

/**
 * Resets storage stack.  Should only be called at top level.
 *
 * @param rootName root name
 */
void ResetStorageStack(std::string_view rootName = {});

/**
 * Gets the storage object for the current point in the ID stack.
 *
 * @return Storage object
 */
Storage& GetStorage();

/**
 * Pushes label/ID onto the storage stack, without pushing the imgui ID stack.
 *
 * @param label_id label or label###id
 */
void PushStorageStack(std::string_view label_id);

/**
 * Pushes specific storage onto the storage stack.
 *
 * @param storage storage
 */
void PushStorageStack(Storage& storage);

/**
 * Pops storage stack, without popping the imgui ID stack.
 */
void PopStorageStack();

bool Begin(const char* name, bool* p_open = nullptr,
           ImGuiWindowFlags flags = 0);

void End();

bool BeginChild(const char* str_id, const ImVec2& size = ImVec2(0, 0),
                bool border = false, ImGuiWindowFlags flags = 0);

void EndChild();

/**
 * Saves open status to storage "open" key.
 * If returning 'true' the header is open. doesn't indent nor push on ID stack.
 * user doesn't have to call TreePop().
 */
bool CollapsingHeader(const char* label, ImGuiTreeNodeFlags flags = 0);

bool TreeNodeEx(const char* label, ImGuiTreeNodeFlags flags = 0);

void TreePop();

// push string into the ID stack (will hash string).
void PushID(const char* str_id);

// push string into the ID stack (will hash string).
void PushID(const char* str_id_begin, const char* str_id_end);

// push string into the ID stack (will hash string).
inline void PushID(std::string_view str) {
  PushID(str.data(), str.data() + str.size());
}

// push integer into the ID stack (will hash integer).
void PushID(int int_id);

// pop from the ID stack.
void PopID();

bool PopupEditName(const char* label, std::string* name);

}  // namespace glass
