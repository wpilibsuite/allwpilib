// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <chrono>
#include <string>
#include <string_view>

#include "DBusRuntime.hpp"
#include "wpi/net/BluetoothLEPacketClient.hpp"

namespace wpi::net::linuxbluetooth {

/**
 * Creates a pairing-agent reply scoped to one requested device.
 *
 * @param message incoming agent method call.
 * @param devicePath device being paired by this client.
 * @param reply generated method return or rejection, when handled.
 * @return whether the request was handled, unrecognized, or out of memory.
 */
DBusHandlerResult CreateBlueZPairingReply(DBusMessage* message,
                                          std::string_view devicePath,
                                          DBusMessagePtr* reply);

BluetoothLEDeviceScanResult ScanBlueZDevices(std::chrono::milliseconds timeout);
BluetoothLEPairingResult PairBlueZDevice(std::string_view target);
bool DisconnectBlueZDevice(std::string_view target, std::string* error);

}  // namespace wpi::net::linuxbluetooth
