/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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
