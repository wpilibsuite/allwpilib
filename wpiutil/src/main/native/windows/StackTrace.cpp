// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/StackTrace.h"

#include "StackWalker.h"
#include "wpi/ConvertUTF.h"
#include "wpi/SmallString.h"

#if defined(_MSC_VER)

namespace {
class StackTraceWalker : public StackWalker {
 public:
  explicit StackTraceWalker(std::string& output) : m_output(output) {}

  void OnOutput(LPCTSTR szText) override;

 private:
  std::string& m_output;
};
}  // namespace

void StackTraceWalker::OnOutput(LPCTSTR szText) {
#ifdef _UNICODE
  wpi::SmallString<128> utf8;
  wpi::sys::windows::UTF16ToUTF8(szText, wcslen(szText), utf8);
  m_output.append(utf8.data(), utf8.size());
#else
  m_output.append(szText);
#endif
}

namespace wpi {

std::string GetStackTraceDefault(int offset) {
  // TODO: implement offset
  std::string output;
  StackTraceWalker walker(output);
  return output;
}

}  // namespace wpi

#endif  // defined(_MSC_VER)
