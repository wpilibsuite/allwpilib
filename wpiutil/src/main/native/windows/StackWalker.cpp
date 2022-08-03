/**********************************************************************
 *
 * StackWalker.cpp
 * https://github.com/JochenKalmbach/StackWalker
 *
 * Old location: http://stackwalker.codeplex.com/
 *
 *
 * History:
 *  2005-07-27   v1    - First public release on http://www.codeproject.com/
 *                       http://www.codeproject.com/threads/StackWalker.asp
 *  2005-07-28   v2    - Changed the params of the constructor and ShowCallstack
 *                       (to simplify the usage)
 *  2005-08-01   v3    - Changed to use 'CONTEXT_FULL' instead of CONTEXT_ALL
 *                       (should also be enough)
 *                     - Changed to compile correctly with the PSDK of VC7.0
 *                       (GetFileVersionInfoSizeA and GetFileVersionInfoA is wrongly defined:
 *                        it uses LPSTR instead of LPCSTR as first parameter)
 *                     - Added declarations to support VC5/6 without using 'dbghelp.h'
 *                     - Added a 'pUserData' member to the ShowCallstack function and the
 *                       PReadProcessMemoryRoutine declaration (to pass some user-defined data,
 *                       which can be used in the readMemoryFunction-callback)
 *  2005-08-02   v4    - OnSymInit now also outputs the OS-Version by default
 *                     - Added example for doing an exception-callstack-walking in main.cpp
 *                       (thanks to owillebo: http://www.codeproject.com/script/profile/whos_who.asp?id=536268)
 *  2005-08-05   v5    - Removed most Lint (http://www.gimpel.com/) errors... thanks to Okko Willeboordse!
 *  2008-08-04   v6    - Fixed Bug: Missing LEAK-end-tag
 *                       http://www.codeproject.com/KB/applications/leakfinder.aspx?msg=2502890#xx2502890xx
 *                       Fixed Bug: Compiled with "WIN32_LEAN_AND_MEAN"
 *                       http://www.codeproject.com/KB/applications/leakfinder.aspx?msg=1824718#xx1824718xx
 *                       Fixed Bug: Compiling with "/Wall"
 *                       http://www.codeproject.com/KB/threads/StackWalker.aspx?msg=2638243#xx2638243xx
 *                       Fixed Bug: Now checking SymUseSymSrv
 *                       http://www.codeproject.com/KB/threads/StackWalker.aspx?msg=1388979#xx1388979xx
 *                       Fixed Bug: Support for recursive function calls
 *                       http://www.codeproject.com/KB/threads/StackWalker.aspx?msg=1434538#xx1434538xx
 *                       Fixed Bug: Missing FreeLibrary call in "GetModuleListTH32"
 *                       http://www.codeproject.com/KB/threads/StackWalker.aspx?msg=1326923#xx1326923xx
 *                       Fixed Bug: SymDia is number 7, not 9!
 *  2008-09-11   v7      For some (undocumented) reason, dbhelp.h is needing a packing of 8!
 *                       Thanks to Teajay which reported the bug...
 *                       http://www.codeproject.com/KB/applications/leakfinder.aspx?msg=2718933#xx2718933xx
 *  2008-11-27   v8      Debugging Tools for Windows are now stored in a different directory
 *                       Thanks to Luiz Salamon which reported this "bug"...
 *                       http://www.codeproject.com/KB/threads/StackWalker.aspx?msg=2822736#xx2822736xx
 *  2009-04-10   v9      License slightly corrected (<ORGANIZATION> replaced)
 *  2009-11-01   v10     Moved to http://stackwalker.codeplex.com/
 *  2009-11-02   v11     Now try to use IMAGEHLP_MODULE64_V3 if available
 *  2010-04-15   v12     Added support for VS2010 RTM
 *  2010-05-25   v13     Now using secure MyStrcCpy. Thanks to luke.simon:
 *                       http://www.codeproject.com/KB/applications/leakfinder.aspx?msg=3477467#xx3477467xx
 *  2013-01-07   v14     Runtime Check Error VS2010 Debug Builds fixed:
 *                       http://stackwalker.codeplex.com/workitem/10511
 *
 *
 * LICENSE (http://www.opensource.org/licenses/bsd-license.php)
 *
 *   Copyright (c) 2005-2013, Jochen Kalmbach
 *   All rights reserved.
 *
 *   Redistribution and use in source and binary forms, with or without modification,
 *   are permitted provided that the following conditions are met:
 *
 *   Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 *   Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *   Neither the name of Jochen Kalmbach nor the names of its contributors may be
 *   used to endorse or promote products derived from this software without
 *   specific prior written permission.
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 *   THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *   ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
 *   FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *   ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 **********************************************************************/

#include "StackWalker.h"

#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#pragma comment(lib, "version.lib") // for "VerQueryValue"
#pragma comment(lib, "Advapi32.lib") // for "GetUserName"
#pragma warning(disable : 4826)

#ifdef UNICODE
  #define DBGHELP_TRANSLATE_TCHAR

#endif
#pragma pack(push, 8)
#include <dbghelp.h>
#pragma pack(pop)


static void MyStrCpy(TCHAR* szDest, size_t nMaxDestSize, const TCHAR* szSrc)
{
  if (nMaxDestSize <= 0)
    return;
  _tcsncpy_s(szDest, nMaxDestSize, szSrc, _TRUNCATE);
  // INFO: _TRUNCATE will ensure that it is null-terminated;
  // but with older compilers (<1400) it uses "strncpy" and this does not!)
  szDest[nMaxDestSize - 1] = 0;
} // MyStrCpy

#ifdef _UNICODE
  typedef SYMBOL_INFOW   tSymbolInfo;
  typedef IMAGEHLP_LINEW64  tImageHelperLine;
#else
  typedef SYMBOL_INFO   tSymbolInfo;
  typedef IMAGEHLP_LINE64  tImageHelperLine;
#endif

// Normally it should be enough to use 'CONTEXT_FULL' (better would be 'CONTEXT_ALL')
#define USED_CONTEXT_FLAGS CONTEXT_FULL

