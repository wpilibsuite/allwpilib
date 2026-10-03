// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <string>
#include <vector>

#include "Cli.hpp"
#include "wpi/util/raw_ostream.hpp"

#ifndef RUNNING_DATALOGTOOL_TESTS
int main(int argc, char** argv) {
  std::vector<std::string> args{argv + 1, argv + argc};
  auto& out = wpi::util::outs();
  auto& err = wpi::util::errs();
  int result = dlt::RunCli(args, out, err);
  out.flush();
  err.flush();
  if (out.has_error() || err.has_error()) {
    result = 1;
    out.clear_error();
    err.clear_error();
  }
  return result;
}
#endif
