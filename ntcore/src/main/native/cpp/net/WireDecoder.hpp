// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <span>
#include <string>
#include <string_view>

namespace wpi::util {
class Logger;
}  // namespace wpi

namespace wpi::nt {
class Value;
}  // namespace wpi::nt

namespace wpi::nt::net {

class ClientMessageHandler;
class ServerMessageHandler;

// return true if client pub/sub metadata needs updating
bool WireDecodeText(std::string_view in, ClientMessageHandler& out,
                    wpi::util::Logger& logger);
void WireDecodeText(std::string_view in, ServerMessageHandler& out,
                    wpi::util::Logger& logger);

// returns true if successfully decoded a message
bool WireDecodeBinary(std::span<const uint8_t>* in, int* outId, Value* outValue,
                      std::string* error, int64_t localTimeOffset);

}  // namespace wpi::nt::net