class StackWalkerInternal
{
public:
  StackWalkerInternal(StackWalker* parent, HANDLE hProcess)
  {
    m_parent = parent;
    m_hDbhHelp = NULL;
    symCleanup = NULL;
    m_hProcess = hProcess;
    m_szSymPath = NULL;
    symFunctionTableAccess64 = NULL;
    symGetLineFromAddr64 = NULL;
    symGetModuleBase64 = NULL;
    symGetModuleInfo64 = NULL;
    symGetOptions = NULL;
    symFromAddr = NULL;
    symInitialize = NULL;
    symLoadModuleEx = NULL;
    symSetOptions = NULL;
    stackWalk64 = NULL;
    unDecorateSymbolName = NULL;
    symGetSearchPath = NULL;
  }
  ~StackWalkerInternal()
  {
    if (symCleanup != NULL)
      symCleanup(m_hProcess); // SymCleanup
    if (m_hDbhHelp != NULL)
      FreeLibrary(m_hDbhHelp);
    m_hDbhHelp = NULL;
    m_parent = NULL;
    if (m_szSymPath != NULL)
      free(m_szSymPath);
    m_szSymPath = NULL;
  }
  BOOL Init(LPCTSTR szSymPath)
  {
    if (m_parent == NULL)
      return FALSE;
    // Dynamically load the Entry-Points for dbghelp.dll:
    // First try to load the newest one from
    TCHAR szTemp[4096];
    // But before we do this, we first check if the ".local" file exists
    if (GetModuleFileName(NULL, szTemp, 4096) > 0)
    {
      _tcscat_s(szTemp, _T(".local"));
      if (GetFileAttributes(szTemp) == INVALID_FILE_ATTRIBUTES)
      {
        // ".local" file does not exist, so we can try to load the dbghelp.dll from the "Debugging Tools for Windows"
        // Ok, first try the new path according to the architecture:
#ifdef _M_IX86
        if ((m_hDbhHelp == NULL) && (GetEnvironmentVariable(_T("ProgramFiles"), szTemp, 4096) > 0))
        {
          _tcscat_s(szTemp, _T("\\Debugging Tools for Windows (x86)\\dbghelp.dll"));
          // now check if the file exists:
          if (GetFileAttributes(szTemp) != INVALID_FILE_ATTRIBUTES)
          {
            m_hDbhHelp = LoadLibrary(szTemp);
          }
        }
#elif _M_X64
        if ((m_hDbhHelp == NULL) && (GetEnvironmentVariable(_T("ProgramFiles"), szTemp, 4096) > 0))
        {
          _tcscat_s(szTemp, _T("\\Debugging Tools for Windows (x64)\\dbghelp.dll"));
          // now check if the file exists:
          if (GetFileAttributes(szTemp) != INVALID_FILE_ATTRIBUTES)
          {
            m_hDbhHelp = LoadLibrary(szTemp);
          }
        }
#elif _M_IA64
        if ((m_hDbhHelp == NULL) && (GetEnvironmentVariable(_T("ProgramFiles"), szTemp, 4096) > 0))
        {
          _tcscat_s(szTemp, _T("\\Debugging Tools for Windows (ia64)\\dbghelp.dll"));
          // now check if the file exists:
          if (GetFileAttributes(szTemp) != INVALID_FILE_ATTRIBUTES)
          {
            m_hDbhHelp = LoadLibrary(szTemp);
          }
        }
#endif
        // If still not found, try the old directories...
        if ((m_hDbhHelp == NULL) && (GetEnvironmentVariable(_T("ProgramFiles"), szTemp, 4096) > 0))
        {
          _tcscat_s(szTemp, _T("\\Debugging Tools for Windows\\dbghelp.dll"));
          // now check if the file exists:
          if (GetFileAttributes(szTemp) != INVALID_FILE_ATTRIBUTES)
          {
            m_hDbhHelp = LoadLibrary(szTemp);
          }
        }
#if defined _M_X64 || defined _M_IA64
        // Still not found? Then try to load the (old) 64-Bit version:
        if ((m_hDbhHelp == NULL) && (GetEnvironmentVariable(_T("ProgramFiles"), szTemp, 4096) > 0))
        {
          _tcscat_s(szTemp, _T("\\Debugging Tools for Windows 64-Bit\\dbghelp.dll"));
          if (GetFileAttributes(szTemp) != INVALID_FILE_ATTRIBUTES)
          {
            m_hDbhHelp = LoadLibrary(szTemp);
          }
        }
#endif
      }
    }
    if (m_hDbhHelp == NULL) // if not already loaded, try to load a default-one
      m_hDbhHelp = LoadLibrary(_T("dbghelp.dll"));
    if (m_hDbhHelp == NULL)
      return FALSE;

#ifdef _UNICODE
    static const char strSymInitialize[] = "SymInitializeW";
    static const char strUnDecorateSymbolName[] = "UnDecorateSymbolNameW";
    static const char strSymGetSearchPath[] = "SymGetSearchPathW";
    static const char strSymLoadModuleEx[] = "SymLoadModuleExW";
    static const char strSymGetLineFromAddr64[] = "SymGetLineFromAddrW64";
    static const char strSymGetModuleInfo64[] = "SymGetModuleInfoW64";
    static const char strSymFromAddr[] = "SymFromAddrW";
#else
    static const char strSymInitialize[] = "SymInitialize";
    static const char strUnDecorateSymbolName[] = "UnDecorateSymbolName";
    static const char strSymGetSearchPath[] = "SymGetSearchPath";
    static const char strSymLoadModuleEx[] = "SymLoadModuleEx";
    static const char strSymGetLineFromAddr64[] = "SymGetLineFromAddr64";
    static const char strSymGetModuleInfo64[] = "SymGetModuleInfo64";
    static const char strSymFromAddr[] = "SymFromAddr";
#endif
    symInitialize = (tSI)GetProcAddress(m_hDbhHelp, strSymInitialize);
    symCleanup = (tSC)GetProcAddress(m_hDbhHelp, "SymCleanup");

    stackWalk64 = (tSW)GetProcAddress(m_hDbhHelp, "StackWalk64");
    symGetOptions = (tSGO)GetProcAddress(m_hDbhHelp, "SymGetOptions");
    symSetOptions = (tSSO)GetProcAddress(m_hDbhHelp, "SymSetOptions");

    symFunctionTableAccess64 = (tSFTA)GetProcAddress(m_hDbhHelp, "SymFunctionTableAccess64");
    symGetLineFromAddr64 = (tSGLFA)GetProcAddress(m_hDbhHelp, strSymGetLineFromAddr64);
    symGetModuleBase64 = (tSGMB)GetProcAddress(m_hDbhHelp, "SymGetModuleBase64");
    symGetModuleInfo64 = (tSGMI)GetProcAddress(m_hDbhHelp, strSymGetModuleInfo64);
    symFromAddr = (tSFA)GetProcAddress(m_hDbhHelp, strSymFromAddr);
    unDecorateSymbolName = (tUDSN)GetProcAddress(m_hDbhHelp, strUnDecorateSymbolName);
    symLoadModuleEx = (tSLM)GetProcAddress(m_hDbhHelp, strSymLoadModuleEx);
    symGetSearchPath = (tSGSP)GetProcAddress(m_hDbhHelp, strSymGetSearchPath);

    if (symCleanup == NULL || symFunctionTableAccess64 == NULL || symGetModuleBase64 == NULL || symGetModuleInfo64 == NULL || symGetOptions == NULL ||
        symFromAddr == NULL || symInitialize == NULL || symSetOptions == NULL || stackWalk64 == NULL || unDecorateSymbolName == NULL ||
        symLoadModuleEx == NULL)
    {
      FreeLibrary(m_hDbhHelp);
      m_hDbhHelp = NULL;
      symCleanup = NULL;
      return FALSE;
    }

    // SymInitialize
    if (szSymPath != NULL)
      m_szSymPath = _tcsdup(szSymPath);
    if (this->symInitialize(m_hProcess, m_szSymPath, FALSE) == FALSE)
      this->m_parent->OnDbgHelpErr(_T("SymInitialize"), GetLastError(), 0);

    DWORD symOptions = this->symGetOptions(); // SymGetOptions
    symOptions |= SYMOPT_LOAD_LINES;
    symOptions |= SYMOPT_FAIL_CRITICAL_ERRORS;
    //symOptions |= SYMOPT_NO_PROMPTS;
    // SymSetOptions
    symOptions = this->symSetOptions(symOptions);

    TCHAR buf[StackWalker::STACKWALK_MAX_NAMELEN] = {0};
    if (this->symGetSearchPath != NULL)
    {
      if (this->symGetSearchPath(m_hProcess, buf, StackWalker::STACKWALK_MAX_NAMELEN) == FALSE)
        this->m_parent->OnDbgHelpErr(_T("SymGetSearchPath"), GetLastError(), 0);
    }
    TCHAR  szUserName[1024] = {0};
    DWORD dwSize = 1024;
    GetUserName(szUserName, &dwSize);
    this->m_parent->OnSymInit(buf, symOptions, szUserName);

    return TRUE;
  }

  StackWalker* m_parent;

