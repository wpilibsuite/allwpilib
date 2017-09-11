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

#ifndef SHA1_HPP
#define SHA1_HPP


#include <cstdint>
#include <string>
#include "llvm/StringRef.h"

namespace llvm {
template <typename T> class SmallVectorImpl;
}

namespace wpi {

class raw_istream;

class SHA1
{
public:
    SHA1();
    void Update(llvm::StringRef s);
    void Update(raw_istream &is);
    std::string Final();
    llvm::StringRef Final(llvm::SmallVectorImpl<char>& buf);
    static std::string FromFile(llvm::StringRef filename);

private:
    uint32_t digest[5];
    unsigned char buffer[64];
    size_t buf_size;
    uint64_t transforms;
};

}  // namespace wpi

#endif /* SHA1_HPP */
