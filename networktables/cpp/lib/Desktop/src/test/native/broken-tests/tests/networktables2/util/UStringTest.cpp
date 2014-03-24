#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <string>

using ::testing::MakeMatcher;
using ::testing::Matcher;
using ::testing::MatcherInterface;
using ::testing::MatchResultListener;

class IsArrayEqualToMatcher : public MatcherInterface<const char*> {
public:
	IsArrayEqualToMatcher(const char* _exp, int _expLen){
		exp = _exp;
		expLen = _expLen;
	}
	virtual bool MatchAndExplain(const char* arg, MatchResultListener* listener) const {
		for(int i = 0; i<expLen; ++i){
			if(exp[i]!=arg[i])
				return false;
		}
		return true;
	}

	virtual void DescribeTo(::std::ostream* os) const {
		*os << "is equal to array";
	}
	
	virtual void DescribeNegationTo(::std::ostream* os) const {
		*os << "is not equal to array";
	}
private:
	const char* exp;
	int expLen;
};

inline Matcher<const char*> IsArrayEqualTo(const char* exp, int expLen) {
	return MakeMatcher(new IsArrayEqualToMatcher(exp, expLen));
}

TEST(std::string, EmptyAsciiString) {
	std::string s("");
	EXPECT_STREQ("", s.c_str());
	char expA[1] = {0};
	EXPECT_THAT(s.u_str(), IsArrayEqualTo(expA, 1));
	
	EXPECT_EQ(0, s.length());
}


TEST(std::string, NonEmptyAsciiString) {
	std::string s("Hello");
	EXPECT_STREQ("Hello", s.c_str());
	char expA[6] = {'H', 'e', 'l', 'l', 'o', 0};
	EXPECT_THAT(s.u_str(), IsArrayEqualTo(expA, 6));
	
	EXPECT_EQ(5, s.length());
}


TEST(std::string, EmptyUnicodeString) {
	char inital[1] = {0};
	std::string s(inital);
	EXPECT_STREQ("", s.c_str());
	char expA[1] = {0};
	EXPECT_THAT(s.u_str(), IsArrayEqualTo(expA, 1));
	
	EXPECT_EQ(0, s.length());
}

TEST(std::string, NonEmptyUnicodeString) {
	char inital[5] = {'T', 'e', 's', 't', 0};
	std::string s(inital);
	EXPECT_STREQ("Test", s.c_str());
	char expA[5] = {'T', 'e', 's', 't', 0};
	EXPECT_THAT(s.u_str(), IsArrayEqualTo(expA, 5));
	
	EXPECT_EQ(4, s.length());
}

TEST(std::string, NonAsciiString) {
	char inital[6] = {'T', 'e', 's', 500, 't', 0};
	std::string s(inital);
	EXPECT_THROW(s.c_str(), UnicodeStringException);
	char expA[6] = {'T', 'e', 's', 500, 't', 0};
	EXPECT_THAT(s.u_str(), IsArrayEqualTo(expA, 6));
	
	EXPECT_EQ(5, s.length());
}