  HMODULE m_hDbhHelp;
  HANDLE  m_hProcess;
  LPTSTR   m_szSymPath;

#pragma pack(push, 8)
  typedef struct IMAGEHLP_MODULE64_V3
  {
    DWORD    SizeOfStruct;         // set to sizeof(IMAGEHLP_MODULE64)
    DWORD64  BaseOfImage;          // base load address of module
    DWORD    ImageSize;            // virtual size of the loaded module
    DWORD    TimeDateStamp;        // date/time stamp from pe header
    DWORD    CheckSum;             // checksum from the pe header
    DWORD    NumSyms;              // number of symbols in the symbol table
    SYM_TYPE SymType;              // type of symbols loaded
    TCHAR     ModuleName[32];       // module name
    TCHAR     ImageName[256];       // image name
    TCHAR     LoadedImageName[256]; // symbol file name
    // new elements: 07-Jun-2002
    TCHAR  LoadedPdbName[256];   // pdb file name
    DWORD CVSig;                // Signature of the CV record in the debug directories
    TCHAR  CVData[MAX_PATH * 3]; // Contents of the CV record
    DWORD PdbSig;               // Signature of PDB
    GUID  PdbSig70;             // Signature of PDB (VC 7 and up)
    DWORD PdbAge;               // DBI age of pdb
    BOOL  PdbUnmatched;         // loaded an unmatched pdb
    BOOL  DbgUnmatched;         // loaded an unmatched dbg
    BOOL  LineNumbers;          // we have line number information
    BOOL  GlobalSymbols;        // we have internal symbol information
    BOOL  TypeInfo;             // we have type information
    // new elements: 17-Dec-2003
    BOOL SourceIndexed; // pdb supports source server
    BOOL Publics;       // contains public symbols
  };

  typedef struct IMAGEHLP_MODULE64_V2
  {
    DWORD    SizeOfStruct;         // set to sizeof(IMAGEHLP_MODULE64)
    DWORD64  BaseOfImage;          // base load address of module
    DWORD    ImageSize;            // virtual size of the loaded module
    DWORD    TimeDateStamp;        // date/time stamp from pe header
    DWORD    CheckSum;             // checksum from the pe header
    DWORD    NumSyms;              // number of symbols in the symbol table
    SYM_TYPE SymType;              // type of symbols loaded
    CHAR     ModuleName[32];       // module name
    CHAR     ImageName[256];       // image name
    CHAR     LoadedImageName[256]; // symbol file name
  };
#pragma pack(pop)

  // SymCleanup()
  typedef BOOL(__stdcall* tSC)(IN HANDLE hProcess);
  tSC symCleanup;

  // SymFunctionTableAccess64()
  typedef PVOID(__stdcall* tSFTA)(HANDLE hProcess, DWORD64 AddrBase);
  tSFTA symFunctionTableAccess64;

  // SymGetLineFromAddr64()
  typedef BOOL(__stdcall* tSGLFA)(IN HANDLE hProcess,
                                  IN DWORD64 dwAddr,
                                  OUT PDWORD pdwDisplacement,
                                  OUT tImageHelperLine* Line);
  tSGLFA symGetLineFromAddr64;

  // SymGetModuleBase64()
  typedef DWORD64(__stdcall* tSGMB)(IN HANDLE hProcess, IN DWORD64 dwAddr);
  tSGMB symGetModuleBase64;

  // SymGetModuleInfo64()
  typedef BOOL(__stdcall* tSGMI)(IN HANDLE hProcess,
                                 IN DWORD64 dwAddr,
                                 OUT IMAGEHLP_MODULE64_V3* ModuleInfo);
  tSGMI symGetModuleInfo64;

  // SymGetOptions()
  typedef DWORD(__stdcall* tSGO)(VOID);
  tSGO symGetOptions;


  // SymGetSymFromAddr64()
  typedef BOOL(__stdcall* tSFA)(IN HANDLE hProcess,
                                  IN DWORD64 Address,
                                  OUT PDWORD64 pdwDisplacement,
                                  OUT tSymbolInfo* Symbol);
  tSFA symFromAddr;

  // SymInitialize()
  typedef BOOL(__stdcall* tSI)(IN HANDLE hProcess, IN PTSTR UserSearchPath, IN BOOL fInvadeProcess);
  tSI symInitialize;

  // SymLoadModule64()
  typedef DWORD64(__stdcall* tSLM)(IN HANDLE hProcess,
                                   IN HANDLE hFile,
                                   IN PTSTR ImageName,
                                   IN PTSTR ModuleName,
                                   IN DWORD64 BaseOfDll,
                                   IN DWORD SizeOfDll,
                                   IN PMODLOAD_DATA Data,
                                   IN DWORD         Flags);
  tSLM symLoadModuleEx;

  // SymSetOptions()
  typedef DWORD(__stdcall* tSSO)(IN DWORD SymOptions);
  tSSO symSetOptions;

  // StackWalk64()
  typedef BOOL(__stdcall* tSW)(DWORD                            MachineType,
                               HANDLE                           hProcess,
                               HANDLE                           hThread,
                               LPSTACKFRAME64                   StackFrame,
                               PVOID                            ContextRecord,
                               PREAD_PROCESS_MEMORY_ROUTINE64   ReadMemoryRoutine,
                               PFUNCTION_TABLE_ACCESS_ROUTINE64 FunctionTableAccessRoutine,
                               PGET_MODULE_BASE_ROUTINE64       GetModuleBaseRoutine,
                               PTRANSLATE_ADDRESS_ROUTINE64     TranslateAddress);
  tSW stackWalk64;

  // UnDecorateSymbolName()
  typedef DWORD(__stdcall WINAPI* tUDSN)(PCTSTR DecoratedName,
                                         PTSTR  UnDecoratedName,
                                         DWORD UndecoratedLength,
                                         DWORD Flags);
  tUDSN unDecorateSymbolName;

  typedef BOOL(__stdcall WINAPI* tSGSP)(HANDLE hProcess, PTSTR SearchPath, DWORD SearchPathLength);
  tSGSP symGetSearchPath;

private:
// **************************************** ToolHelp32 ************************
#define MAX_MODULE_NAME32 255
#define TH32CS_SNAPMODULE 0x00000008
#pragma pack(push, 8)
  typedef struct tagMODULEENTRY32
  {
    DWORD   dwSize;
    DWORD   th32ModuleID;  // This module
    DWORD   th32ProcessID; // owning process
    DWORD   GlblcntUsage;  // Global usage count on the module
    DWORD   ProccntUsage;  // Module usage count in th32ProcessID's context
    BYTE*   modBaseAddr;   // Base address of module in th32ProcessID's context
    DWORD   modBaseSize;   // Size in bytes of module starting at modBaseAddr
    HMODULE hModule;       // The hModule of this module in th32ProcessID's context
    TCHAR   szModule[MAX_MODULE_NAME32 + 1];
    TCHAR   szExePath[MAX_PATH];
  } MODULEENTRY32;
  typedef MODULEENTRY32* PMODULEENTRY32;
  typedef MODULEENTRY32* LPMODULEENTRY32;
#pragma pack(pop)

