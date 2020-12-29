/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "wpi/DataLog.h"

int main() {
  {
    auto log1 = wpi::log::DataLog::Open("test.log");
    if (!log1) {
      wpi::errs() << "could not open log\n";
      return EXIT_FAILURE;
    }
    auto log = wpi::log::DoubleLog::Wrap(*log1);
    if (!log) {
      wpi::errs() << "log is not a double log\n";
      return EXIT_FAILURE;
    }
    for (auto&& p : *log) {
      wpi::outs() << "TS=" << p.first << " Value=" << p.second << '\n';
    }

    {
      auto it = log->find(600000);
      wpi::outs() << "found 600000: TS=" << it->first << " Value=" << it->second
                  << '\n';
    }

    {
      auto it = log->find(600001);
      wpi::outs() << "found 600001: TS=" << it->first << " Value=" << it->second
                  << '\n';
    }

    {
      auto it = log->find(599999);
      wpi::outs() << "found 599999: TS=" << it->first << " Value=" << it->second
                  << '\n';
    }

    {
      auto it = log->find(120001, log->begin() + 2, log->begin() + 20);
      wpi::outs() << "found 120001: TS=" << it->first << " Value=" << it->second
                  << '\n';
    }
  }
  {
    auto log =
        wpi::log::StringLog::Open("test-string.log", wpi::log::CD_OpenExisting);
    if (!log) {
      wpi::errs() << "could not open log\n";
      return EXIT_FAILURE;
    }
    for (auto&& p : *log) {
      wpi::outs() << "TS=" << p.first << " Value=" << p.second << '\n';
    }
  }
  {
    auto log = wpi::log::DoubleArrayLog::Open("test-double-array.log",
                                              wpi::log::CD_OpenExisting);
    if (!log) {
      wpi::errs() << "could not open log\n";
      return EXIT_FAILURE;
    }
    for (auto&& p : *log) {
      wpi::outs() << "TS=" << p.first << " Value Len=" << p.second.size()
                  << '\n';
      for (auto&& v : p.second) {
        wpi::outs() << "  " << v << '\n';
      }
    }
  }
  {
    auto log = wpi::log::StringArrayLog::Open("test-string-array.log",
                                              wpi::log::CD_OpenExisting);
    if (!log) {
      wpi::errs() << "could not open log\n";
      return EXIT_FAILURE;
    }
    for (auto&& p : *log) {
      wpi::outs() << "TS=" << p.first << " Value Len=" << p.second.size()
                  << '\n';
      for (auto&& v : p.second) {
        wpi::outs() << " " << v << '\n';
      }
    }
    wpi::SmallVector<wpi::StringRef, 4> buf;
    for (auto&& v : log->Get(0, buf).second) {
      wpi::outs() << " " << v << '\n';
    }
  }
}
