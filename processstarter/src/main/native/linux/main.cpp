// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <sys/types.h>
#include <unistd.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>

#include <filesystem>

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
  } else {
    printf("%s\n", dest);
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

  std::filesystem::path JarPath{exePath};
  JarPath.replace_extension("jar");
  std::filesystem::path ParentPath{exePath.parent_path()};

  if (!ParentPath.has_parent_path()) {
    return 1;
  }
  std::filesystem::path ToolsFolder{ParentPath.parent_path()};

  std::filesystem::path Java = ToolsFolder / "jdk" / "bin" / "java";

  std::string ToRun = "-jar \"";
  ToRun += JarPath;
  ToRun += "\"";


}
