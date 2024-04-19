#include "das.hpp"
#include <Yo/File>
#include <Yo/Types>
#include <chrono>
#include <cstdio>
#include <numbers>

namespace Yo::File {
    template <>
    size_t __Read(std::ifstream &file, Union::__330::DASType &data, [[maybe_unused]] size_t file_size) {
        return data.__Read(file, file_size);
    }

} // namespace Yo::File

namespace Union::__330 {
    std::unique_ptr<Union::AScan::AScanIntf> DASType::FromFile(const std::wstring &fileName) {
        auto ret = std::make_unique<DASType>();
        auto res = Yo::File::ReadFile(fileName, *(ret.get()));
        if (res) {
            return ret;
        }
        return nullptr;
    }

    size_t DASType::__Read(std::ifstream &file, size_t file_size) {
        if (file_size <= 454 + 480 || file_size >= 454 + 580) {
            return 0;
        }
        size_t ret = 0;
        ret += Yo::File::__Read(file, Yo::File::MakeStructSub(&(systemStatus), &(channelTemp), sizeof(uint32_t)), file_size);
        data.resize(480);
        ret += Yo::File::__Read(file, data, file_size);
        name.resize(file_size - 454 - 480);
        ret += Yo::File::__Read(file, name, file_size);
        return ret;
    }

    int DASType::getDataSize(void) const {
        return 1;
    }

    std::vector<std::wstring> DASType::getFileNameList(void) const {
        return {};
    }

    void DASType::setFileNameIndex(int index) {
        (void)index;
    }

    Base::Performance DASType::getPerformance(int idx) const {
        (void)idx;

        Base::Performance ret;
        ret.horizontalLinearity = *(reinterpret_cast<const uint32_t *>(&name[72])) / 10.0;
        ret.verticalLinearity   = *(reinterpret_cast<const uint32_t *>(&name[72]) + 1) / 10.0;
        ret.resolution          = *(reinterpret_cast<const uint32_t *>(&name[72]) + 2) / 10.0;
        ret.dynamicRange        = *(reinterpret_cast<const uint32_t *>(&name[72]) + 3) / 10.0;
        ret.sensitivity         = *(reinterpret_cast<const uint32_t *>(&name[72]) + 4) / 10.0;
        return ret;
    }

    std::string DASType::getDate(int idx) const {
        (void)idx;
        char buf[11];
        snprintf(buf, sizeof(buf), "%02x%02x-%02x-%02x", name[71], name[70], name[68], name[67]);
        return std::string(buf, 10);
    }

    std::wstring DASType::getProbe(int idx) const {
        (void)idx;
#if __has_include("QString")
        return Union::Base::Probe::Index2Name_QtExtra((channelStatus.sys >> 12) & 0x07).toStdWString();
#else
        return Yo::Types::WStringFromString(std::string(Union::Base::Probe::Index2Name((channelStatus.sys >> 12) & 0x07)));
#endif
    }

    double DASType::getProbeFrequence(int idx) const {
        (void)idx;
        return channelParam.freqence / 100.0;
    }

    std::string DASType::getProbeChipShape(int idx) const {
        (void)idx;
        return Union::Base::Probe::CreateProbeChipShape(((channelStatus.sys >> 12) & 0x07), channelParam.crystal_l / 1000, channelParam.crystal_w / 1000);
    }

    double DASType::getAngle(int idx) const {
        (void)idx;
        return channelParam.angle / 10.0;
    }

    double DASType::getSoundVelocity(int idx) const {
        (void)idx;
        return channelParam.speed;
    }

    double DASType::getFrontDistance(int idx) const {
        (void)idx;
        return channelParam.forward / 10.0;
    }

    double DASType::getZeroPointBias(int idx) const {
        (void)idx;
        return channelParam.offset;
    }

    double DASType::getSamplingDelay(int idx) const {
        (void)idx;
        return channelParam.delay;
    }

    int DASType::getChannel(int idx) const {
        (void)idx;
        return -1;
    }

    std::string DASType::getInstrumentName(void) const {
        return "330 Single";
    }

