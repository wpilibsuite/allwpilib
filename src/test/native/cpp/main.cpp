/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "gtest/gtest.h"

#include "Log.h"

int main(int argc, char **argv)
{
  nt::Logger::GetInstance().SetLogger(
      [](unsigned int level, const char* file, unsigned int line,
         const char* msg) {
        std::fputs(msg, stderr);
        std::fputc('\n', stderr);
      });
  nt::Logger::GetInstance().set_min_level(0);
    ::testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();
    return ret;
}
