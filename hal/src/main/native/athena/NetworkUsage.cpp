// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/NetworkUsage.h"

#include <wpi/MemoryBuffer.h>
#include <wpi/StringExtras.h>

#include "hal/Errors.h"

static constexpr std::string_view kFilepath = "/proc/net/dev";
static constexpr std::string_view kNicName = "eth0:";

extern "C" {

void HAL_GetNetworkStatus(int32_t* rxBytes, int32_t* txBytes, int32_t* status) {
  std::error_code ec;
  std::unique_ptr<wpi::MemoryBuffer> fileBuffer =
      wpi::MemoryBuffer::GetFileAsStream(kFilepath, ec);
  if (ec || !fileBuffer) {
    *status = HAL_PROC_NET_OPEN_ERROR;
    return;
  }
  auto content = std::string_view(
      reinterpret_cast<const char*>(fileBuffer->begin()), fileBuffer->size());
  auto offset = content.find(kNicName);
  if (offset == std::string_view::npos) {
    *status = HAL_PROC_NET_PARSE_ERROR;
    return;
  }
  offset += kNicName.length();
  // rx bytes
  auto rxBytesStart = content.find_first_not_of(' ', offset);
  offset = content.find_first_of(' ', rxBytesStart);
  auto rxBytesStr = wpi::slice(content, rxBytesStart, offset);
  auto rxBytesOpt = wpi::parse_integer<int32_t>(rxBytesStr, 10u);
  if (rxBytesOpt) {
    *rxBytes = rxBytesOpt.value();
  } else {
    *status = HAL_PROC_NET_PARSE_ERROR;
    return;
  }

  // skip over other rx fields:
  // packets, errs, drop, fifo, frame, compressed, multicast
  for (size_t i = 0; i <= 7; i++) {
    offset = content.find_first_not_of(' ', offset);
    offset = content.find_first_of(' ', offset);
  }

  // tx bytes
  auto txBytesStart = content.find_first_not_of(' ', offset);
  offset = content.find_first_of(' ', txBytesStart);
  auto txBytesStr = wpi::slice(content, txBytesStart, offset);
  auto txBytesOpt = wpi::parse_integer<int32_t>(txBytesStr, 10u);
  if (txBytesOpt) {
    *txBytes = txBytesOpt.value();
  } else {
    *status = HAL_PROC_NET_PARSE_ERROR;
    return;
  }
}

}  // extern "C"
