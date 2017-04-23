/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2017. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef NT_TEST_TESTPRINTERS_H_
#define NT_TEST_TESTPRINTERS_H_

#include <memory>
#include <ostream>

#include "gtest/gtest.h"

#include "llvm/StringRef.h"

namespace llvm {

inline void PrintTo(StringRef str, ::std::ostream *os) {
  ::testing::internal::PrintStringTo(str.str(), os);
}

}  // namespace llvm

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
  if (msg) PrintTo(*msg, os);
  *os << '}';
}

void PrintTo(const Value& value, std::ostream* os);

inline void PrintTo(std::shared_ptr<Value> value, std::ostream* os) {
  *os << "shared_ptr{";
  if (value) PrintTo(*value, os);
  *os << '}';
}

}  // namespace nt

#endif  // NT_TEST_TESTPRINTERS_H_
