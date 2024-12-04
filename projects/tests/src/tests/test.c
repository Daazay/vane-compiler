#include "utest/utest.h"

#include "vanec/vanec.h"

UTEST(Test, sum) {
    ASSERT_EQ(5 + 16, sum(5, 16));
}