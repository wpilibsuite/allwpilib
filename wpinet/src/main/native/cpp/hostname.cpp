// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/net/hostname.hpp"

#include <cstdlib>
#include <string>
#include <string_view>

#include <wpi/util/SmallVector.hpp>

#include "uv.h"

namespace wpi::net {

std::string GetHostname() {
  std::string rv;
  char name[256];
  size_t size = sizeof(name);

  int err = uv_os_gethostname(name, &size);
  if (err == 0) {
    rv.assign(name, size);
  } else if (err == UV_ENOBUFS) {
    char* name2 = static_cast<char*>(std::malloc(size));
    err = uv_os_gethostname(name2, &size);
    if (err == 0) {
      rv.assign(name2, size);
    }
    std::free(name2);
  }

  return rv;
}

std::string_view GetHostname(wpi::util::SmallVectorImpl<char>& name) {
  // Use a tmp array to not require the wpi::util::SmallVector to be too large.
  char tmpName[256];
  size_t size = sizeof(tmpName);

  name.clear();

  int err = uv_os_gethostname(tmpName, &size);
  if (err == 0) {
    name.append(tmpName, tmpName + size);
  } else if (err == UV_ENOBUFS) {
    name.resize(size);
    err = uv_os_gethostname(name.data(), &size);
    if (err != 0) {
      size = 0;
    }
  }

  return {name.data(), size};
}

}  // namespace wpi::net
