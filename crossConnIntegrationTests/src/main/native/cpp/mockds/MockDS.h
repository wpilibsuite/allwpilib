// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <atomic>
#include <thread>

class MockDS {
 public:
  MockDS() = default;
  ~MockDS() { Stop(); }
  MockDS(const MockDS& other) = delete;
  MockDS& operator=(const MockDS& other) = delete;

  void Start();
  void Stop();

 private:
  std::thread m_thread;
  std::atomic_bool m_active{false};
};
