/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "wpi/StackTrace.h"

#include <atlstr.h>

#include "StackWalker.h"

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
  m_output.append(CT2A(szText));
}

namespace wpi {

std::string GetStackTrace(int offset) {
  // TODO: implement offset
  std::string output;
  StackTraceWalker walker(output);
  return output;
}

}  // namespace wpi
