#include "ASCAN/330/lzw.hpp"
#include <UnionType>
#include <functional>
#include <gtest/gtest.h>
#include <iostream>
#include <memory>
#include <utility>
#include <variant>
#include <vector>
#include <Yo/File>

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
    EXPECT_TRUE(Union::__330::Serial_330::FromFile(L"test_file/serial.cod"));
    EXPECT_TRUE(Union::__390::DAAType::FromFile(LR"(test_file\2024-05-13.daa)"));
    EXPECT_TRUE(Union::TOFD_PE::TOF::TofType::FromFile(L"test_file/1.tof"));
    EXPECT_TRUE(Union::TOFD_PE::TOF::TofType::FromFile(L"test_file/3322.tof"));
    EXPECT_TRUE(Union::__330::DATType::FromFile(LR"(test_file/PXUT-390.DAT)"));
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

TEST(HDBridge, seralize) {
    using namespace Union::Bridge::MultiChannelHardwareBridge;
    using namespace Union::Bridge;
    // _12CH_USB a;
    // a.setChannelFlag(0xffffffff);
    // a.serializeConfigData(L"123.bin");
    // _12CH_USB b;
    // b.deserializeConfigData(L"123.bin");
    // EXPECT_EQ(a, b);
}

static void test_instrumentBaseInfo() {
    using namespace Union::__390N_T8::MDATType;

    InstrumentBaseInfo baseInfo(1, 2, "13213545");
    QByteArray         arr;
    QDataStream        stream(&arr, QIODevice::WriteOnly);
    baseInfo.serialize_payload(stream);
    InstrumentBaseInfo baseInfo2;
    QDataStream        stream2(&arr, QIODevice::ReadOnly);
    baseInfo2.unserialize_payload(stream2);
    EXPECT_TRUE(baseInfo == baseInfo2);
}

static void test_channelParam() {
    using namespace Union::__390N_T8::MDATType;
    ChannelParam channelParam;
    channelParam.scanGain              = 1.0;
    channelParam.baseGain              = 2.0;
    channelParam.soundVelocity         = 3.0;
    channelParam.channel               = 4;
    channelParam.suppression           = 5.0;
    channelParam.angle                 = 6.0;
    channelParam.probeFrontDistance    = 7.0;
    channelParam.axisLen               = 8.0;
    channelParam.probeChipShapeWorD    = 9.0;
    channelParam.probeChipShapeLorZero = 10.0;
    channelParam.axisBias              = 11.0;
    channelParam.distanceMode          = 12;
    channelParam.probe                 = 13;
    channelParam.gateAHeight           = 14.0;
    channelParam.gateBHeight           = 15.0;
    channelParam.gateAWidth            = 16.0;
    channelParam.gateBWidth            = 17.0;
    channelParam.samplingDelay         = 18.0;
    channelParam.gateAPos              = 19.0;
    channelParam.gateBPos              = 20.0;
    channelParam.compensatingGain      = 21.0;
    channelParam.probeFrequency        = 22.0;
    channelParam.gateAEnable           = 23;
    channelParam.gateBEnable           = 24;

    QByteArray  arr;
    QDataStream stream(&arr, QIODevice::WriteOnly);
    channelParam.serialize_payload(stream);
    ChannelParam channelParam2;
    QDataStream  stream2(&arr, QIODevice::ReadOnly);
    channelParam2.unserialize_payload(stream2);
    EXPECT_TRUE(channelParam == channelParam2);
}