  BOOL GetModuleListTH32(HANDLE hProcess, DWORD pid)
  {
    // CreateToolhelp32Snapshot()
    typedef HANDLE(__stdcall * tCT32S)(DWORD dwFlags, DWORD th32ProcessID);
    // Module32First()
    typedef BOOL(__stdcall * tM32F)(HANDLE hSnapshot, LPMODULEENTRY32 lpme);
    // Module32Next()
    typedef BOOL(__stdcall * tM32N)(HANDLE hSnapshot, LPMODULEENTRY32 lpme);

    // try both dlls...
    const TCHAR* dllname[] = {_T("kernel32.dll"), _T("tlhelp32.dll")};
    HINSTANCE    hToolhelp = NULL;
    tCT32S       createToolhelp32Snapshot = NULL;
    tM32F        module32First = NULL;
    tM32N        module32Next = NULL;

    HANDLE        hSnap;
    MODULEENTRY32 moduleEntry32;
    moduleEntry32.dwSize = sizeof(moduleEntry32);
    BOOL   keepGoing;

#ifdef _UNICODE
    static const char strModule32First[] = "Module32FirstW";
    static const char strModule32Next[] = "Module32NextW";
 #else
    static const char strModule32First[] = "Module32First";
    static const char strModule32Next[] = "Module32Next";
#endif
    for (size_t i = 0; i < (sizeof(dllname) / sizeof(dllname[0])); i++)
    {
      hToolhelp = LoadLibrary(dllname[i]);
      if (hToolhelp == NULL)
        continue;
      createToolhelp32Snapshot = (tCT32S)GetProcAddress(hToolhelp, "CreateToolhelp32Snapshot");
      module32First = (tM32F)GetProcAddress(hToolhelp, strModule32First);  
      module32Next = (tM32N)GetProcAddress(hToolhelp, strModule32Next); 
      if ((createToolhelp32Snapshot != NULL) && (module32First != NULL) && (module32Next != NULL))
        break; // found the functions!
      FreeLibrary(hToolhelp);
      hToolhelp = NULL;
    }

    if (hToolhelp == NULL)
      return FALSE;

    hSnap = createToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
    if (hSnap == (HANDLE)-1)
    {
      FreeLibrary(hToolhelp);
      return FALSE;
    }

    keepGoing = !!module32First(hSnap, &moduleEntry32);
    int cnt = 0;
    while (keepGoing)
    {
      this->LoadModule(hProcess, moduleEntry32.szExePath, moduleEntry32.szModule, (DWORD64)moduleEntry32.modBaseAddr,
                       moduleEntry32.modBaseSize);
      cnt++;
      keepGoing = !!module32Next(hSnap, &moduleEntry32);
    }
    CloseHandle(hSnap);
    FreeLibrary(hToolhelp);
    if (cnt <= 0)
      return FALSE;
    return TRUE;
  } // GetModuleListTH32

  // **************************************** PSAPI ************************
  typedef struct _MODULEINFO
  {
    LPVOID lpBaseOfDll;
    DWORD  SizeOfImage;
    LPVOID EntryPoint;
  } MODULEINFO, *LPMODULEINFO;

  BOOL GetModuleListPSAPI(HANDLE hProcess)
  {
    // EnumProcessModules()
    typedef BOOL(__stdcall * tEPM)(HANDLE hProcess, HMODULE * lphModule, DWORD cb,
                                   LPDWORD lpcbNeeded);
    // GetModuleFileNameEx()
    typedef DWORD(__stdcall * tGMFNE)(HANDLE hProcess, HMODULE hModule, LPTSTR lpFilename,
                                      DWORD nSize);
    // GetModuleBaseName()
    typedef DWORD(__stdcall * tGMBN)(HANDLE hProcess, HMODULE hModule, LPTSTR lpFilename,
                                     DWORD nSize);
    // GetModuleInformation()
    typedef BOOL(__stdcall * tGMI)(HANDLE hProcess, HMODULE hModule, LPMODULEINFO pmi, DWORD nSize);

      //ModuleEntry e;
    DWORD        cbNeeded;
    MODULEINFO   mi;
    HMODULE*     hMods = 0;
    TCHAR*        tt = NULL;
    TCHAR*        tt2 = NULL;
    const SIZE_T TTBUFLEN = 8096;
    int          cnt = 0;

    HINSTANCE hPsapi = LoadLibrary(_T("psapi.dll"));
    if (hPsapi == NULL)
      return FALSE;

#ifdef _UNICODE
    static const char strGetModuleFileName[] = "GetModuleFileNameExW";
    static const char strGetModuleBaseName[] = "GetModuleBaseNameW";
#else
    static const char strGetModuleFileName[] = "GetModulefileNameExA";
    static const char strGetModuleBaseName[] = "GetModuleBaseNameA";
#endif

    tEPM   enumProcessModules = (tEPM)GetProcAddress(hPsapi, "EnumProcessModules");
    tGMFNE getModuleFileNameEx = (tGMFNE)GetProcAddress(hPsapi, strGetModuleFileName);
    tGMBN  getModuleBaseName = (tGMFNE)GetProcAddress(hPsapi, strGetModuleBaseName);
    tGMI   getModuleInformation = (tGMI)GetProcAddress(hPsapi, "GetModuleInformation");
    if ((enumProcessModules == NULL) || (getModuleFileNameEx == NULL) ||
        (getModuleBaseName == NULL) || (getModuleInformation == NULL))
    {
      // we couldn't find all functions
      FreeLibrary(hPsapi);
      return FALSE;
    }

    hMods = (HMODULE*)malloc(sizeof(HMODULE) * (TTBUFLEN / sizeof(HMODULE)));
    tt = (TCHAR*)malloc(sizeof(TCHAR) * TTBUFLEN);
    tt2 = (TCHAR*)malloc(sizeof(TCHAR) * TTBUFLEN);
    if ((hMods == NULL) || (tt == NULL) || (tt2 == NULL))
      goto cleanup;

    if (!enumProcessModules(hProcess, hMods, TTBUFLEN, &cbNeeded))
    {
      //_ftprintf(fLogFile, _T("%lu: EPM failed, GetLastError = %lu\n"), g_dwShowCount, gle );
      goto cleanup;
    }

    if (cbNeeded > TTBUFLEN)
    {
      //_ftprintf(fLogFile, _T("%lu: More than %lu module handles. Huh?\n"), g_dwShowCount, lenof( hMods ) );
      goto cleanup;
    }

    for (DWORD i = 0; i < cbNeeded / sizeof(hMods[0]); i++)
    {
      // base address, size
      getModuleInformation(hProcess, hMods[i], &mi, sizeof(mi));
      // image file name
      tt[0] = 0;
      getModuleFileNameEx(hProcess, hMods[i], tt, TTBUFLEN);
      // module name
      tt2[0] = 0;
      getModuleBaseName(hProcess, hMods[i], tt2, TTBUFLEN);

      DWORD dwRes = this->LoadModule(hProcess, tt, tt2, (DWORD64)mi.lpBaseOfDll, mi.SizeOfImage);
      if (dwRes != ERROR_SUCCESS)
        this->m_parent->OnDbgHelpErr(_T("LoadModule"), dwRes, 0);
      cnt++;
    }

  cleanup:
    if (hPsapi != NULL)
      FreeLibrary(hPsapi);
    if (tt2 != NULL)
      free(tt2);
    if (tt != NULL)
      free(tt);
    if (hMods != NULL)
      free(hMods);

    return cnt != 0;
  } // GetModuleListPSAPI

