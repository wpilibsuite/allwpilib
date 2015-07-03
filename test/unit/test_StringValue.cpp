#include "Value.h"

#include "gtest/gtest.h"

namespace ntimpl {

class StringValueTest : public ::testing::Test {
 public:
  NT_String& ToNT(StringValue& v) { return v; }
};

TEST_F(StringValueTest, ConstructEmpty) {
  StringValue v;
  ASSERT_EQ(ToNT(v).str, nullptr);
  ASSERT_EQ(ToNT(v).len, 0u);
}

TEST_F(StringValueTest, ConstructStringRef) {
  StringValue v("hello");
  ASSERT_EQ(llvm::StringRef(v), "hello");
  ASSERT_EQ(ToNT(v).str, llvm::StringRef("hello"));
  ASSERT_EQ(ToNT(v).len, 5u);
}

TEST_F(StringValueTest, ConstructMove) {
  StringValue v("hello");
  char* orig = ToNT(v).str;
  ASSERT_NE(orig, nullptr);
  ASSERT_EQ(ToNT(v).len, 5u);
  StringValue v2(std::move(v));
  ASSERT_EQ(ToNT(v).str, nullptr);
  ASSERT_EQ(ToNT(v).len, 0u);
  ASSERT_EQ(ToNT(v2).str, orig);
  ASSERT_EQ(ToNT(v2).len, 5u);
}

TEST_F(StringValueTest, AssignMove) {
  StringValue v("hello");
  char* orig = ToNT(v).str;
  ASSERT_NE(orig, nullptr);
  ASSERT_EQ(ToNT(v).len, 5u);
  StringValue v2("goodbye");
  ASSERT_NE(ToNT(v2).str, nullptr);
  ASSERT_EQ(ToNT(v2).len, 7u);
  v2 = std::move(v);
  ASSERT_EQ(ToNT(v).str, nullptr);
  ASSERT_EQ(ToNT(v).len, 0u);
  ASSERT_EQ(ToNT(v2).str, orig);
  ASSERT_EQ(ToNT(v2).len, 5u);
}

}  // namespace ntimpl
