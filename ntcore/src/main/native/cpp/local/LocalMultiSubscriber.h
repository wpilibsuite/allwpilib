// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <span>
#include <string>
#include <string_view>
#include <vector>

#include <wpi/StringExtras.h>
#include <wpi/Synchronization.h>

#include "Handle.h"
#include "PubSubOptions.h"
#include "VectorSet.h"
#include "ntcore_c.h"

namespace nt::local {

constexpr bool PrefixMatch(std::string_view name, std::string_view prefix,
                           bool special) {
  return (!special || !prefix.empty()) && wpi::starts_with(name, prefix);
}

struct LocalMultiSubscriber {
  static constexpr auto kType = Handle::kMultiSubscriber;

  LocalMultiSubscriber(NT_MultiSubscriber handle,
                       std::span<const std::string_view> prefixes,
                       const PubSubOptionsImpl& options)
      : handle{handle}, options{options} {
    this->options.prefixMatch = true;
    this->prefixes.reserve(prefixes.size());
    for (auto&& prefix : prefixes) {
      this->prefixes.emplace_back(prefix);
    }
  }

  bool Matches(std::string_view name, bool special) {
    for (auto&& prefix : prefixes) {
      if (PrefixMatch(name, prefix, special)) {
        return true;
      }
    }
    return false;
  }

  // invariants
  wpi::SignalObject<NT_MultiSubscriber> handle;
  std::vector<std::string> prefixes;
  PubSubOptionsImpl options;

  // value listeners
  VectorSet<NT_Listener> valueListeners;
};

}  // namespace nt::local
