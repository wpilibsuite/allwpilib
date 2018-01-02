/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2017-2018. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <chrono>
#include <climits>
#include <cstdio>
#include <thread>

#include "ntcore.h"

int main() {
  auto inst = nt::GetDefaultInstance();
  nt::AddLogger(inst,
                [](const nt::LogMessage& msg) {
                  std::fputs(msg.message.c_str(), stderr);
                  std::fputc('\n', stderr);
                },
                0, UINT_MAX);
  nt::StartServer(inst, "persistent.ini", "", 10000);
  std::this_thread::sleep_for(std::chrono::seconds(1));

  auto foo = nt::GetEntry(inst, "/foo");
  nt::SetEntryValue(foo, nt::Value::MakeDouble(0.5));
  nt::SetEntryFlags(foo, NT_PERSISTENT);

  auto foo2 = nt::GetEntry(inst, "/foo2");
  nt::SetEntryValue(foo2, nt::Value::MakeDouble(0.5));
  nt::SetEntryValue(foo2, nt::Value::MakeDouble(0.7));
  nt::SetEntryValue(foo2, nt::Value::MakeDouble(0.6));
  nt::SetEntryValue(foo2, nt::Value::MakeDouble(0.5));

  std::this_thread::sleep_for(std::chrono::seconds(10));
}
