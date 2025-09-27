/*
    sha1.hpp - header of

    ============
    SHA-1 in C++
    ============

    100% Public Domain.

    Original C Code
        -- Steve Reid <steve@edmweb.com>
    Small changes to fit into bglibs
        -- Bruce Guenter <bruce@untroubled.org>
    Translation to simpler C++ Code
        -- Volker Grabsch <vog@notjusthosting.com>
    Safety fixes
        -- Eugene Hopkinson <slowriot at voxelstorm dot com>
*/

#ifndef WPIUTIL_WPI_SHA1_H_
#define WPIUTIL_WPI_SHA1_H_

#include <stdint.h>

#include <string>
#include <string_view>

namespace wpi {
template <typename T>
class SmallVectorImpl;
class raw_istream;

class SHA1 {
 public:
  SHA1();
  void Update(std::string_view s);
  void Update(raw_istream& is);
  std::string Final();
  std::string_view Final(SmallVectorImpl<char>& buf);
  std::string_view RawFinal(SmallVectorImpl<char>& buf);
  static std::string FromFile(std::string_view filename);

 private:
  uint32_t digest[5];
  unsigned char buffer[64];
  size_t buf_size;
  uint64_t transforms;
};

}  // namespace wpi

#endif  // WPIUTIL_WPI_SHA1_H_
