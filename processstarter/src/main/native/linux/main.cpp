// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <poll.h>

#include <filesystem>
#include <spawn.h>

int main(int argc, char* argv[]) {
  char path[PATH_MAX];
  char dest[PATH_MAX];
  memset(dest, 0, sizeof(dest));  // readlink does not null terminate!
  pid_t pid = getpid();
  sprintf(path, "/proc/%d/exe", pid);
  int readlink_len = readlink(path, dest, PATH_MAX);
  if (readlink_len < 0) {
    perror("readlink");
    return 1;
  } else if (readlink_len == PATH_MAX) {
    printf("Truncation occured\n");
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

  std::filesystem::path jarPath{exePath};
  jarPath.replace_extension("jar");
  std::filesystem::path parentPath{exePath.parent_path()};

  if (!parentPath.has_parent_path()) {
    return 1;
  }
  std::filesystem::path toolsFolder{parentPath.parent_path()};

  std::filesystem::path Java = toolsFolder / "jdk" / "bin" / "java";

  pid = 0;
  std::string data = jarPath;
  std::string jarArg = "-jar";
  char* const arguments[] = {jarArg.data(), data.data(), nullptr};

  int status = posix_spawn(&pid, Java.c_str(), nullptr, nullptr, arguments, environ);
  if (status != 0) {
    char* home = getenv("JAVA_HOME");
    std::string javaLocal = "java";
    if (home != nullptr) {
      std::filesystem::path javaHomePath{home};
      javaHomePath /= "bin";
      javaHomePath /= "java";
      javaLocal = javaHomePath;
    }

    status = posix_spawn(&pid, javaLocal.c_str(), nullptr, nullptr, arguments, environ);
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
