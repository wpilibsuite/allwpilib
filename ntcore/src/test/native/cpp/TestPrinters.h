// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <ostream>
#include <span>
#include <string>
#include <string_view>

#include "gtest/gtest.h"

namespace wpi {

class json;

inline void PrintTo(std::string_view str, ::std::ostream* os) {
  ::testing::internal::PrintStringTo(std::string{str}, os);
}

template <typename T>
void PrintTo(std::span<T> val, ::std::ostream* os) {
  *os << '{';
  bool first = true;
  for (auto v : val) {
    if (first) {
      first = false;
    } else {
      *os << ", ";
    }
    *os << ::testing::PrintToString(v);
  }
  *os << '}';
}

void PrintTo(const json& val, ::std::ostream* os);

}  // namespace wpi

namespace nt {

namespace net3 {
class Message3;
}  // namespace net3

namespace net {
struct ClientMessage;
struct ServerMessage;
}  // namespace net

class Event;
class Handle;
class PubSubOptions;
class Value;

void PrintTo(const Event& event, std::ostream* os);
void PrintTo(const Handle& handle, std::ostream* os);
void PrintTo(const net3::Message3& msg, std::ostream* os);
void PrintTo(const net::ClientMessage& msg, std::ostream* os);
void PrintTo(const net::ServerMessage& msg, std::ostream* os);
void PrintTo(const Value& value, std::ostream* os);
void PrintTo(const PubSubOptions& options, std::ostream* os);

}  // namespace nt
