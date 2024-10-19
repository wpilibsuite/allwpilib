// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "ntcore_c.h"

namespace nt::local {

struct MultiSubscriberData;
struct SubscriberData;

struct ListenerData {
  ListenerData(NT_Listener handle, SubscriberData* subscriber,
               unsigned int eventMask, bool subscriberOwned)
      : handle{handle},
        eventMask{eventMask},
        subscriber{subscriber},
        subscriberOwned{subscriberOwned} {}
  ListenerData(NT_Listener handle, MultiSubscriberData* subscriber,
               unsigned int eventMask, bool subscriberOwned)
      : handle{handle},
        eventMask{eventMask},
        multiSubscriber{subscriber},
        subscriberOwned{subscriberOwned} {}

  NT_Listener handle;
  unsigned int eventMask;
  SubscriberData* subscriber{nullptr};
  MultiSubscriberData* multiSubscriber{nullptr};
  bool subscriberOwned;
};

}  // namespace nt::local
