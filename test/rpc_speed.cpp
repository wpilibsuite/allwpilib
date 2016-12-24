#include <chrono>
#include <cstdio>
#include <thread>
#include <iostream>

#include "ntcore.h"

std::string callback1(nt::StringRef name, nt::StringRef params_str,
                      const nt::ConnectionInfo& conn_info) {
  auto params = nt::UnpackRpcValues(params_str, NT_DOUBLE);
  if (params.empty()) {
    std::fputs("empty params?\n", stderr);
    return "";
  }
  return nt::PackRpcValues(nt::Value::MakeDouble(params[0]->GetDouble() + 1.2));
}

int main() {
  nt::RpcDefinition def;
  def.version = 1;
  def.name = "myfunc1";
  def.params.emplace_back("param1", nt::Value::MakeDouble(0.0));
  def.results.emplace_back("result1", NT_DOUBLE);
  nt::CreateRpc("func1", nt::PackRpcDefinition(def), callback1);
  std::string call1_result_str;

  auto start2 = std::chrono::high_resolution_clock::now();
  auto start = nt::Now();
  for (int i=0; i<10000; ++i) {
    unsigned int call1_uid =
        nt::CallRpc("func1", nt::PackRpcValues(nt::Value::MakeDouble(i)));
    nt::GetRpcResult(true, call1_uid, &call1_result_str);
    auto call1_result = nt::UnpackRpcValues(call1_result_str, NT_DOUBLE);
    if (call1_result.empty()) {
      std::fputs("empty result?\n", stderr);
      return 1;
    }
  }
  auto end2 = std::chrono::high_resolution_clock::now();
  auto end = nt::Now();
  std::cerr << "nt::Now start=" << start << " end=" << end << '\n';
  std::cerr << "std::chrono start=" <<
      std::chrono::duration_cast<std::chrono::nanoseconds>(start2.time_since_epoch()).count() << " end=" <<
      std::chrono::duration_cast<std::chrono::nanoseconds>(end2.time_since_epoch()).count() << '\n';
  std::fprintf(stderr, "time/call = %g us\n", (end-start) / 10.0 / 10000.0);
  std::chrono::duration<double, std::micro> diff = end2-start2;
  std::cerr << "time/call = " << (diff.count() / 10000.0) << " us\n";

  return 0;
}
