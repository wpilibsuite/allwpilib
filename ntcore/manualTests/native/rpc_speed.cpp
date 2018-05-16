/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <chrono>
#include <climits>
#include <cstdio>
#include <iostream>
#include <thread>

#include <support/json.h>

#include "ntcore.h"

void callback1(const nt::RpcAnswer& answer) {
  wpi::json params;
  try {
    params = wpi::json::from_cbor(answer.params);
  } catch (wpi::json::parse_error err) {
    std::fputs("could not decode params?\n", stderr);
    return;
  }
  if (!params.is_number()) {
    std::fputs("did not get number\n", stderr);
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
      std::fputs("could not decode result?\n", stderr);
      return 1;
    }
    if (!call1_result.is_number()) {
      std::fputs("result is not number?\n", stderr);
      return 1;
    }
  }
  auto end2 = std::chrono::high_resolution_clock::now();
  auto end = nt::Now();
  std::cerr << "nt::Now start=" << start << " end=" << end << '\n';
  std::cerr << "std::chrono start="
            << std::chrono::duration_cast<std::chrono::nanoseconds>(
                   start2.time_since_epoch())
                   .count()
            << " end="
            << std::chrono::duration_cast<std::chrono::nanoseconds>(
                   end2.time_since_epoch())
                   .count()
            << '\n';
  std::fprintf(stderr, "time/call = %g us\n", (end - start) / 10.0 / 10000.0);
  std::chrono::duration<double, std::micro> diff = end2 - start2;
  std::cerr << "time/call = " << (diff.count() / 10000.0) << " us\n";

  return 0;
}
