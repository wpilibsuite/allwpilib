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
}  // namespace wpi::util

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

/**
 * Decodes a binary value message using the negotiated protocol's time units.
 *
 * @param in Input bytes, advanced past the message on success.
 * @param outId Decoded topic or publisher ID.
 * @param outValue Decoded value with timestamps in nanoseconds.
 * @param error Error description on failure.
 * @param localTimeOffset Offset added to server time, in nanoseconds.
 * @param protoRev Negotiated protocol revision (e.g. NT_4_2).
 * @return True if a message was successfully decoded.
 */
bool WireDecodeBinary(std::span<const uint8_t>* in, int* outId, Value* outValue,
                      std::string* error, int64_t localTimeOffset,
                      unsigned int protoRev);

}  // namespace wpi::nt::net
