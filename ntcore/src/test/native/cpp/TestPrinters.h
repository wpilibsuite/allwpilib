// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef NTCORE_TESTPRINTERS_H_
#define NTCORE_TESTPRINTERS_H_

#include <memory>
#include <ostream>

#include <wpi/StringRef.h>

#include "gtest/gtest.h"

namespace wpi {

inline void PrintTo(StringRef str, ::std::ostream* os) {
  ::testing::internal::PrintStringTo(str.str(), os);
}

}  // namespace wpi

namespace nt {

class EntryNotification;
class Handle;
class Message;
class Value;

void PrintTo(const EntryNotification& event, std::ostream* os);
void PrintTo(const Handle& handle, std::ostream* os);

void PrintTo(const Message& msg, std::ostream* os);

inline void PrintTo(std::shared_ptr<Message> msg, std::ostream* os) {
  *os << "shared_ptr{";
  if (msg) {
    PrintTo(*msg, os);
  }
  *os << '}';
}

void PrintTo(const Value& value, std::ostream* os);

inline void PrintTo(std::shared_ptr<Value> value, std::ostream* os) {
  *os << "shared_ptr{";
  if (value) {
    PrintTo(*value, os);
  }
  *os << '}';
}

}  // namespace nt

#endif  // NTCORE_TESTPRINTERS_H_
