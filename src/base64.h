/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef NT_BASE64_H_
#define NT_BASE64_H_

#include <cstddef>

namespace ntimpl {

std::size_t base64decode_len(const char *bufcoded);
std::size_t base64decode(char *bufplain, const char *bufcoded);
std::size_t base64encode_len(std::size_t len);
std::size_t base64encode(char *encoded, const unsigned char *string,
                         std::size_t len);

}  // namespace ntimpl

#endif  // NT_BASE64_H_
