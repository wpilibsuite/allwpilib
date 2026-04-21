// Copyright (c) Sleipnir contributors

#pragma once

namespace slp {

[[noreturn]]
inline void unreachable() {
#if defined(_MSC_VER) && !defined(__clang__)
  __assume(false);
#else
  __builtin_unreachable();
#endif
}

}  // namespace slp
