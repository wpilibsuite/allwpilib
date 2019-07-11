/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "tables/ITableListener.h"

#include "ntcore_c.h"

void ITableListener::ValueChangedEx(ITable* source, wpi::StringRef key,
                                    std::shared_ptr<nt::Value> value,
                                    unsigned int flags) {
  ValueChanged(source, key, value, (flags & NT_NOTIFY_NEW) != 0);
}
