// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "main.h"

#include <poll.h>
#include <spawn.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>

#include <climits>
#include <cstdio>
#include <cstring>
#include <string>

int main(int argc, char* argv[]) {
  char path[PATH_MAX];
  char dest[PATH_MAX];
  std::memset(dest, 0, sizeof(dest));  // readlink does not null terminate!
  pid_t pid = getpid();
  std::snprintf(path, PATH_MAX, "/proc/%d/exe", pid);
  int readlink_len = readlink(path, dest, PATH_MAX);
  if (readlink_len < 0) {
    std::perror("readlink");
    return 1;
  } else if (readlink_len == PATH_MAX) {
    std::printf("Truncation occurred\n");
    return 1;
  }

  std::filesystem::path exePath{dest};
  if (exePath.empty()) {
    return 1;
  }

  if (!exePath.has_stem()) {
    return 1;
  }

  if (!exePath.has_parent_path()) {
    return 1;
  }

  if (exePath.stem() == (L"AdvantageScope")) {
    std::filesystem::path AdvantageScopePath{
        exePath.parent_path().parent_path() / L"advantagescope" /
        L"advantagescope-wpilib"};
    return StartExeTool(AdvantageScopePath);
  } else if (exePath.stem() == (L"Elastic")) {
    std::filesystem::path ElasticPath{exePath.parent_path().parent_path() /
                                      L"elastic" / L"elastic_dashboard"};
    return StartExeTool(ElasticPath);
  } else {
    return StartJavaTool(exePath);
  }
}

int StartJavaTool(std::filesystem::path& exePath) {
  pid_t pid = 0;
  std::filesystem::path jarPath{exePath};
  jarPath.replace_extension("jar");
  std::filesystem::path parentPath{exePath.parent_path()};

  if (!parentPath.has_parent_path()) {
    return 1;
  }
  std::filesystem::path toolsFolder{parentPath.parent_path()};

  std::filesystem::path Java = toolsFolder / "jdk" / "bin" / "java";

  std::string data = jarPath;
  std::string jarArg = "-jar";
  char* const arguments[] = {Java.generic_string().data(), jarArg.data(),
                             data.data(), nullptr};

  int status =
      posix_spawn(&pid, Java.c_str(), nullptr, nullptr, arguments, environ);
  if (status != 0) {
    char* home = std::getenv("JAVA_HOME");
    std::string javaLocal = "java";
    if (home != nullptr) {
      std::filesystem::path javaHomePath{home};
      javaHomePath /= "bin";
      javaHomePath /= "java";
      javaLocal = javaHomePath;
    }

    status = posix_spawn(&pid, javaLocal.c_str(), nullptr, nullptr, arguments,
                         environ);
    if (status != 0) {
      return 1;
    }
  }

  int childPid = syscall(SYS_pidfd_open, pid, 0);
  if (childPid <= 0) {
    return 1;
  }

  struct pollfd pfd = {childPid, POLLIN, 0};
  return poll(&pfd, 1, 3000);
}

int StartExeTool(std::filesystem::path& exePath) {
  char* const arguments[] = {nullptr};
  pid_t pid = 0;

  int status =
      posix_spawn(&pid, exePath.c_str(), nullptr, nullptr, arguments, environ);

  if (status != 0) {
    return 1;
  }
  int childPid = syscall(SYS_pidfd_open, pid, 0);
  if (childPid <= 0) {
    return 1;
  }

  struct pollfd pfd = {childPid, POLLIN, 0};
  return poll(&pfd, 1, 5000);
}
