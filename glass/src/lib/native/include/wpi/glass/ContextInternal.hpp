// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <functional>
#include <string>
#include <vector>

#include "wpi/glass/Context.hpp"
#include "wpi/glass/Storage.hpp"
#include "wpi/util/SmallVector.hpp"
#include "wpi/util/StringMap.hpp"

namespace wpi::glass {

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
  wpi::util::SmallVector<Storage*, 32> storageStack;
  wpi::util::StringMap<Storage> storageRoots;
  wpi::util::StringMap<bool> deviceHidden;
  wpi::util::StringMap<DataSource*> sources;
  Storage& sourceNameStorage;
  uint64_t zeroTime = 0;
  bool isPlatformSaveDir = false;
};

extern Context* gContext;

}  // namespace wpi::glass
