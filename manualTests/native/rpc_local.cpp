#include <chrono>
#include <cstdio>
#include <thread>

#include "ntcore.h"

std::string callback1(nt::StringRef name, nt::StringRef params_str,
                      const nt::ConnectionInfo& conn_info) {
  auto params = nt::UnpackRpcValues(params_str, NT_DOUBLE);
  if (params.empty()) {
    std::fputs("empty params?\n", stderr);
    return "";
  }
  std::fprintf(stderr, "called with %g\n", params[0]->GetDouble());

  return nt::PackRpcValues(nt::Value::MakeDouble(params[0]->GetDouble() + 1.2));
}

int main() {
  nt::SetLogger(
      [](unsigned int level, const char* file, unsigned int line,
         const char* msg) {
        std::fputs(msg, stderr);
        std::fputc('\n', stderr);
      },
      0);

  nt::RpcDefinition def;
  def.version = 1;
  def.name = "myfunc1";
  def.params.emplace_back("param1", nt::Value::MakeDouble(0.0));
  def.results.emplace_back("result1", NT_DOUBLE);
  nt::CreateRpc("func1", nt::PackRpcDefinition(def), callback1);
  std::fputs("calling rpc\n", stderr);
  unsigned int call1_uid =
      nt::CallRpc("func1", nt::PackRpcValues(nt::Value::MakeDouble(2.0)));
  std::string call1_result_str;
  std::fputs("waiting for rpc result\n", stderr);
  nt::GetRpcResult(true, call1_uid, &call1_result_str);
  auto call1_result = nt::UnpackRpcValues(call1_result_str, NT_DOUBLE);
  if (call1_result.empty()) {
    std::fputs("empty result?\n", stderr);
    return 1;
  }
  std::fprintf(stderr, "got %g\n", call1_result[0]->GetDouble());

  return 0;
}
