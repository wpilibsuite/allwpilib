#include "gtest/gtest.h"
#include "gmock/gmock.h"
using ::testing::MakeMatcher;
using ::testing::Matcher;
using ::testing::MatcherInterface;
using ::testing::MatchResultListener;

template<class T> class IsArrayEqualToMatcher : public MatcherInterface<const T*> {
public:
	IsArrayEqualToMatcher(const T* _exp, int _expLen){
		exp = _exp;
		expLen = _expLen;
	}
	virtual bool MatchAndExplain(const T* arg, MatchResultListener* listener) const {
		*listener << "[";
		for(int i = 0; i<expLen; ++i){
			*listener << arg[i];
			if(i<expLen-1)
				*listener << ", ";
		}
		*listener << "]";
		
		for(int i = 0; i<expLen; ++i){
			if(exp[i]!=arg[i])
				return false;
		}
		return true;
	}

	virtual void DescribeTo(::std::ostream* os) const {
		*os << "is equal to array [";
		for(int i = 0; i<expLen; ++i){
			*os << exp[i];
			if(i<expLen-1)
				*os << ", ";
		}
		*os << "]";
	}
	
	virtual void DescribeNegationTo(::std::ostream* os) const {
		*os << "is not equal to array";
	}
private:
	const T* exp;
	int expLen;
};

template<class T> Matcher<const T*> IsArrayEqualTo(const T* exp, int expLen) {
	return MakeMatcher(new IsArrayEqualToMatcher<T>(exp, expLen));
}