  DWORD LoadModule(HANDLE hProcess, LPCTSTR img, LPCTSTR mod, DWORD64 baseAddr, DWORD size)
  {
    TCHAR* szImg = _tcsdup(img);
    TCHAR* szMod = _tcsdup(mod);
    DWORD result = ERROR_SUCCESS;
    if ((szImg == NULL) || (szMod == NULL))
      result = ERROR_NOT_ENOUGH_MEMORY;
    else
    {
      if (symLoadModuleEx(hProcess, 0, szImg, szMod, baseAddr, size, 0, 0) == 0)
        result = GetLastError();
    }
    ULONGLONG fileVersion = 0;
    if ((m_parent != NULL) && (szImg != NULL))
    {
      // try to retrieve the file-version:
      if ((this->m_parent->m_options & StackWalker::RetrieveFileVersion) != 0)
      {
        VS_FIXEDFILEINFO* fInfo = NULL;
        DWORD             dwHandle;
        DWORD             dwSize = GetFileVersionInfoSize(szImg, &dwHandle);
        if (dwSize > 0)
        {
          LPVOID vData = malloc(dwSize);
          if (vData != NULL)
          {
            if (GetFileVersionInfo(szImg, dwHandle, dwSize, vData) != 0)
            {
              UINT  len;
              TCHAR szSubBlock[] = _T("\\");
              if (VerQueryValue(vData, szSubBlock, (LPVOID*)&fInfo, &len) == 0)
                fInfo = NULL;
              else
              {
                fileVersion =
                    ((ULONGLONG)fInfo->dwFileVersionLS) + ((ULONGLONG)fInfo->dwFileVersionMS << 32);
              }
            }
            free(vData);
          }
        }
      }

      // Retrieve some additional-infos about the module
      IMAGEHLP_MODULE64_V3 Module;
      const TCHAR*          szSymType = _T("-unknown-");
      if (this->GetModuleInfo(hProcess, baseAddr, &Module) != FALSE)
      {
        switch (Module.SymType)
        {
          case SymNone:
            szSymType = _T("-nosymbols-");
            break;
          case SymCoff: // 1
            szSymType = _T("COFF");
            break;
          case SymCv: // 2
            szSymType = _T("CV");
            break;
          case SymPdb: // 3
            szSymType = _T("PDB");
            break;
          case SymExport: // 4
            szSymType = _T("-exported-");
            break;
          case SymDeferred: // 5
            szSymType = _T("-deferred-");
            break;
          case SymSym: // 6
            szSymType = _T("SYM");
            break;
          case 7: // SymDia:
            szSymType = _T("DIA");
            break;
          case 8: //SymVirtual:
            szSymType = _T("Virtual");
            break;
        }
      }
      LPCTSTR pdbName = Module.LoadedImageName;
      if (Module.LoadedPdbName[0] != 0)
        pdbName = Module.LoadedPdbName;
      this->m_parent->OnLoadModule(img, mod, baseAddr, size, result, szSymType, pdbName,
                                   fileVersion);
    }
    if (szImg != NULL)
      free(szImg);
    if (szMod != NULL)
      free(szMod);
    return result;
  }

public:
  BOOL LoadModules(HANDLE hProcess, DWORD dwProcessId)
  {
    // first try toolhelp32
    if (GetModuleListTH32(hProcess, dwProcessId))
      return true;
    // then try psapi
    return GetModuleListPSAPI(hProcess);
  }

  BOOL GetModuleInfo(HANDLE hProcess, DWORD64 baseAddr, IMAGEHLP_MODULE64_V3* pModuleInfo)
  {
    memset(pModuleInfo, 0, sizeof(IMAGEHLP_MODULE64_V3));
    if (this->symGetModuleInfo64 == NULL)
    {
      SetLastError(ERROR_DLL_INIT_FAILED);
      return FALSE;
    }
    // First try to use the larger ModuleInfo-Structure
    pModuleInfo->SizeOfStruct = sizeof(IMAGEHLP_MODULE64_V3);
    void* pData = malloc(
        4096); // reserve enough memory, so the bug in v6.3.5.1 does not lead to memory-overwrites...
    if (pData == NULL)
    {
      SetLastError(ERROR_NOT_ENOUGH_MEMORY);
      return FALSE;
    }
    memcpy(pData, pModuleInfo, sizeof(IMAGEHLP_MODULE64_V3));
    static bool s_useV3Version = true;
    if (s_useV3Version)
    {
      if (this->symGetModuleInfo64(hProcess, baseAddr, (IMAGEHLP_MODULE64_V3*)pData) != FALSE)
      {
        // only copy as much memory as is reserved...
        memcpy(pModuleInfo, pData, sizeof(IMAGEHLP_MODULE64_V3));
        pModuleInfo->SizeOfStruct = sizeof(IMAGEHLP_MODULE64_V3);
        free(pData);
        return TRUE;
      }
      s_useV3Version = false; // to prevent unnecessary calls with the larger struct...
    }

    // could not retrieve the bigger structure, try with the smaller one (as defined in VC7.1)...
    pModuleInfo->SizeOfStruct = sizeof(IMAGEHLP_MODULE64_V2);
    memcpy(pData, pModuleInfo, sizeof(IMAGEHLP_MODULE64_V2));
    if (this->symGetModuleInfo64(hProcess, baseAddr, (IMAGEHLP_MODULE64_V3*)pData) != FALSE)
    {
      // only copy as much memory as is reserved...
      memcpy(pModuleInfo, pData, sizeof(IMAGEHLP_MODULE64_V2));
      pModuleInfo->SizeOfStruct = sizeof(IMAGEHLP_MODULE64_V2);
      free(pData);
      return TRUE;
    }
    free(pData);
    SetLastError(ERROR_DLL_INIT_FAILED);
    return FALSE;
  }
};

// #############################################################
StackWalker::StackWalker(DWORD dwProcessId, HANDLE hProcess)
{
  this->m_options = OptionsAll;
  this->m_modulesLoaded = FALSE;
  this->m_hProcess = hProcess;
  this->m_sw = new StackWalkerInternal(this, this->m_hProcess);
  this->m_dwProcessId = dwProcessId;
  this->m_szSymPath = NULL;
  this->m_MaxRecursionCount = 1000;
}
StackWalker::StackWalker(int options, LPCTSTR szSymPath, DWORD dwProcessId, HANDLE hProcess)
{
  this->m_options = options;
  this->m_modulesLoaded = FALSE;
  this->m_hProcess = hProcess;
  this->m_sw = new StackWalkerInternal(this, this->m_hProcess);
  this->m_dwProcessId = dwProcessId;
  if (szSymPath != NULL)
  {
    this->m_szSymPath = _tcsdup(szSymPath);
    this->m_options |= SymBuildPath;
  }
  else
    this->m_szSymPath = NULL;
  this->m_MaxRecursionCount = 1000;
}

StackWalker::~StackWalker()
{
  if (m_szSymPath != NULL)
    free(m_szSymPath);
  m_szSymPath = NULL;
  if (this->m_sw != NULL)
    delete this->m_sw;
  this->m_sw = NULL;
}

