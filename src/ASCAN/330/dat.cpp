#include "dat.hpp"
#include "lzw.hpp"
#include <Yo/File>
#include <Yo/Types>
#include <chrono>
#include <ctime>
#include <numeric>
#include <regex>
#include <stdexcept>

static constexpr uint8_t BCD2INT(uint8_t bcd) {
    return (bcd >> 4) * 10 + (bcd & 0x0F);
}

namespace Yo::File {
    template <>
    size_t __Read(std::ifstream &file, Union::__330::DATType &data, [[maybe_unused]] size_t file_size) {
        return data.__Read(file, file_size);
    }

} // namespace Yo::File

namespace Union::__330 {
    void DATType::setDate(const std::string &date) {
        m_date = date;
    }

    std::unique_ptr<Union::AScan::AScanIntf> DATType::FromFile(const std::wstring &filename) {
        auto ret = std::make_unique<DATType>();
        if (!Yo::File::ReadFile(filename, *(ret.get()))) {
            return nullptr;
        }
        std::wregex  reg(LR"((\d+-\d+-\d+)\.dat)", std::regex_constants::icase);
        std::wsmatch match;
        if (std::regex_search(filename, match, reg)) {
            std::wstring dateStr = match[1];
            dynamic_cast<DATType *>(ret.get())->setDate(Yo::Types::StringFromWString(dateStr));
        }
        return ret;
    }

    size_t DATType::__Read(std::ifstream &file, size_t file_size) {
        if (file_size == 0) {
            return 0;
        }
        size_t  ret         = 0;
        int32_t bufferCount = 0;
        ret += Yo::File::__Read(file, bufferCount, file_size);
        ret += Yo::File::__Read(file, Yo::File::SkipSize(4), file_size); // TODO: 这个标志位用来判断330N、330C？
        for (int32_t i = 0; i < bufferCount; i++) {
            int32_t encoderLen = 0;
            int32_t infoLen    = -1;
            ret += Yo::File::__Read(file, encoderLen, file_size);
            ret += Yo::File::__Read(file, infoLen, file_size);
            std::vector<uint8_t> rawData;
            rawData.resize(encoderLen);
            ret += Yo::File::__Read(file, rawData, file_size);
            auto decoderBuf = Union::__330::lzw_decompress(rawData.data(), encoderLen);
            if (infoLen < 0 || (infoLen != (decoderBuf->size() % ASCAN_FRAME_SIZE))) {
                throw std::runtime_error("info lenght error!");
            }

            if (infoLen > 0) {
                auto temp_head = __DATHead();
                auto ptr       = &(*decoderBuf)[decoderBuf->size() - infoLen];
                memcpy(&temp_head, ptr, Union::__330::DAT_HEAD_SIZE);
                temp_head.info_buf.resize((infoLen - Union::__330::DAT_HEAD_SIZE) / 2);
                ptr = &(*decoderBuf)[decoderBuf->size() - infoLen + Union::__330::DAT_HEAD_SIZE];
                memcpy(temp_head.info_buf.data(), ptr, temp_head.info_buf.size() * 2);
                temp_head.header_timestamp.hour      = BCD2INT(temp_head.header_timestamp.hour);
                temp_head.header_timestamp.minute    = BCD2INT(temp_head.header_timestamp.minute);
                temp_head.header_timestamp.second    = BCD2INT(temp_head.header_timestamp.second);
                temp_head.header_timestamp.notes_len = BCD2INT(temp_head.header_timestamp.notes_len);
                m_data.emplace_back(std::make_pair<__DATHead, std::vector<__DATType>>(std::move(temp_head), {}));
            }

            for (auto f = 0; std::cmp_less(f, decoderBuf->size() / ASCAN_FRAME_SIZE); f++) {
                Union::__330::__DATType temp;
                temp.ascan_data.resize(ASCAN_DATA_SIZE);
                memcpy(temp.ascan_data.data(), &(*decoderBuf)[f * ASCAN_FRAME_SIZE], ASCAN_DATA_SIZE);
                memcpy(&(temp.header_timestamp), &(*decoderBuf)[f * ASCAN_FRAME_SIZE + ASCAN_DATA_SIZE], ASCAN_FRAME_SIZE - ASCAN_DATA_SIZE);
                if (std::accumulate(temp.ascan_data.begin(), temp.ascan_data.end(), 0, [](auto a, auto b) { return a + b; }) != 0) {
                    auto &list = m_data.back().second;
                    list.emplace_back(std::move(temp));
                }
            }
        }
        return ret;
    }

    int DATType::getDataSize(void) const {
        return static_cast<int>(m_data[m_fileName_index].second.size());
    }

    std::vector<std::wstring> DATType::getFileNameList(void) const {
        std::vector<std::wstring> ret;
        for (const auto &[key, value] : m_data) {
            std::wstring s;
            for (int i = 0; i < key.info_buf[0]; i += 1) {
                s += (key.info_buf[i + 1]);
            }
            ret.emplace_back(std::move(s));
        }
        return ret;
    }

    void DATType::setFileNameIndex(int index) {
        if (index < 0 || std::cmp_greater_equal(index, m_data.size())) {
            return;
        }
        m_fileName_index = index;
    }

    Base::Performance DATType::getPerformance(int idx) const {
        return Base::Performance();
    }

    std::string DATType::getDate(int idx) const {
        (void)idx;
        return m_date;
    }

    std::wstring DATType::getProbe(int idx) const {
        (void)idx;
        return Union::Base::Probe::Index2Name_QtExtra((getHead().channel_status.sys >> 12) & 0x07).toStdWString();
    }

