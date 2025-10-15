// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef WPINET_WPINET_SRC_MAIN_NATIVE_INCLUDE_WPI_NET_HOSTNAME_HPP_
#define WPINET_WPINET_SRC_MAIN_NATIVE_INCLUDE_WPI_NET_HOSTNAME_HPP_

#include <string>
#include <string_view>

namespace wpi::net {
template <typename T>
class SmallVectorImpl;

std::string GetHostname();
std::string_view GetHostname(wpi::util::SmallVectorImpl<char>& name);
}  // namespace wpi::net

#endif  // WPINET_WPINET_SRC_MAIN_NATIVE_INCLUDE_WPI_NET_HOSTNAME_HPP_