BOOL StackWalker::LoadModules()
{
  if (this->m_sw == NULL)
  {
    SetLastError(ERROR_DLL_INIT_FAILED);
    return FALSE;
  }
  if (m_modulesLoaded != FALSE)
    return TRUE;

  // Build the sym-path:
  TCHAR* szSymPath = NULL;
  if ((this->m_options & SymBuildPath) != 0)
  {
    const size_t nSymPathLen = 4096;
    szSymPath = (TCHAR*)malloc(nSymPathLen * sizeof(TCHAR));
    if (szSymPath == NULL)
    {
      SetLastError(ERROR_NOT_ENOUGH_MEMORY);
      return FALSE;
    }
    szSymPath[0] = 0;
    // Now first add the (optional) provided sympath:
    if (this->m_szSymPath != NULL)
    {
      _tcscat_s(szSymPath, nSymPathLen, this->m_szSymPath);
      _tcscat_s(szSymPath, nSymPathLen, _T(";"));
    }

    _tcscat_s(szSymPath, nSymPathLen, _T(".;"));

    const size_t nTempLen = 1024;
    TCHAR         szTemp[nTempLen];
    // Now add the current directory:
    if (GetCurrentDirectory(nTempLen, szTemp) > 0)
    {
      szTemp[nTempLen - 1] = 0;
      _tcscat_s(szSymPath, nSymPathLen, szTemp);
      _tcscat_s(szSymPath, nSymPathLen, _T(";"));
    }

    // Now add the path for the main-module:
    if (GetModuleFileName(NULL, szTemp, nTempLen) > 0)
    {
      szTemp[nTempLen - 1] = 0;
      for (TCHAR* p = (szTemp + _tcslen(szTemp) - 1); p >= szTemp; --p)
      {
        // locate the rightmost path separator
        if ((*p == '\\') || (*p == '/') || (*p == ':'))
        {
          *p = 0;
          break;
        }
      } // for (search for path separator...)
      if (_tcslen(szTemp) > 0)
      {
        _tcscat_s(szSymPath, nSymPathLen, szTemp);
        _tcscat_s(szSymPath, nSymPathLen, _T(";"));
      }
    }
    if (GetEnvironmentVariable(_T("_NT_SYMBOL_PATH"), szTemp, nTempLen) > 0)
    {
      szTemp[nTempLen - 1] = 0;
      _tcscat_s(szSymPath, nSymPathLen, szTemp);
      _tcscat_s(szSymPath, nSymPathLen, _T(";"));
    }
    if (GetEnvironmentVariable(_T("_NT_ALTERNATE_SYMBOL_PATH"), szTemp, nTempLen) > 0)
    {
      szTemp[nTempLen - 1] = 0;
      _tcscat_s(szSymPath, nSymPathLen, szTemp);
      _tcscat_s(szSymPath, nSymPathLen, _T(";"));
    }
    if (GetEnvironmentVariable(_T("SYSTEMROOT"), szTemp, nTempLen) > 0)
    {
      szTemp[nTempLen - 1] = 0;
      _tcscat_s(szSymPath, nSymPathLen, szTemp);
      _tcscat_s(szSymPath, nSymPathLen, _T(";"));
      // also add the "system32"-directory:
      _tcscat_s(szTemp, nTempLen, _T("\\system32"));
      _tcscat_s(szSymPath, nSymPathLen, szTemp);
      _tcscat_s(szSymPath, nSymPathLen, _T(";"));
    }

    if ((this->m_options & SymUseSymSrv) != 0)
    {
      if (GetEnvironmentVariable(_T("SYSTEMDRIVE"), szTemp, nTempLen) > 0)
      {
        szTemp[nTempLen - 1] = 0;
        _tcscat_s(szSymPath, nSymPathLen, _T("SRV*"));
        _tcscat_s(szSymPath, nSymPathLen, szTemp);
        _tcscat_s(szSymPath, nSymPathLen, _T("\\websymbols"));
        _tcscat_s(szSymPath, nSymPathLen, _T("*http://msdl.microsoft.com/download/symbols;"));
      }
      else
        _tcscat_s(szSymPath, nSymPathLen,
                 _T("SRV*c:\\websymbols*http://msdl.microsoft.com/download/symbols;"));
    }
  } // if SymBuildPath

  // First Init the whole stuff...
  BOOL bRet = this->m_sw->Init(szSymPath);
  if (szSymPath != NULL)
    free(szSymPath);
  szSymPath = NULL;
  if (bRet == FALSE)
  {
    this->OnDbgHelpErr(_T("Error while initializing dbghelp.dll"), 0, 0);
    SetLastError(ERROR_DLL_INIT_FAILED);
    return FALSE;
  }

  bRet = this->m_sw->LoadModules(this->m_hProcess, this->m_dwProcessId);
  if (bRet != FALSE)
    m_modulesLoaded = TRUE;
  return bRet;
}

// The following is used to pass the "userData"-Pointer to the user-provided readMemoryFunction
// This has to be done due to a problem with the "hProcess"-parameter in x64...
// Because this class is in no case multi-threading-enabled (because of the limitations
// of dbghelp.dll) it is "safe" to use a static-variable
static StackWalker::PReadProcessMemoryRoutine s_readMemoryFunction = NULL;
static LPVOID                                 s_readMemoryFunction_UserData = NULL;

