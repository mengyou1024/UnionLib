#include <UnionType>
#include <functional>
#include <gtest/gtest.h>
#include <iostream>
#include <memory>
#include <utility>
#include <variant>
#include <vector>

TEST(Union, common) {
    using namespace Union;
    EXPECT_NEAR(CalculatedGain(1.0, 2.0), 6, 0.1);
    EXPECT_NEAR(CalculateGainOutput(1.0, 6), 2.0, 0.01);
    EXPECT_NEAR(CalculateGainInput(2.0, 6), 1.0, 0.01);
    EXPECT_DOUBLE_EQ(KeepDecimals(1.999), 2.0);
    EXPECT_DOUBLE_EQ(KeepDecimals<3>(3.1415926), 3.142);
    EXPECT_DOUBLE_EQ(ValueMap(-100.0, {0.0, 100.0}, {0.0, 200.0}), -50.0);
    EXPECT_EQ(ValueMap(100.0, {0.0, 100.0}, {0.0, 200.0}), 50);
}

TEST(Union, readFile) {
    EXPECT_TRUE(Union::__390N_T8::LDAT::FromFile(L"test_file/1.LDAT").has_value());
    EXPECT_TRUE(Union::__TOFD_PE::Data::FromFile(L"test_file/2.tpe").has_value());
    EXPECT_FALSE(Union::__TOFD_PE::Data::FromFile(L"test_file/3.tpe").has_value());
}
