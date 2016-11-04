/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef TABLEKEYNOTDEFINEDEXCEPTION_H_
#define TABLEKEYNOTDEFINEDEXCEPTION_H_

#include <exception>
#include "llvm/StringRef.h"

#if defined(_MSC_VER)
#define NT_NOEXCEPT throw()
#else
#define NT_NOEXCEPT noexcept
#endif

/**
 * An exception thrown when the lookup a a key-value fails in a {@link ITable}
 */
class TableKeyNotDefinedException : public std::exception {
 public:
  /**
   * @param key the key that was not defined in the table
   */
  TableKeyNotDefinedException(llvm::StringRef key);
  ~TableKeyNotDefinedException() NT_NOEXCEPT;
  const char* what() const NT_NOEXCEPT override;

 private:
  std::string msg;
};

#endif  // TABLEKEYNOTDEFINEDEXCEPTION_H_