BOOL StackWalker::ShowCallstack(HANDLE                    hThread,
                                const CONTEXT*            context,
                                PReadProcessMemoryRoutine readMemoryFunction,
                                LPVOID                    pUserData)
{
  CONTEXT                                   c;
  CallstackEntry                            csEntry;

  tSymbolInfo* pSym = NULL;
  StackWalkerInternal::IMAGEHLP_MODULE64_V3 Module;
  tImageHelperLine                           Line;
  int                                       frameNum;
  bool                                      bLastEntryCalled = true;
  int                                       curRecursionCount = 0;

  if (m_modulesLoaded == FALSE)
    this->LoadModules(); // ignore the result...

  if (this->m_sw->m_hDbhHelp == NULL)
  {
    SetLastError(ERROR_DLL_INIT_FAILED);
    return FALSE;
  }

  s_readMemoryFunction = readMemoryFunction;
  s_readMemoryFunction_UserData = pUserData;

  if (context == NULL)
  {
    // If no context is provided, capture the context
    // See: https://stackwalker.codeplex.com/discussions/446958
#if _WIN32_WINNT <= 0x0501
    // If we need to support XP, we need to use the "old way", because "GetThreadId" is not available!
    if (hThread == GetCurrentThread())
#else
    if (GetThreadId(hThread) == GetCurrentThreadId())
#endif
    {
      GET_CURRENT_CONTEXT_STACKWALKER_CODEPLEX(c, USED_CONTEXT_FLAGS);
    }
    else
    {
      SuspendThread(hThread);
      memset(&c, 0, sizeof(CONTEXT));
      c.ContextFlags = USED_CONTEXT_FLAGS;

      // TODO: Detect if you want to get a thread context of a different process, which is running a different processor architecture...
      // This does only work if we are x64 and the target process is x64 or x86;
      // It cannot work, if this process is x64 and the target process is x64... this is not supported...
      // See also: http://www.howzatt.demon.co.uk/articles/DebuggingInWin64.html
      if (GetThreadContext(hThread, &c) == FALSE)
      {
        ResumeThread(hThread);
        return FALSE;
      }
    }
  }
  else
    c = *context;

  // init STACKFRAME for first call
  STACKFRAME64 s; // in/out stackframe
  memset(&s, 0, sizeof(s));
  DWORD imageType;
#ifdef _M_IX86
  // normally, call ImageNtHeader() and use machine info from PE header
  imageType = IMAGE_FILE_MACHINE_I386;
  s.AddrPC.Offset = c.Eip;
  s.AddrPC.Mode = AddrModeFlat;
  s.AddrFrame.Offset = c.Ebp;
  s.AddrFrame.Mode = AddrModeFlat;
  s.AddrStack.Offset = c.Esp;
  s.AddrStack.Mode = AddrModeFlat;
#elif _M_X64
  imageType = IMAGE_FILE_MACHINE_AMD64;
  s.AddrPC.Offset = c.Rip;
  s.AddrPC.Mode = AddrModeFlat;
  s.AddrFrame.Offset = c.Rsp;
  s.AddrFrame.Mode = AddrModeFlat;
  s.AddrStack.Offset = c.Rsp;
  s.AddrStack.Mode = AddrModeFlat;
#elif _M_IA64
  imageType = IMAGE_FILE_MACHINE_IA64;
  s.AddrPC.Offset = c.StIIP;
  s.AddrPC.Mode = AddrModeFlat;
  s.AddrFrame.Offset = c.IntSp;
  s.AddrFrame.Mode = AddrModeFlat;
  s.AddrBStore.Offset = c.RsBSP;
  s.AddrBStore.Mode = AddrModeFlat;
  s.AddrStack.Offset = c.IntSp;
  s.AddrStack.Mode = AddrModeFlat;
#else
#error "Platform not supported!"
#endif

  pSym = (tSymbolInfo*)malloc(sizeof(tSymbolInfo) + STACKWALK_MAX_NAMELEN*sizeof(TCHAR));
  if (!pSym)
    goto cleanup; // not enough memory...
  memset(pSym, 0, sizeof(tSymbolInfo) + STACKWALK_MAX_NAMELEN*sizeof(TCHAR));
  pSym->SizeOfStruct = sizeof(tSymbolInfo);
  pSym->MaxNameLen = STACKWALK_MAX_NAMELEN;

  memset(&Line, 0, sizeof(Line));
  Line.SizeOfStruct = sizeof(Line);

  memset(&Module, 0, sizeof(Module));
  Module.SizeOfStruct = sizeof(Module);

  for (frameNum = 0;; ++frameNum)
  {
    // get next stack frame (StackWalk64(), SymFunctionTableAccess64(), SymGetModuleBase64())
    // if this returns ERROR_INVALID_ADDRESS (487) or ERROR_NOACCESS (998), you can
    // assume that either you are done, or that the stack is so hosed that the next
    // deeper frame could not be found.
    // CONTEXT need not to be supplied if imageTyp is IMAGE_FILE_MACHINE_I386!
    if (!this->m_sw->stackWalk64(imageType, this->m_hProcess, hThread, &s, &c, myReadProcMem,
                         this->m_sw->symFunctionTableAccess64, this->m_sw->symGetModuleBase64, NULL))
    {
      // INFO: "StackWalk64" does not set "GetLastError"...
      this->OnDbgHelpErr(_T("StackWalk64"), 0, s.AddrPC.Offset);
      break;
    }

    csEntry.offset = s.AddrPC.Offset;
    csEntry.name[0] = 0;
    csEntry.undName[0] = 0;
    csEntry.undFullName[0] = 0;
    csEntry.offsetFromSmybol = 0;
    csEntry.offsetFromLine = 0;
    csEntry.lineFileName[0] = 0;
    csEntry.lineNumber = 0;
    csEntry.loadedImageName[0] = 0;
    csEntry.moduleName[0] = 0;
    if (s.AddrPC.Offset == s.AddrReturn.Offset)
    {
      if ((this->m_MaxRecursionCount > 0) && (curRecursionCount > m_MaxRecursionCount))
      {
        this->OnDbgHelpErr(_T("StackWalk64-Endless-Callstack!"), 0, s.AddrPC.Offset);
        break;
      }
      curRecursionCount++;
    }
    else
      curRecursionCount = 0;
    if (s.AddrPC.Offset != 0)
    {
      // we seem to have a valid PC
      // show procedure info (SymGetSymFromAddr64())
      if (this->m_sw->symFromAddr(this->m_hProcess, s.AddrPC.Offset, &(csEntry.offsetFromSmybol),
                             pSym) != FALSE)
      {
        MyStrCpy(csEntry.name, STACKWALK_MAX_NAMELEN, pSym->Name);
        // UnDecorateSymbolName()
        DWORD res = this->m_sw->unDecorateSymbolName(pSym->Name, csEntry.undName, STACKWALK_MAX_NAMELEN, UNDNAME_NAME_ONLY);
        res = this->m_sw->unDecorateSymbolName(pSym->Name, csEntry.undFullName, STACKWALK_MAX_NAMELEN, UNDNAME_COMPLETE);
      }
      else
      {
        this->OnDbgHelpErr(_T("SymGetSymFromAddr64"), GetLastError(), s.AddrPC.Offset);
      }

      // show line number info, NT5.0-method (SymGetLineFromAddr64())
      if (this->m_sw->symGetLineFromAddr64 != NULL)
      { // yes, we have SymGetLineFromAddr64()
        if (this->m_sw->symGetLineFromAddr64(this->m_hProcess, s.AddrPC.Offset, &(csEntry.offsetFromLine),
                               &Line) != FALSE)
        {
          csEntry.lineNumber = Line.LineNumber;
          MyStrCpy(csEntry.lineFileName, STACKWALK_MAX_NAMELEN, Line.FileName);
        }
        else
        {
          this->OnDbgHelpErr(_T("SymGetLineFromAddr64"), GetLastError(), s.AddrPC.Offset);
        }
      } // yes, we have SymGetLineFromAddr64()

      // show module info (SymGetModuleInfo64())
      if (this->m_sw->GetModuleInfo(this->m_hProcess, s.AddrPC.Offset, &Module) != FALSE)
      { // got module info OK
        switch (Module.SymType)
        {
          case SymNone:
            csEntry.symTypeString = "-nosymbols-";
            break;
          case SymCoff:
            csEntry.symTypeString = "COFF";
            break;
          case SymCv:
            csEntry.symTypeString = "CV";
            break;
          case SymPdb:
            csEntry.symTypeString = "PDB";
            break;
          case SymExport:
            csEntry.symTypeString = "-exported-";
            break;
          case SymDeferred:
            csEntry.symTypeString = "-deferred-";
            break;
          case SymSym:
            csEntry.symTypeString = "SYM";
            break;
#if API_VERSION_NUMBER >= 9
          case SymDia:
            csEntry.symTypeString = "DIA";
            break;
#endif
          case 8: //SymVirtual:
            csEntry.symTypeString = "Virtual";
            break;
          default:
            //_snprintf( ty, sizeof(ty), "symtype=%ld", (long) Module.SymType );
            csEntry.symTypeString = NULL;
            break;
        }

        MyStrCpy(csEntry.moduleName, STACKWALK_MAX_NAMELEN, Module.ModuleName);
        csEntry.baseOfImage = Module.BaseOfImage;
        MyStrCpy(csEntry.loadedImageName, STACKWALK_MAX_NAMELEN, Module.LoadedImageName);
      } // got module info OK
      else
      {
        this->OnDbgHelpErr(_T("SymGetModuleInfo64"), GetLastError(), s.AddrPC.Offset);
      }
    } // we seem to have a valid PC

    CallstackEntryType et = nextEntry;
    if (frameNum == 0)
      et = firstEntry;
    bLastEntryCalled = false;
    this->OnCallstackEntry(et, csEntry);

    if (s.AddrReturn.Offset == 0)
    {
      bLastEntryCalled = true;
      this->OnCallstackEntry(lastEntry, csEntry);
      SetLastError(ERROR_SUCCESS);
      break;
    }
  } // for ( frameNum )

cleanup:
  if (pSym)
    free(pSym);

  if (bLastEntryCalled == false)
    this->OnCallstackEntry(lastEntry, csEntry);

  if (context == NULL)
    ResumeThread(hThread);

  return TRUE;
}

BOOL StackWalker::ShowObject(LPVOID pObject)
{
  // Load modules if not done yet
  if (m_modulesLoaded == FALSE)
    this->LoadModules(); // ignore the result...

  // Verify that the DebugHelp.dll was actually found
  if (this->m_sw->m_hDbhHelp == NULL)
  {
    SetLastError(ERROR_DLL_INIT_FAILED);
    return FALSE;
  }

  // SymGetSymFromAddr64() is required
  if (this->m_sw->symFromAddr == NULL)
    return FALSE;

  // Show object info (SymGetSymFromAddr64())
  DWORD64            dwAddress = DWORD64(pObject);
  DWORD64            dwDisplacement = 0;
  tSymbolInfo* pSym =
      (tSymbolInfo*)malloc(sizeof(tSymbolInfo) + STACKWALK_MAX_NAMELEN*sizeof(TCHAR));
  memset(pSym, 0, sizeof(tSymbolInfo) + STACKWALK_MAX_NAMELEN*sizeof(TCHAR));
  pSym->SizeOfStruct = sizeof(tSymbolInfo);
  pSym->MaxNameLen = STACKWALK_MAX_NAMELEN;
  if (this->m_sw->symFromAddr(this->m_hProcess, dwAddress, &dwDisplacement, pSym) == FALSE)
  {
    this->OnDbgHelpErr(_T("SymGetSymFromAddr64"), GetLastError(), dwAddress);
    return FALSE;
  }
  // Object name output
  this->OnOutput(pSym->Name);

  free(pSym);
  return TRUE;
};

