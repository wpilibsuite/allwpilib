/*
 * ITableListener.h
 */

#ifndef ITABLELISTENER_H_
#define ITABLELISTENER_H_

#include <memory>

#include "llvm/StringRef.h"
#include "nt_Value.h"

class ITable;

/**
 * A listener that listens to changes in values in a {@link ITable}
 */
class ITableListener {
 public:
  virtual ~ITableListener() = default;
  /**
   * Called when a key-value pair is changed in a {@link ITable}
   * @param source the table the key-value pair exists in
   * @param key the key associated with the value that changed
   * @param value the new value
   * @param isNew true if the key did not previously exist in the table,
   * otherwise it is false
   */
  virtual void ValueChanged(ITable* source, llvm::StringRef key,
                            std::shared_ptr<nt::Value> value, bool isNew) = 0;

  /**
   * Extended version of ValueChanged.  Called when a key-value pair is
   * changed in a {@link ITable}.  The default implementation simply calls
   * ValueChanged().  If this is overridden, ValueChanged() will not be called.
   * @param source the table the key-value pair exists in
   * @param key the key associated with the value that changed
   * @param value the new value
   * @param flags update flags; for example, NT_NOTIFY_NEW if the key did not
   * previously exist in the table
   */
  virtual void ValueChangedEx(ITable* source, llvm::StringRef key,
                              std::shared_ptr<nt::Value> value,
                              unsigned int flags);
};

#endif /* ITABLELISTENER_H_ */
