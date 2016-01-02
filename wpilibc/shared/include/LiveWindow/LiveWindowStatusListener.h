/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2012-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef _LIVE_WINDOW_STATUS_LISTENER_H
#define _LIVE_WINDOW_STATUS_LISTENER_H

#include "tables/ITable.h"
#include "tables/ITableListener.h"

class LiveWindowStatusListener : public ITableListener {
 public:
  virtual void ValueChanged(ITable* source, llvm::StringRef key,
                            std::shared_ptr<nt::Value> value, bool isNew);
};

#endif
