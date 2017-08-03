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
  nt::StartServer("persistent.ini", "", 10000);
  std::this_thread::sleep_for(std::chrono::seconds(1));
  nt::SetEntryValue("/foo", nt::Value::MakeDouble(0.5));
  nt::SetEntryFlags("/foo", NT_PERSISTENT);
  nt::SetEntryValue("/foo2", nt::Value::MakeDouble(0.5));
  nt::SetEntryValue("/foo2", nt::Value::MakeDouble(0.7));
  nt::SetEntryValue("/foo2", nt::Value::MakeDouble(0.6));
  nt::SetEntryValue("/foo2", nt::Value::MakeDouble(0.5));
  std::this_thread::sleep_for(std::chrono::seconds(10));
}
