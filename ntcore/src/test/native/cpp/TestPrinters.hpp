// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>

#include <catch2/catch_tostring.hpp>

namespace wpi::nt {

namespace net {
struct ClientMessage;
struct ServerMessage;
}  // namespace net

class Event;
class Handle;
class PubSubOptionsImpl;
class Value;

}  // namespace wpi::nt

namespace Catch {

template <>
struct StringMaker<wpi::nt::Event> {
  static std::string convert(const wpi::nt::Event& event);
};

template <>
struct StringMaker<wpi::nt::Handle> {
  static std::string convert(const wpi::nt::Handle& handle);
};

template <>
struct StringMaker<wpi::nt::net::ClientMessage> {
  static std::string convert(const wpi::nt::net::ClientMessage& msg);
};

template <>
struct StringMaker<wpi::nt::net::ServerMessage> {
  static std::string convert(const wpi::nt::net::ServerMessage& msg);
};

template <>
struct StringMaker<wpi::nt::Value> {
  static std::string convert(const wpi::nt::Value& value);
};

template <>
struct StringMaker<wpi::nt::PubSubOptionsImpl> {
  static std::string convert(const wpi::nt::PubSubOptionsImpl& options);
};

}  // namespace Catch
