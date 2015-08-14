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
