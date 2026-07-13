// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <chrono>
#include <string_view>

#include "wpi/net/BluetoothLEPacketClient.hpp"

namespace wpi::net::linuxbluetooth {

BluetoothLEDeviceScanResult ScanBlueZDevices(std::chrono::milliseconds timeout);
BluetoothLEPairingResult PairBlueZDevice(std::string_view target);

}  // namespace wpi::net::linuxbluetooth