    std::array<Base::Gate, 2> DASType::getGate(int idx) const {
        (void)idx;
        std::array<Base::Gate, 2> ret;
        for (auto i = 0; i < 2; i++) {
            ret[i].pos    = gateParam[i].pos / 480.0;
            ret[i].width  = gateParam[i].width / 480.0;
            ret[i].height = gateParam[i].height / 200.0;
            if (ret[i].height > 0.0) {
                ret[i].enable = true;
            }
        }
        return ret;
    }

    const std::vector<uint8_t> &DASType::getScanData(int idx) const {
        return data;
    }

    double DASType::getAxisBias(int idx) const {
        auto       ret = 0.0;
        const auto opt = getOption();
        if (opt == 0) {
            ret = static_cast<double>(channelParam.delay) * static_cast<double>(channelParam.speed) / (20000.0) * std::cos(getAngle(idx) * std::numbers::pi / 180.0);
        } else if (opt == 1) {
            ret = static_cast<double>(channelParam.delay) * static_cast<double>(channelParam.speed) / (20000.0) * std::sin(getAngle(idx) * std::numbers::pi / 180.0);
        } else {
            ret = static_cast<double>(channelParam.delay) * static_cast<double>(channelParam.speed) / (20000.0);
        }
        if (getUnit() > 1.0) {
            return Union::KeepDecimals(ret / (10.0 * getUnit()));
        }
        return Union::KeepDecimals(ret / (10.0 * getUnit()));
    }

    double DASType::getAxisLen(int idx) const {
        (void)idx;
        const auto opt = getOption();
        auto       ret = 0.0;
        if (opt == 0) {
            ret = static_cast<double>(channelParam.range) * std::cos(getAngle(idx) * std::numbers::pi / 180.0);
        } else if (opt == 1) {
            ret = static_cast<double>(channelParam.range) * std::sin(getAngle(idx) * std::numbers::pi / 180.0);
        } else {
            ret = static_cast<double>(channelParam.range);
        }
        if (getUnit() > 1.0) {
            return Union::KeepDecimals<1>(ret / (10.0 * getUnit()));
        }
        return Union::KeepDecimals<1>(ret / 10.0);
    }

    double DASType::getBaseGain(int idx) const {
        (void)idx;
        return channelParam.baseGain / 10.0;
    }

    double DASType::getScanGain(int idx) const {
        (void)idx;
        return channelParam.compGain / 10.0;
    }

    double DASType::getSurfaceCompentationGain(int idx) const {
        (void)idx;
        return channelParam.surGain / 10.0;
    }

    int DASType::getSupression(int idx) const {
        (void)idx;
        return channelParam.reject;
    }

    Union::AScan::DistanceMode DASType::getDistanceMode(int idx) const {
        // TODO: 返回正确的DistanceMode
        return Union::AScan::DistanceMode_Y;
    }

    std::optional<Base::AVG> DASType::getAVG(int idx) const {
        if (channelStatus.status & 0b001) {
            Base::AVG ret;
            // TODO: 填入AVG数据
            return std::nullopt;
        }
        return std::nullopt;
    }

    std::optional<Base::DAC> DASType::getDAC(int idx) const {
        if (channelStatus.status & 0b01) {
            Base::DAC ret;
            // TODO: 填入AVG数据
            return std::nullopt;
        }
        return std::nullopt;
    }

    Union::AScan::DAC_Standard DASType::getDACStandard(int idx) const {
        return Union::AScan::DAC_Standard();
    }

    std::function<double(double)> DASType::getAVGLineExpr(int idx) const {
        return std::function<double(double)>();
    }

    std::function<double(double)> DASType::getDACLineExpr(int idx) const {
        return std::function<double(double)>();
    }

    int DASType::getOption(void) const noexcept {
        auto ret = channelStatus.option;
        ret >>= 24;
        ret &= 0x03;
        return ret;
    }

    double DASType::getUnit(void) const noexcept {
        if (systemStatus.unit) {
            return 100.0;
        }
        return 1.0;
    }
} // namespace Union::__330
