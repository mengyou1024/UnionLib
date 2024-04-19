#include "ASCAN/330/lzw.hpp"
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
    using namespace Union::__330;
    EXPECT_TRUE(Union::__390N_T8::LDAT::FromFile(L"test_file/1.LDAT"));
    EXPECT_EQ(sizeof(Union::__330::SystemStatus), 108);
    EXPECT_EQ(sizeof(Union::__330::ChannelStatus), 36);
    EXPECT_EQ(sizeof(Union::__330::ChannelParam), 84);
    EXPECT_EQ(sizeof(Union::__330::GateParam), 8);
    EXPECT_EQ(sizeof(Union::__330::DACParam), 46);
    EXPECT_EQ(sizeof(Union::__330::WeldParam), 68);
    EXPECT_EQ(sizeof(Union::__330::GatePeakSingleParam), 24);
    EXPECT_TRUE(Union::__330::DASType::FromFile(L"test_file/4.das"));
    EXPECT_TRUE(Union::__330::DATType::FromFile(L"test_file/5.dat"));
    EXPECT_TRUE(Union::TOFD_PE::TPE::TpeType::FromFile(L"test_file/2.tpe"));
    EXPECT_FALSE(Union::TOFD_PE::TPE::TpeType::FromFile(L"test_file/3.tpe"));
    EXPECT_TRUE(Union::__330::DATType::FromFile(L"test_file/23-11-03.DAT"));
}

TEST(__330, lzw_decompressed) {
    using namespace Yo::File;
    for (uint32_t i = 0; i <= 10; i++) {
        std::wstring fileName = LR"(test_file\lzwTest\raw\)";
        fileName += std::to_wstring(i) + L".bin";
        std::vector<uint8_t> rawData = {};
        EXPECT_TRUE(Yo::File::ReadFile(fileName, MakeFunctional([&](size_t file_size) -> std::vector<uint8_t> & {
                                           rawData.resize(file_size);
                                           return rawData;
                                       })));
        std::vector<uint8_t> decodeData = {};
        fileName                        = LR"(test_file\lzwTest\decode\)";
        fileName += std::to_wstring(i) + L".bin";
        EXPECT_TRUE(Yo::File::ReadFile(fileName, MakeFunctional([&](size_t file_size) -> std::vector<uint8_t> & {
                                           decodeData.resize(file_size);
                                           return decodeData;
                                       })));
        auto ret = Union::__330::lzw_decompress(rawData.data(), rawData.size());
        EXPECT_EQ(ret.value(), decodeData);
        EXPECT_NE(ret.value(), rawData);
    }
}
