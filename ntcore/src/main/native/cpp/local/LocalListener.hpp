// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "ntcore_c.h"

namespace nt::local {

struct LocalMultiSubscriber;
struct LocalSubscriber;

struct LocalListener {
  LocalListener(NT_Listener handle, LocalSubscriber* subscriber,
                unsigned int eventMask, bool subscriberOwned)
      : handle{handle},
        eventMask{eventMask},
        subscriber{subscriber},
        subscriberOwned{subscriberOwned} {}
  LocalListener(NT_Listener handle, LocalMultiSubscriber* subscriber,
                unsigned int eventMask, bool subscriberOwned)
      : handle{handle},
        eventMask{eventMask},
        multiSubscriber{subscriber},
        subscriberOwned{subscriberOwned} {}

  NT_Listener handle;
  unsigned int eventMask;
  LocalSubscriber* subscriber{nullptr};
  LocalMultiSubscriber* multiSubscriber{nullptr};
  bool subscriberOwned;
};

}  // namespace nt::local
