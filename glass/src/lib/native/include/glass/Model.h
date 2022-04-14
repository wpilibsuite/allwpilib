// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

namespace glass {

class Model {
 public:
  Model() = default;
  virtual ~Model() = default;

  Model(const Model&) = delete;
  Model& operator=(const Model&) = delete;

  virtual void Update() = 0;
  virtual bool Exists() = 0;
  virtual bool IsReadOnly();
};

}  // namespace glass
