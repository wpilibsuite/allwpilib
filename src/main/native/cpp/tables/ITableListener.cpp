#include "tables/ITableListener.h"

#include "ntcore_c.h"

void ITableListener::ValueChangedEx(ITable* source, llvm::StringRef key,
                                    std::shared_ptr<nt::Value> value,
                                    unsigned int flags) {
  ValueChanged(source, key, value, (flags & NT_NOTIFY_NEW) != 0);
}
