/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef NTCORE_TABLES_ITABLELISTENER_H_
#define NTCORE_TABLES_ITABLELISTENER_H_

#include <memory>

#include <wpi/StringRef.h>
#include <wpi/deprecated.h>

#include "networktables/NetworkTableValue.h"

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

class ITable;

/**
 * A listener that listens to changes in values in a {@link ITable}
 */
class WPI_DEPRECATED(
    "Use EntryListener, TableEntryListener, or TableListener as appropriate")
    ITableListener {
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
  virtual void ValueChanged(ITable* source, wpi::StringRef key,
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
  virtual void ValueChangedEx(ITable* source, wpi::StringRef key,
                              std::shared_ptr<nt::Value> value,
                              unsigned int flags);
};

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

#endif  // NTCORE_TABLES_ITABLELISTENER_H_
