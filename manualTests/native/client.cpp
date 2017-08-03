#include <chrono>
#include <cstdio>
#include <thread>

#include "ntcore.h"

int main() {
  nt::SetLogger(
      [](unsigned int level, const char* file, unsigned int line,
         const char* msg) {
        std::fputs(msg, stderr);
        std::fputc('\n', stderr);
      },
      0);
  nt::StartClient("127.0.0.1", 10000);
  std::this_thread::sleep_for(std::chrono::seconds(2));
  auto foo = nt::GetEntryValue("/foo");
  if (foo && foo->IsDouble()) printf("Got foo: %g\n", foo->GetDouble());
  nt::SetEntryValue("/bar", nt::Value::MakeBoolean(false));
  nt::SetEntryFlags("/bar", NT_PERSISTENT);
  nt::SetEntryValue("/bar2", nt::Value::MakeBoolean(true));
  nt::SetEntryValue("/bar2", nt::Value::MakeBoolean(false));
  nt::SetEntryValue("/bar2", nt::Value::MakeBoolean(true));
  std::this_thread::sleep_for(std::chrono::seconds(10));
}
