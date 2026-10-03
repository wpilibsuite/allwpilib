// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <memory>
#include <string>

#ifdef __BAZEL__
#include "tools/cpp/runfiles/runfiles.h"
using bazel::tools::cpp::runfiles::Runfiles;
std::string LookupPath(std::string path) {
  std::string error;
  std::unique_ptr<Runfiles> runfiles(Runfiles::CreateForTest(&error));
  std::string rpath = runfiles->Rlocation("_main/" + path);
  if (rpath.empty()) {
    rpath = runfiles->Rlocation("__main__/" + path);
  }
  return rpath;
}
#else
std::string LookupPath(std::string path) {
  return path;
}
#endif
