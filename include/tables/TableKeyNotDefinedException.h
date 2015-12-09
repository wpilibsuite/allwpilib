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

/**
 * An exception thrown when the lookup a a key-value fails in a {@link ITable}
 */
class TableKeyNotDefinedException : public std::exception {
 public:
  /**
   * @param key the key that was not defined in the table
   */
  TableKeyNotDefinedException(llvm::StringRef key);
  ~TableKeyNotDefinedException() noexcept;
  const char* what() const noexcept override;

 private:
  std::string msg;
};

#endif  // TABLEKEYNOTDEFINEDEXCEPTION_H_
