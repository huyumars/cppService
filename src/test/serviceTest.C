#include <gtest/gtest.h>
#include "logTest.H"
#include "executorTest.H"
#include "threadSafeTest.H"
#include "tcpCSTest.H"

int main(int argc, char **argv)
{
 testing::InitGoogleTest(&argc, argv);
 return RUN_ALL_TESTS();
}

