#include "Value.h"

#include "gtest/gtest.h"

namespace ntimpl {

class StringValueTest : public ::testing::Test {
 public:
  static NT_String& ToNT(StringValue& v) { return v; }
};

}  // namespace ntimpl
