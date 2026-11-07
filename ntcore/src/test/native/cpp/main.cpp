// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <climits>
#include <cstdio>
#include <cstdlib>

#include <catch2/catch_session.hpp>

#include "wpi/nt/ntcore.h"
#include "wpi/util/timestamp.h"

int main(int argc, char** argv) {
  wpi::nt::AddLogger(wpi::nt::GetDefaultInstance(), 0, UINT_MAX,
                     [](auto& event) {
                       if (auto msg = event.GetLogMessage()) {
                         std::fputs(msg->message.c_str(), stderr);
                         std::fputc('\n', stderr);
                       }
                     });
  int ret = Catch::Session().run(argc, argv);
  wpi::nt::ResetInstance(wpi::nt::GetDefaultInstance());
  return ret;
}

extern "C" {
void __ubsan_on_report(void) {
  std::puts("Encountered an undefined behavior sanitizer error");
  std::_Exit(EXIT_FAILURE);
}
void __asan_on_error(void) {
  std::puts("Encountered an address sanitizer error");
  std::_Exit(EXIT_FAILURE);
}
void __tsan_on_report(void) {
  std::puts("Encountered a thread sanitizer error");
  std::_Exit(EXIT_FAILURE);
}
}  // extern "C"
