// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/commands2/PrintCommand.hpp"

#include <array>
#include <cstdio>
#include <regex>
#include <string>

#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

#include "CommandTestBase.hpp"
#include "wpi/commands2/Commands.hpp"

using namespace wpi::cmd;
class PrintCommandTest : public CommandTestBase {};

namespace {

class StdoutCapture {
 public:
  StdoutCapture() {
    std::fflush(stdout);
    m_file = std::tmpfile();
    REQUIRE(m_file != nullptr);
    m_originalStdout = Dup(Fileno(stdout));
    REQUIRE(m_originalStdout >= 0);
    REQUIRE(Dup2(Fileno(m_file), Fileno(stdout)) >= 0);
  }

  StdoutCapture(const StdoutCapture&) = delete;
  StdoutCapture& operator=(const StdoutCapture&) = delete;

  ~StdoutCapture() {
    if (m_originalStdout >= 0) {
      std::fflush(stdout);
      Dup2(m_originalStdout, Fileno(stdout));
      Close(m_originalStdout);
    }
    if (m_file) {
      std::fclose(m_file);
    }
  }

  std::string Stop() {
    std::fflush(stdout);
    REQUIRE(Dup2(m_originalStdout, Fileno(stdout)) >= 0);
    Close(m_originalStdout);
    m_originalStdout = -1;

    std::rewind(m_file);
    std::string output;
    std::array<char, 1024> buffer;
    while (auto count = std::fread(buffer.data(), 1, buffer.size(), m_file)) {
      output.append(buffer.data(), count);
    }
    return output;
  }

 private:
  static int Fileno(FILE* file) {
#ifdef _WIN32
    return _fileno(file);
#else
    return fileno(file);
#endif
  }

  static int Dup(int fd) {
#ifdef _WIN32
    return _dup(fd);
#else
    return dup(fd);
#endif
  }

  static int Dup2(int source, int dest) {
#ifdef _WIN32
    return _dup2(source, dest);
#else
    return dup2(source, dest);
#endif
  }

  static void Close(int fd) {
#ifdef _WIN32
    _close(fd);
#else
    close(fd);
#endif
  }

  FILE* m_file = nullptr;
  int m_originalStdout = -1;
};

}  // namespace

TEST_CASE_METHOD(PrintCommandTest, "PrintCommandTest PrintCommandSchedule",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();

  auto command = Print("Test!");

  StdoutCapture capture;

  scheduler.Schedule(command);
  scheduler.Run();

  CHECK(std::regex_search(capture.Stop(), std::regex("Test!")));

  CHECK_FALSE(scheduler.IsScheduled(command));
}