static void test_dacParam() {
    using namespace Union::__390N_T8::MDATType;
    DACParam dacParam;
    dacParam.samplingXAxisLen  = 1.0;
    dacParam.samplingXAxisBias = 2.0;
    dacParam.criteriaBiasSL    = 3.0;
    dacParam.criteriaBiasEL    = 4.0;
    dacParam.criteriaBiasRL    = 5.0;
    dacParam.criteria          = 6;
    dacParam.compensatingGain  = 7.0;
    dacParam.baseGain          = 8.0;
    dacParam.onlyShowBaseLine  = 9;
    dacParam.equivalent        = 10.0;
    dacParam.isReady           = 11;
    dacParam.value             = {1.0, 2.0, 3.0, 4.0, 5.0};
    dacParam.index             = {1.0, 2.0, 3.0, 4.0, 5.0};
    QByteArray  arr;
    QDataStream stream(&arr, QIODevice::WriteOnly);
    dacParam.serialize_payload(stream);
    DACParam    dacParam2;
    QDataStream stream2(&arr, QIODevice::ReadOnly);
    dacParam2.unserialize_payload(stream2);
    EXPECT_TRUE(dacParam == dacParam2);
}

static void test_avgParam() {
    using namespace Union::__390N_T8::MDATType;
    AVGParam avgParam;
    avgParam.scanGain          = 1.0;
    avgParam.baseGain          = 2.0;
    avgParam.equivalent        = 3.0;
    avgParam.onlyShowBaseLine  = 4;
    avgParam.samplingXAxisLen  = 5.0;
    avgParam.samplingXAxisBias = 6.0;
    avgParam.compensatingGain  = 7.0;
    avgParam.isReady           = 8;
    avgParam.reflectorDiameter = 9.0;
    avgParam.reflectorMaxDepth = 10.0;
    avgParam.diameter          = 11.0;
    avgParam.value             = {1.0, 2.0, 3.0, 4.0, 5.0};
    avgParam.index             = {1.0, 2.0, 3.0, 4.0, 5.0};
    QByteArray  arr;
    QDataStream stream(&arr, QIODevice::WriteOnly);
    avgParam.serialize_payload(stream);
    AVGParam    avgParam2;
    QDataStream stream2(&arr, QIODevice::ReadOnly);
    avgParam2.unserialize_payload(stream2);
    EXPECT_TRUE(avgParam == avgParam2);
}

static void test_performance() {
    using namespace Union::__390N_T8::MDATType;
    Performance performance;
    performance.horizontalLinearity = 1.0;
    performance.verticalLinearity   = 2.0;
    performance.resolution          = 3.0;
    performance.dynamicRange        = 4.0;
    performance.surplusSensitivity  = 5.0;
    QByteArray  arr;
    QDataStream stream(&arr, QIODevice::WriteOnly);
    performance.serialize_payload(stream);
    Performance performance2;
    QDataStream stream2(&arr, QIODevice::ReadOnly);
    performance2.unserialize_payload(stream2);
    EXPECT_TRUE(performance == performance2);
}

static void test_cameraData() {
    using namespace Union::__390N_T8::MDATType;
    CameraData cameraData;
    cameraData.m_width     = 1;
    cameraData.m_height    = 2;
    cameraData.m_imgFormat = 3;
    cameraData.m_data      = {1, 2, 3, 4, 5};
    QByteArray  arr;
    QDataStream stream(&arr, QIODevice::WriteOnly);
    cameraData.serialize_payload(stream);
    CameraData  cameraData2;
    QDataStream stream2(&arr, QIODevice::ReadOnly);
    cameraData2.unserialize_payload(stream2);
    EXPECT_TRUE(cameraData == cameraData2);
}

static void test_ascanData() {
    using namespace Union::__390N_T8::MDATType;
    AScanData ascanData;
    ascanData.m_data = {1, 2, 3, 4, 5};
    QByteArray  arr;
    QDataStream stream(&arr, QIODevice::WriteOnly);
    ascanData.serialize_payload(stream);
    AScanData   ascanData2;
    QDataStream stream2(&arr, QIODevice::ReadOnly);
    ascanData2.unserialize_payload(stream2, 5ul);
    EXPECT_TRUE(ascanData == ascanData2);
}

TEST(__390N_T8, seralize_and_unseralize) {
    test_instrumentBaseInfo();
    test_channelParam();
    test_dacParam();
    test_avgParam();
    test_performance();
    test_cameraData();
    test_ascanData();
}
