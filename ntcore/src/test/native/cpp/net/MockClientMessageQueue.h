// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <algorithm>
#include <utility>
#include <vector>

#include "net/ClientMessageQueue.h"
#include "net/Message.h"

namespace nt::net {

class MockClientMessageQueue : public net::ClientMessageQueue {
 public:
  std::span<ClientMessage> ReadQueue(std::span<ClientMessage> out) override {
    size_t size = out.size();
    if (size > msgs.size()) {
      size = msgs.size();
    }
    std::move(msgs.begin(), msgs.begin() + size, out.begin());
    msgs.erase(msgs.begin(), msgs.begin() + size);
    return out.subspan(0, size);
  }

  void ClearQueue() override { msgs.clear(); }

  std::vector<ClientMessage> msgs;
};

}  // namespace nt::net
