// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <filesystem>

#include "Windows.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPTSTR pCmdLine, int nCmdShow) {
  DWORD Status;
  WCHAR ExePathRaw[1024];
  Status = GetModuleFileNameW(NULL, ExePathRaw, 1024);
  if (Status == 0) {
    DWORD LastError = GetLastError();
    return LastError;
  }

  std::filesystem::path ExePath{ExePathRaw};
  if (ExePath.empty()) {
    return 1;
  }

  if (!ExePath.has_stem()) {
    return 1;
  }

  if (!ExePath.has_parent_path()) {
    return 1;
  }

  std::filesystem::path JarPath{ExePath};
  JarPath.replace_extension(L"jar");
  std::filesystem::path ParentPath{ExePath.parent_path()};

  if (!ParentPath.has_parent_path()) {
    return 1;
  }
  std::filesystem::path ToolsFolder{ParentPath.parent_path()};

  std::filesystem::path Javaw = ToolsFolder / L"jdk" / L"bin" / L"javaw.exe";

  std::wstring ToRun = L" -jar \"";
  ToRun += JarPath;
  ToRun += L"\"";

  STARTUPINFOW StartupInfo;
  PROCESS_INFORMATION ProcessInfo;

  ZeroMemory(&StartupInfo, sizeof(StartupInfo));
  StartupInfo.cb = sizeof(StartupInfo);
  ZeroMemory(&ProcessInfo, sizeof(ProcessInfo));

  if (!CreateProcessW(Javaw.c_str(), ToRun.data(), NULL, NULL, FALSE, 0, NULL,
                      NULL, &StartupInfo, &ProcessInfo)) {
    ZeroMemory(&StartupInfo, sizeof(StartupInfo));
    StartupInfo.cb = sizeof(StartupInfo);
    ZeroMemory(&ProcessInfo, sizeof(ProcessInfo));

    ToRun = L" -jar \"";
    ToRun += JarPath;
    ToRun += L"\"";

    Status = GetEnvironmentVariableW(L"JAVA_HOME", ExePathRaw, 1024);
    std::wstring JavawLocal = L"javaw";
    if (Status != 0 && Status < 1024) {
      std::filesystem::path JavaHomePath{ExePathRaw};
      JavaHomePath /= "bin";
      JavaHomePath /= "javaw.exe";
      JavawLocal = JavaHomePath;
    }

    if (!CreateProcessW(JavawLocal.c_str(), ToRun.data(), NULL, NULL, FALSE, 0,
                        NULL, NULL, &StartupInfo, &ProcessInfo)) {
      return 1;
    }
  }

  Status =
      WaitForSingleObject(ProcessInfo.hProcess, 3000);  // Wait for 3 seconds
  CloseHandle(ProcessInfo.hProcess);
  CloseHandle(ProcessInfo.hThread);

  return Status == WAIT_TIMEOUT ? 0 : 1;
}
