// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/StackTrace.h"

#include <string>

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
  m_output.append(szText);
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
