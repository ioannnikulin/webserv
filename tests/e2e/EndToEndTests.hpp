#ifndef ENDTOENDTESTS_HPP
#define ENDTOENDTESTS_HPP

#include <cxxtest/TestSuite.h>

class TestsNothing : public CxxTest::TestSuite {
public:
    void testNothing() {
        TS_ASSERT_EQUALS(1, 1);
    }
};

#endif