BOOL __stdcall StackWalker::myReadProcMem(HANDLE  hProcess,
                                          DWORD64 qwBaseAddress,
                                          PVOID   lpBuffer,
                                          DWORD   nSize,
                                          LPDWORD lpNumberOfBytesRead)
{
  if (s_readMemoryFunction == NULL)
  {
    SIZE_T st;
    BOOL   bRet = ReadProcessMemory(hProcess, (LPVOID)qwBaseAddress, lpBuffer, nSize, &st);
    *lpNumberOfBytesRead = (DWORD)st;
    //printf("ReadMemory: hProcess: %p, baseAddr: %p, buffer: %p, size: %d, read: %d, result: %d\n", hProcess, (LPVOID) qwBaseAddress, lpBuffer, nSize, (DWORD) st, (DWORD) bRet);
    return bRet;
  }
  else
  {
    return s_readMemoryFunction(hProcess, qwBaseAddress, lpBuffer, nSize, lpNumberOfBytesRead,
                                s_readMemoryFunction_UserData);
  }
}

void StackWalker::OnLoadModule(LPCTSTR    img,
                               LPCTSTR    mod,
                               DWORD64   baseAddr,
                               DWORD     size,
                               DWORD     result,
                               LPCTSTR    symType,
                               LPCTSTR    pdbName,
                               ULONGLONG fileVersion)
{
  TCHAR   buffer[STACKWALK_MAX_NAMELEN];
  size_t maxLen = STACKWALK_MAX_NAMELEN;
#if _MSC_VER >= 1400
  maxLen = _TRUNCATE;
#endif
  if (fileVersion == 0)
    _sntprintf_s(buffer, maxLen, _T("%s:%s (%p), size: %d (result: %d), SymType: '%s', PDB: '%s'\n"),
                img, mod, (LPVOID)baseAddr, size, result, symType, pdbName);
  else
  {
    DWORD v4 = (DWORD)(fileVersion & 0xFFFF);
    DWORD v3 = (DWORD)((fileVersion >> 16) & 0xFFFF);
    DWORD v2 = (DWORD)((fileVersion >> 32) & 0xFFFF);
    DWORD v1 = (DWORD)((fileVersion >> 48) & 0xFFFF);
    _sntprintf_s(
        buffer, maxLen,
        _T("%s:%s (%p), size: %d (result: %d), SymType: '%s', PDB: '%s', fileVersion: %d.%d.%d.%d\n"),
        img, mod, (LPVOID)baseAddr, size, result, symType, pdbName, v1, v2, v3, v4);
  }
  buffer[STACKWALK_MAX_NAMELEN - 1] = 0; // be sure it is NULL terminated
  OnOutput(buffer);
}

void StackWalker::OnCallstackEntry(CallstackEntryType eType, CallstackEntry& entry)
{
  TCHAR   buffer[STACKWALK_MAX_NAMELEN];
  size_t maxLen = STACKWALK_MAX_NAMELEN;
#if _MSC_VER >= 1400
  maxLen = _TRUNCATE;
#endif
  if ((eType != lastEntry) && (entry.offset != 0))
  {
    if (entry.name[0] == 0)
      MyStrCpy(entry.name, STACKWALK_MAX_NAMELEN, _T("(function-name not available)"));
    if (entry.undName[0] != 0)
      MyStrCpy(entry.name, STACKWALK_MAX_NAMELEN, entry.undName);
    if (entry.undFullName[0] != 0)
      MyStrCpy(entry.name, STACKWALK_MAX_NAMELEN, entry.undFullName);
    if (entry.lineFileName[0] == 0)
    {
      MyStrCpy(entry.lineFileName, STACKWALK_MAX_NAMELEN, _T("(filename not available)"));
      if (entry.moduleName[0] == 0)
        MyStrCpy(entry.moduleName, STACKWALK_MAX_NAMELEN, _T("(module-name not available)"));
      _sntprintf_s(buffer, maxLen, _T("%p (%s): %s: %s\n"), (LPVOID)entry.offset, entry.moduleName,
                  entry.lineFileName, entry.name);
    }
    else
      _sntprintf_s(buffer, maxLen, _T("%s (%d): %s\n"), entry.lineFileName, entry.lineNumber,
                  entry.name);
    buffer[STACKWALK_MAX_NAMELEN - 1] = 0;
    OnOutput(buffer);
  }
}

void StackWalker::OnDbgHelpErr(LPCTSTR szFuncName, DWORD gle, DWORD64 addr)
{
  TCHAR   buffer[STACKWALK_MAX_NAMELEN];
  size_t maxLen = STACKWALK_MAX_NAMELEN;
#if _MSC_VER >= 1400
  maxLen = _TRUNCATE;
#endif
  _sntprintf_s(buffer, maxLen, _T("ERROR: %s, GetLastError: %d (Address: %p)\n"), szFuncName, gle,
              (LPVOID)addr);
  buffer[STACKWALK_MAX_NAMELEN - 1] = 0;
  OnOutput(buffer);
}

void StackWalker::OnSymInit(LPCTSTR szSearchPath, DWORD symOptions, LPCTSTR szUserName)
{
  TCHAR   buffer[STACKWALK_MAX_NAMELEN];
  size_t maxLen = STACKWALK_MAX_NAMELEN;
#if _MSC_VER >= 1400
  maxLen = _TRUNCATE;
#endif
  _sntprintf_s(buffer, maxLen, _T("SymInit: Symbol-SearchPath: '%s', symOptions: %d, UserName: '%s'\n"),
              szSearchPath, symOptions, szUserName);
  buffer[STACKWALK_MAX_NAMELEN - 1] = 0;
  OnOutput(buffer);
  // Also display the OS-version
#if _MSC_VER <= 1200
  OSVERSIONINFOA ver;
  ZeroMemory(&ver, sizeof(OSVERSIONINFOA));
  ver.dwOSVersionInfoSize = sizeof(ver);
  if (GetVersionExA(&ver) != FALSE)
  {
    _snprintf_s(buffer, maxLen, "OS-Version: %d.%d.%d (%s)\n", ver.dwMajorVersion,
                ver.dwMinorVersion, ver.dwBuildNumber, ver.szCSDVersion);
    buffer[STACKWALK_MAX_NAMELEN - 1] = 0;
    OnOutput(buffer);
  }
#else
  OSVERSIONINFOEX ver;
  ZeroMemory(&ver, sizeof(OSVERSIONINFOEX));
  ver.dwOSVersionInfoSize = sizeof(ver);
#if _MSC_VER >= 1900
#pragma warning(push)
#pragma warning(disable : 4996)
#endif
  if (GetVersionEx((OSVERSIONINFO*)&ver) != FALSE)
  {
    _sntprintf_s(buffer, maxLen, _T("OS-Version: %d.%d.%d (%s) 0x%x-0x%x\n"), ver.dwMajorVersion,
                ver.dwMinorVersion, ver.dwBuildNumber, ver.szCSDVersion, ver.wSuiteMask,
                ver.wProductType);
    buffer[STACKWALK_MAX_NAMELEN - 1] = 0;
    OnOutput(buffer);
  }
#if _MSC_VER >= 1900
#pragma warning(pop)
#endif
#endif
}

void StackWalker::OnOutput(LPCTSTR buffer)
{
  OutputDebugString(buffer);
}
