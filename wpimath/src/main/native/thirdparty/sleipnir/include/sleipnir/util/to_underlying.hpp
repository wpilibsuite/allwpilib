// Copyright (c) Sleipnir contributors

#pragma once

#include <type_traits>

namespace slp {

template <typename Enum>
constexpr std::underlying_type_t<Enum> to_underlying(Enum e) noexcept {
  return static_cast<std::underlying_type_t<Enum>>(e);
}

}  // namespace slp
