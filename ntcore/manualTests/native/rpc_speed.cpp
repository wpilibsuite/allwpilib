// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <chrono>
#include <climits>
#include <thread>

#include <fmt/core.h>
#include <support/json.h>

#include "ntcore.h"

void callback1(const nt::RpcAnswer& answer) {
  wpi::json params;
  try {
    params = wpi::json::from_cbor(answer.params);
  } catch (wpi::json::parse_error err) {
    fmt::print(stderr, "could not decode params?\n");
    return;
  }
  if (!params.is_number()) {
    fmt::print(stderr, "did not get number\n");
    return;
  }
  double val = params.get<double>();
  answer.PostResponse(wpi::json::to_cbor(val + 1.2));
}

int main() {
  auto inst = nt::GetDefaultInstance();
  nt::StartServer(inst, "rpc_speed.ini", "", 10000);
  auto entry = nt::GetEntry(inst, "func1");
  nt::CreateRpc(entry, nt::StringRef("", 1), callback1);
  std::string call1_result_str;

  auto start2 = std::chrono::high_resolution_clock::now();
  auto start = nt::Now();
  for (int i = 0; i < 10000; ++i) {
    unsigned int call1_uid = nt::CallRpc(entry, wpi::json::to_cbor(i));
    nt::GetRpcResult(entry, call1_uid, &call1_result_str);
    wpi::json call1_result;
    try {
      call1_result = wpi::json::from_cbor(call1_result_str);
    } catch (wpi::json::parse_error err) {
      fmt::print(stderr, "could not decode result?\n");
      return 1;
    }
    if (!call1_result.is_number()) {
      fmt::print(stderr, "result is not number?\n");
      return 1;
    }
  }
  auto end2 = std::chrono::high_resolution_clock::now();
  auto end = nt::Now();
  fmt::print(stderr, "nt::Now start={} end={}\n", start, end);
  fmt::print(stderr, "std::chrono start={} end={}\n",
             std::chrono::duration_cast<std::chrono::nanoseconds>(
                 start2.time_since_epoch())
                 .count(),
             std::chrono::duration_cast<std::chrono::nanoseconds>(
                 end2.time_since_epoch())
                 .count());
  fmt::print(stderr, "time/call = %g us\n", (end - start) / 10.0 / 10000.0);
  std::chrono::duration<double, std::micro> diff = end2 - start2;
  fmt::print(stderr, "time/call = {} us\n", diff.count() / 10000.0);
}
