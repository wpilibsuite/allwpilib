// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <chrono>
#include <climits>
#include <cstdio>
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
  std::fprintf(stderr, "called with %g\n", val);

  answer.PostResponse(wpi::json::to_cbor(val + 1.2));
}

int main() {
  auto inst = nt::GetDefaultInstance();
  nt::AddLogger(
      inst,
      [](const nt::LogMessage& msg) {
        std::fputs(msg.message.c_str(), stderr);
        std::fputc('\n', stderr);
      },
      0, UINT_MAX);

  nt::StartServer(inst, "rpc_local.ini", "", 10000);
  auto entry = nt::GetEntry(inst, "func1");
  nt::CreateRpc(entry, nt::StringRef("", 1), callback1);
  std::fputs("calling rpc\n", stderr);
  unsigned int call1_uid = nt::CallRpc(entry, wpi::json::to_cbor(2.0));
  std::string call1_result_str;
  std::fputs("waiting for rpc result\n", stderr);
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
  std::fprintf(stderr, "got %g\n", call1_result.get<double>());

  return 0;
}
