/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

/* ====================================================================
 * Copyright (c) 1995-1999 The Apache Group.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. All advertising materials mentioning features or use of this
 *    software must display the following acknowledgment:
 *    "This product includes software developed by the Apache Group
 *    for use in the Apache HTTP server project (http://www.apache.org/)."
 *
 * 4. The names "Apache Server" and "Apache Group" must not be used to
 *    endorse or promote products derived from this software without
 *    prior written permission. For written permission, please contact
 *    apache@apache.org.
 *
 * 5. Products derived from this software may not be called "Apache"
 *    nor may "Apache" appear in their names without prior written
 *    permission of the Apache Group.
 *
 * 6. Redistributions of any form whatsoever must retain the following
 *    acknowledgment:
 *    "This product includes software developed by the Apache Group
 *    for use in the Apache HTTP server project (http://www.apache.org/)."
 *
 * THIS SOFTWARE IS PROVIDED BY THE APACHE GROUP ``AS IS'' AND ANY
 * EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE APACHE GROUP OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 * ====================================================================
 *
 * This software consists of voluntary contributions made by many
 * individuals on behalf of the Apache Group and was originally based
 * on public domain software written at the National Center for
 * Supercomputing Applications, University of Illinois, Urbana-Champaign.
 * For more information on the Apache Group and the Apache HTTP server
 * project, please see <http://www.apache.org/>.
 *
 */

#include "support/Base64.h"

namespace wpi {

// aaaack but it's fast and const should make it shared text page.
static const unsigned char pr2six[256] =
{
    // ASCII table
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 62, 64, 64, 64, 63,
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 64, 64, 64, 64, 64, 64,
    64,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64, 64,
    64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64
};

std::size_t Base64Decode(llvm::StringRef encoded, std::string* plain) {
  const unsigned char *end = encoded.bytes_begin();
  while (pr2six[*end] <= 63 && end != encoded.bytes_end()) ++end;
  std::size_t nprbytes = end - encoded.bytes_begin();

  plain->clear();
  if (nprbytes == 0)
    return 0;
  plain->reserve(((nprbytes + 3) / 4) * 3);

  const unsigned char *cur = encoded.bytes_begin();

  while (nprbytes > 4) {
    (*plain) += (pr2six[cur[0]] << 2 | pr2six[cur[1]] >> 4);
    (*plain) += (pr2six[cur[1]] << 4 | pr2six[cur[2]] >> 2);
    (*plain) += (pr2six[cur[2]] << 6 | pr2six[cur[3]]);
    cur += 4;
    nprbytes -= 4;
  }

  // Note: (nprbytes == 1) would be an error, so just ignore that case
  if (nprbytes > 1) (*plain) += (pr2six[cur[0]] << 2 | pr2six[cur[1]] >> 4);
  if (nprbytes > 2) (*plain) += (pr2six[cur[1]] << 4 | pr2six[cur[2]] >> 2);
  if (nprbytes > 3) (*plain) += (pr2six[cur[2]] << 6 | pr2six[cur[3]]);

  return (end - encoded.bytes_begin()) + ((4 - nprbytes) & 3);
}

static const char basis_64[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

void Base64Encode(llvm::StringRef plain, std::string* encoded) {
  encoded->clear();
  if (plain.empty())
    return;
  std::size_t len = plain.size();
  encoded->reserve(((len + 2) / 3 * 4) + 1);

  std::size_t i;
  for (i = 0; (i + 2) < len; i += 3) {
    (*encoded) += basis_64[(plain[i] >> 2) & 0x3F];
    (*encoded) +=
        basis_64[((plain[i] & 0x3) << 4) | ((int)(plain[i + 1] & 0xF0) >> 4)];
    (*encoded) += basis_64[((plain[i + 1] & 0xF) << 2) |
                           ((int)(plain[i + 2] & 0xC0) >> 6)];
    (*encoded) += basis_64[plain[i + 2] & 0x3F];
  }
  if (i < len) {
    (*encoded) += basis_64[(plain[i] >> 2) & 0x3F];
    if (i == (len - 1)) {
      (*encoded) += basis_64[((plain[i] & 0x3) << 4)];
      (*encoded) += '=';
    } else {
      (*encoded) +=
          basis_64[((plain[i] & 0x3) << 4) | ((int)(plain[i + 1] & 0xF0) >> 4)];
      (*encoded) += basis_64[((plain[i + 1] & 0xF) << 2)];
    }
    (*encoded) += '=';
  }
}

}  // namespace wpi
