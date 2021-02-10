// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "tables/ITableListener.h"

#include "ntcore_c.h"

void ITableListener::ValueChangedEx(ITable* source, wpi::StringRef key,
                                    std::shared_ptr<nt::Value> value,
                                    unsigned int flags) {
  ValueChanged(source, key, value, (flags & NT_NOTIFY_NEW) != 0);
}
