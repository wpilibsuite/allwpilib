// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <functional>
#include <string>
#include <vector>

#include <wpi/SmallVector.h>
#include <wpi/StringMap.h>

#include "glass/Context.h"
#include "glass/Storage.h"

namespace glass {

class DataSource;

class Context {
 public:
  Context();
  Context(const Context&) = delete;
  Context& operator=(const Context&) = delete;
  ~Context();

  std::vector<std::function<void()>> workspaceInit;
  std::vector<std::function<void()>> workspaceReset;
  std::string storageLoadDir = ".";
  std::string storageAutoSaveDir = ".";
  std::string storageName = "imgui";
  wpi::SmallVector<Storage*, 32> storageStack;
  wpi::StringMap<Storage> storageRoots;
  wpi::StringMap<bool> deviceHidden;
  wpi::StringMap<DataSource*> sources;
  Storage& sourceNameStorage;
  uint64_t zeroTime = 0;
  bool isPlatformSaveDir = false;
};

extern Context* gContext;

}  // namespace glass