    double DATType::getProbeFrequence(int idx) const {
        (void)idx;
        return getHead().channel_param.Frequence / 100.0;
    }

    std::string DATType::getProbeChipShape(int idx) const {
        (void)idx;
        return Union::Base::Probe::CreateProbeChipShape(((getHead().channel_status.sys >> 12) & 0x07), getHead().channel_param.Crystal_l / 1000, getHead().channel_param.Crystal_w / 1000);
    }

    double DATType::getAngle(int idx) const {
        (void)idx;
        return getHead().channel_param.Angle / 10.0;
    }

    double DATType::getSoundVelocity(int idx) const {
        (void)idx;
        return getHead().channel_param.Speed;
    }

    double DATType::getFrontDistance(int idx) const {
        (void)idx;
        return getHead().channel_param.Forward / 10.0;
    }

    double DATType::getZeroPointBias(int idx) const {
        (void)idx;
        return getHead().channel_param.Offset;
    }

    double DATType::getSamplingDelay(int idx) const {
        (void)idx;
        return getHead().channel_param.Delay;
    }

    int DATType::getChannel(int idx) const {
        (void)idx;
        return -1;
    }

    std::string DATType::getInstrumentName(void) const {
        return "330 Continuous";
    }

    std::array<Base::Gate, 2> DATType::getGate(int idx) const {
        (void)idx;
        std::array<Base::Gate, 2> ret;
        for (auto i = 0; i < 2; i++) {
            ret[i].pos    = getHead().gate_param[i].Position / 500.0;
            ret[i].width  = getHead().gate_param[i].Width / 500.0;
            ret[i].height = getHead().gate_param[i].Height / 200.0;
            if (ret[i].height > 0.0) {
                ret[i].enable = true;
            }
        }
        return ret;
    }

    const std::vector<uint8_t> &DATType::getScanData(int idx) const {
        return m_data[m_fileName_index].second.at(idx).ascan_data;
    }

    double DATType::getAxisBias(int idx) const {
        (void)idx;
        auto       ret = 0.0;
        const auto opt = getOption(idx);
        if (opt == 0) {
            ret = static_cast<double>(getHead().channel_param.Delay) * static_cast<double>(getHead().channel_param.Speed) / (20000.0) * std::cos(getAngle(idx) * std::numbers::pi / 180.0);
        } else if (opt == 1) {
            ret = static_cast<double>(getHead().channel_param.Delay) * static_cast<double>(getHead().channel_param.Speed) / (20000.0) * std::sin(getAngle(idx) * std::numbers::pi / 180.0);
        } else {
            ret = static_cast<double>(getHead().channel_param.Delay) * static_cast<double>(getHead().channel_param.Speed) / (20000.0);
        }
        if (getUnit(idx) > 1.0) {
            return Union::KeepDecimals(ret / (10.0 * getUnit(idx)));
        }
        return Union::KeepDecimals(ret / (10.0 * getUnit(idx)));
    }

    double DATType::getAxisLen(int idx) const {
        (void)idx;
        const auto opt = getOption(idx);
        auto       ret = 0.0;
        if (opt == 0) {
            ret = static_cast<double>(getHead().channel_param.Range) * std::cos(getAngle(idx) * std::numbers::pi / 180.0);
        } else if (opt == 1) {
            ret = static_cast<double>(getHead().channel_param.Range) * std::sin(getAngle(idx) * std::numbers::pi / 180.0);
        } else {
            ret = static_cast<double>(getHead().channel_param.Range);
        }
        if (getUnit(idx) > 1.0) {
            return Union::KeepDecimals<1>(ret / (10.0 * getUnit(idx)));
        }
        return Union::KeepDecimals<1>(ret / 10.0);
    }

    double DATType::getBaseGain(int idx) const {
        (void)idx;
        return getHead().channel_param.BaseGain / 10.0;
    }

    double DATType::getScanGain(int idx) const {
        (void)idx;
        return getHead().channel_param.CompGain / 10.0;
    }

    double DATType::getSurfaceCompentationGain(int idx) const {
        (void)idx;
        return getHead().channel_param.SurfGain / 10.0;
    }

    int DATType::getSupression(int idx) const {
        (void)idx;
        return getHead().channel_param.Reject;
    }

    Union::AScan::DistanceMode DATType::getDistanceMode(int idx) const {
        // TODO: 返回正确的DistanceMode
        return Union::AScan::DistanceMode_Y;
    }

    std::optional<Base::AVG> DATType::getAVG(int idx) const {
        return std::optional<Base::AVG>();
    }

    std::optional<Base::DAC> DATType::getDAC(int idx) const {
        return std::optional<Base::DAC>();
    }

    Union::AScan::DAC_Standard DATType::getDACStandard(int idx) const {
        return Union::AScan::DAC_Standard();
    }

    std::function<double(double)> DATType::getAVGLineExpr(int idx) const {
        return std::function<double(double)>();
    }

    std::function<double(double)> DATType::getDACLineExpr(int idx) const {
        return std::function<double(double)>();
    }

    int DATType::getOption(int idx) const noexcept {
        (void)idx;
        auto ret = getHead().channel_status.option;
        ret >>= 24;
        ret &= 0x03;
        return static_cast<int>(ret);
    }

    double DATType::getUnit(int idx) const noexcept {
        (void)idx;
        if (getHead().system_status.unit) {
            return 100.0;
        }
        return 1.0;
    }
    const __DATHead &DATType::getHead() const {
        return m_data.at(m_fileName_index).first;
    }
} // namespace Union::__330
