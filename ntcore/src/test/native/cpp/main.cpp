// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <climits>
#include <cstdio>

#include "gmock/gmock.h"
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
  ::testing::InitGoogleMock(&argc, argv);
  int ret = RUN_ALL_TESTS();
  return ret;
}

extern "C" {
void __ubsan_on_report(void) {
  FAIL() << "Encountered an undefined behavior sanitizer error";
}
void __asan_on_error(void) {
  FAIL() << "Encountered an address sanitizer error";
}
void __tsan_on_report(void) {
  std::puts("Encountered a thread sanitizer error");
  std::_Exit(EXIT_FAILURE);
}
}  // extern "C"
