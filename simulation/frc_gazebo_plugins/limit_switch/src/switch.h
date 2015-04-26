#pragma once

class Switch {
public:
  virtual ~Switch() {}

  /// \brief Returns true when the switch is triggered.
  virtual bool Get() = 0;
};
