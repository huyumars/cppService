#include <gtest/gtest.h>
#include "executorTest.H"
#include "threadSafeTest.H"

int main(int argc, char **argv)
{
 testing::InitGoogleTest(&argc, argv);
 return RUN_ALL_TESTS();
}

