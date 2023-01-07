#include "Windows.h"
#include <filesystem>
#include "stdio.h"

//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR pCmdLine, int nCmdShow) {
int main() {
    printf("Is this not starting?\n");

    DWORD Status;
    WCHAR ExePathRaw[1024];
    Status = GetModuleFileNameW(NULL, ExePathRaw, 1024);
    if (Status == 0) {
        DWORD LastError = GetLastError();
        printf("GetModuleFileNameA failed: %d\n", LastError);
        return LastError;
    }

    std::filesystem::path ExePath{ExePathRaw};
    if (ExePath.empty()) {
        wprintf(L"Exe path is empty, should be %s\n", ExePathRaw);
        return 1;
    }

    if (!ExePath.has_stem()) {
        wprintf(L"Exe does not have stem? %s\n", ExePathRaw);
        return 1;
    }

    if (!ExePath.has_parent_path()) {
        wprintf(L"Exe does not have parent path? %s\n", ExePathRaw);
        return 1;
    }

    std::filesystem::path JarPath = ExePath.replace_extension(L"jar");
    std::filesystem::path ParentPath = JarPath.parent_path();

    if (!ParentPath.has_parent_path()) {
        wprintf(L"Exe directory does not have parent path? %s\n", ExePathRaw);
        return 1;
    }
    std::filesystem::path ToolsFolder = ParentPath.parent_path();

    wprintf(L"%s %s %s\n", ExePathRaw, ExePath.c_str(), JarPath.c_str());

    std::filesystem::path Javaw = ToolsFolder / L"jdk" / L"bin" / L"javaw.exe";

    //std::wstring ToRun = Javaw;
    std::wstring ToRun = L" -jar \"";
    ToRun += JarPath;
    ToRun += L"\"";

    STARTUPINFOW si;
    PROCESS_INFORMATION pi;

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );

    if (!CreateProcessW(Javaw.c_str(), ToRun.data(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        wprintf(L"Failed to start process %s with args %s\n", Javaw.c_str(), ToRun.c_str());

        ZeroMemory( &si, sizeof(si) );
        si.cb = sizeof(si);
        ZeroMemory( &pi, sizeof(pi) );

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

        if (!CreateProcessW(JavawLocal.c_str(), ToRun.data(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
            wprintf(L"Failed to start process %s with args %s\n", JavawLocal.c_str(), ToRun.c_str());
            return 1;
        }
    }

    Status = WaitForSingleObject(pi.hProcess, 3000); // Wait for 3 seconds
    return Status == WAIT_TIMEOUT ? 0 : 1;
}
