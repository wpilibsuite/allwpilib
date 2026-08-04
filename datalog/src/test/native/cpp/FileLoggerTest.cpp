// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/datalog/FileLogger.hpp"

#include <atomic>
#include <chrono>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#ifdef __linux__
#include <fcntl.h>
#include <unistd.h>
#endif

TEST_CASE("FileLoggerTest BufferSingleLine", "[datalog][file-logger]") {
  std::vector<std::string> buf;
  auto func = wpi::log::FileLogger::Buffer(
      [&buf](std::string_view line) { buf.emplace_back(line); });
  func("qwertyuiop\n");
  CHECK("qwertyuiop" == buf[0]);
}

TEST_CASE("FileLoggerTest BufferMultiLine", "[datalog][file-logger]") {
  std::vector<std::string> buf;
  auto func = wpi::log::FileLogger::Buffer(
      [&buf](std::string_view line) { buf.emplace_back(line); });
  func("line 1\nline 2\nline 3\n");
  CHECK("line 1\nline 2\nline 3" == buf[0]);
}

TEST_CASE("FileLoggerTest BufferPartials", "[datalog][file-logger]") {
  std::vector<std::string> buf;
  auto func = wpi::log::FileLogger::Buffer(
      [&buf](std::string_view line) { buf.emplace_back(line); });
  func("part 1");
  func("part 2\npart 3");
  CHECK("part 1part 2" == buf[0]);
  func("\n");
  CHECK("part 3" == buf[1]);
}

TEST_CASE("FileLoggerTest BufferMultiplePartials", "[datalog][file-logger]") {
  std::vector<std::string> buf;
  auto func = wpi::log::FileLogger::Buffer(
      [&buf](std::string_view line) { buf.emplace_back(line); });
  func("part 1");
  func("part 2");
  func("part 3");
  func("part 4\n");
  CHECK("part 1part 2part 3part 4" == buf[0]);
}
TEST_CASE("FileLoggerTest BufferMultipleMultiLinePartials",
          "[datalog][file-logger]") {
  std::vector<std::string> buf;
  auto func = wpi::log::FileLogger::Buffer(
      [&buf](std::string_view line) { buf.emplace_back(line); });
  func("part 1");
  func("part 2\npart 3");
  func("part 4\n");
  CHECK("part 1part 2" == buf[0]);
  CHECK("part 3part 4" == buf[1]);
}

#ifdef __linux__
TEST_CASE("FileLoggerTest MissingFileDoesNotDeadlock",
          "[datalog][file-logger]") {
  // Constructing a FileLogger for a nonexistent path used to spawn a reader
  // thread that blocked forever in read() (no watch was ever added, so no
  // event could wake it), and the destructor's join() then deadlocked.
  // Regression test: the destructor must return promptly.
  auto path = std::format("/tmp/wpi_filelogger_missing_{}", getpid());
  unlink(path.c_str());

  std::atomic<bool> done{false};
  std::thread t{[&] {
    {
      wpi::log::FileLogger logger{path, [](std::string_view) {}};
      // Let the reader thread block in read() before destruction; otherwise
      // the destructor's close() races ahead and the old deadlock never fires.
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    done = true;
  }};
  t.detach();

  for (int i = 0; i < 100 && !done; ++i) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  CHECK(done);
}

TEST_CASE("FileLoggerTest ExistingFileDeliversData", "[datalog][file-logger]") {
  auto path = std::format("/tmp/wpi_filelogger_existing_{}", getpid());
  unlink(path.c_str());
  int fd = open(path.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0644);
  REQUIRE(fd != -1);
  close(fd);

  std::atomic<bool> gotData{false};
  {
    wpi::log::FileLogger logger{
        path, [&gotData](std::string_view) { gotData = true; }};

    // Append repeatedly so the reader thread notices regardless of when its
    // initial lseek() runs.
    for (int i = 0; i < 40 && !gotData; ++i) {
      int wfd = open(path.c_str(), O_WRONLY | O_APPEND);
      REQUIRE(wfd != -1);
      (void)write(wfd, "hello\n", 6);
      close(wfd);
      std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    CHECK(gotData);
  }
  unlink(path.c_str());
}
#endif
