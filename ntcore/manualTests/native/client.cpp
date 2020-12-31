// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <chrono>
#include <climits>
#include <cstdio>
#include <thread>

#include "ntcore.h"

int main() {
  auto inst = nt::GetDefaultInstance();
  nt::AddLogger(
      inst,
      [](const nt::LogMessage& msg) {
        std::fputs(msg.message.c_str(), stderr);
        std::fputc('\n', stderr);
      },
      0, UINT_MAX);
  nt::StartClient(inst, "127.0.0.1", 10000);
  std::this_thread::sleep_for(std::chrono::seconds(2));

  auto foo = nt::GetEntry(inst, "/foo");
  auto foo_val = nt::GetEntryValue(foo);
  if (foo_val && foo_val->IsDouble()) {
    std::printf("Got foo: %g\n", foo_val->GetDouble());
  }

  auto bar = nt::GetEntry(inst, "/bar");
  nt::SetEntryValue(bar, nt::Value::MakeBoolean(false));
  nt::SetEntryFlags(bar, NT_PERSISTENT);

  auto bar2 = nt::GetEntry(inst, "/bar2");
  nt::SetEntryValue(bar2, nt::Value::MakeBoolean(true));
  nt::SetEntryValue(bar2, nt::Value::MakeBoolean(false));
  nt::SetEntryValue(bar2, nt::Value::MakeBoolean(true));
  std::this_thread::sleep_for(std::chrono::seconds(10));
}
