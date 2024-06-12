#include "dat.hpp"
#include "./_330_draw_dac.hpp"
#include "lzw.hpp"
#include <QLoggingCategory>
#include <Yo/File>
#include <Yo/Types>
#include <ctime>
#include <numeric>
#include <regex>

static Q_LOGGING_CATEGORY(TAG, "330.DAT");

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

    std::unique_ptr<Union::AScan::AScanIntf> _FromFile(const std::wstring &filename) {
        auto ret        = std::make_unique<DATType>();
        ret->m_fileName = QString::fromStdWString(filename);
        if (!Yo::File::ReadFile(filename, *(ret.get()))) {
            return nullptr;
        }
        std::wregex  reg(LR"((\d+-\d+-\d+)\.\w+)", std::regex_constants::icase);
        std::wsmatch match;
        if (std::regex_search(filename, match, reg)) {
            std::wstring dateStr = match[1];
            dynamic_cast<DATType *>(ret.get())->setDate(Yo::Types::StringFromWString(L"20" + dateStr));
        }
        return ret;
    }

    std::unique_ptr<Union::AScan::AScanIntf> DATType::FromFile(const std::wstring &fileName) {
        return _FromFile(fileName);
    }

    size_t DATType::__Read(std::ifstream &file, size_t file_size) {
        try {
            if (file_size == 0) {
                return 0;
            }
            size_t   ret         = 0;
            int32_t  bufferCount = 0;
            uint32_t _330CFlag   = 0;
            ret += Yo::File::__Read(file, bufferCount, file_size);
            ret += Yo::File::__Read(file, _330CFlag, file_size); // TODO: 这个标志位用来判断330N、330C？
            if (_330CFlag >= 10) {
                constexpr auto msg = "_330CFlag >= 10";
                qWarning(TAG) << msg << "fileName: " << m_fileName;
            }
            for (int32_t i = 0; i < bufferCount; i++) {
                int32_t encoderLen = 0;
                int32_t infoLen    = -1;
                ret += Yo::File::__Read(file, encoderLen, file_size);
                ret += Yo::File::__Read(file, infoLen, file_size);
                if (std::cmp_greater_equal(encoderLen, file_size) || std::cmp_greater_equal(infoLen, file_size)) {
                    qWarning(TAG) << "encoderLen >= file_size, fileName: " << m_fileName;
                    return 0;
                }
                std::vector<uint8_t> rawData;
                rawData.resize(encoderLen);
                ret += Yo::File::__Read(file, rawData, file_size);
                auto decoderBuf = Union::__330::lzw_decompress(rawData.data(), encoderLen);
                if (infoLen < 0 || (infoLen != (decoderBuf->size() % ASCAN_FRAME_SIZE))) {
                    qFatal("info lenght error!");
                    constexpr auto msg = "info lenght error!";
#if defined(QT_DEBUG)
                    qFatal(msg);
#else
                    qCritical(TAG) << msg;
                    return 0;
#endif
                }

                auto head_ptr = std::make_shared<__DATHead>();
                if (infoLen > 0) {
                    auto temp_head = std::make_shared<__DATHead>();
                    auto ptr       = &(*decoderBuf)[decoderBuf->size() - infoLen];
                    memcpy(temp_head.get(), ptr, Union::__330::DAT_HEAD_SIZE);
                    temp_head->info_buf.resize((infoLen - Union::__330::DAT_HEAD_SIZE) / 2);
                    ptr = &(*decoderBuf)[decoderBuf->size() - infoLen + Union::__330::DAT_HEAD_SIZE];
                    memcpy(temp_head->info_buf.data(), ptr, temp_head->info_buf.size() * 2);
                    temp_head->header_timestamp.hour      = BCD2INT(temp_head->header_timestamp.hour);
                    temp_head->header_timestamp.minute    = BCD2INT(temp_head->header_timestamp.minute);
                    temp_head->header_timestamp.second    = BCD2INT(temp_head->header_timestamp.second);
                    temp_head->header_timestamp.notes_len = BCD2INT(temp_head->header_timestamp.notes_len);
                    if (temp_head->info_buf.size() == 1) {
                        auto &end_head = m_data.back().back();
                        // 当没有文件信息时拷贝上一次的文件信息
                        temp_head->info_buf = end_head.head->info_buf;
                    } else {
                        m_data.push_back({});
                    }
                    head_ptr = temp_head;
                } else {
                    head_ptr = m_data.back().back().head;
                }

                for (auto f = 0; std::cmp_less(f, (decoderBuf->size() - infoLen) / ASCAN_FRAME_SIZE); f++) {
                    Union::__330::__DATType temp;
                    temp.head = head_ptr;
                    temp.ascan_data.resize(ASCAN_DATA_SIZE);
                    memcpy(temp.ascan_data.data(), &(*decoderBuf)[f * ASCAN_FRAME_SIZE], ASCAN_DATA_SIZE);
                    memcpy(temp.wave_para.data(), &(*decoderBuf)[f * ASCAN_FRAME_SIZE + ASCAN_DATA_SIZE], sizeof(temp.wave_para));
                    // FIXME:这边还有284个字节未使用?
                    if (std::accumulate(temp.ascan_data.begin(), temp.ascan_data.end(), 0, [](auto a, auto b) { return a + b; }) != 0) {
                        auto &list = m_data.back();
                        list.emplace_back(std::move(temp));
                    }
                }
            }
            if (ret != file_size) {
                qWarning(TAG) << "ret != file_size, fileName:" << m_fileName;
            }
            return file_size;
        } catch (std::exception &e) {
#if defined(QT_DEBUG)
            qFatal(e.what());
#else
            qCritical(TAG) << e.what();
#endif
            return 0;
        }
    }

    int DATType::getDataSize(void) const {
        return static_cast<int>(m_data[m_fileName_index].size());
    }

    std::vector<std::wstring> DATType::getFileNameList(void) const {
        std::vector<std::wstring> ret;
        for (const auto &it : m_data) {
            const auto info_buf = it.at(0).head->info_buf;
            auto       temp     = std::vector<uint16_t>(info_buf.begin() + 1, info_buf.end());
            ret.emplace_back(Union::LocationCodeToUTF8(temp).toStdWString());
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
        // 330 该参数未使用
        (void)idx;
        return Base::Performance();
    }

    std::string DATType::getDate(int idx) const {
        (void)idx;
        return m_date;
    }

    std::wstring DATType::getProbe(int idx) const {
        (void)idx;
        return Union::Base::Probe::Index2Name_QtExtra((getHead(idx).channel_status.sys >> 12) & 0x07).toStdWString();
    }

    double DATType::getProbeFrequence(int idx) const {
        (void)idx;
        return getHead(idx).channel_param.Frequence / 100.0;
    }

    std::string DATType::getProbeChipShape(int idx) const {
        (void)idx;
        return Union::Base::Probe::CreateProbeChipShape(
            (getHead(idx).channel_status.sys >> 12) & 0x07,
            getHead(idx).channel_param.Crystal_l / 1000,
            getHead(idx).channel_param.Crystal_w / 1000);
    }

    double DATType::getAngle(int idx) const {
        (void)idx;
        return getHead(idx).channel_param.Angle / 10.0;
    }

    double DATType::getSoundVelocity(int idx) const {
        (void)idx;
        return getHead(idx).channel_param.Speed;
    }

    double DATType::getFrontDistance(int idx) const {
        (void)idx;
        return getHead(idx).channel_param.Forward / 10.0;
    }

    double DATType::getZeroPointBias(int idx) const {
        (void)idx;
        return getHead(idx).channel_param.Offset / 160.0;
    }

    double DATType::getSamplingDelay(int idx) const {
        (void)idx;
        return KeepDecimals<1>(getAxisBias(idx));
    }

    int DATType::getChannel(int idx) const {
        (void)idx;
        return ((getHead(idx).channel_status.sys >> 12) & 0x0F) + 1;
    }

    std::string DATType::getInstrumentName(void) const {
        return "330 Continuous";
    }

    std::array<Base::Gate, 2> DATType::getGate(int idx) const {
        (void)idx;
        std::array<Base::Gate, 2> ret;
        for (auto i = 0; i < 2; i++) {
            ret[i].pos    = getHead(idx).gate_param[i].Position / 500.0;
            ret[i].width  = getHead(idx).gate_param[i].Width / 500.0;
            ret[i].height = getHead(idx).gate_param[i].Height / 200.0;
            if (ret[i].height > 0.0) {
                ret[i].enable = true;
            }
        }
        return ret;
    }

    const std::vector<uint8_t> &DATType::getScanData(int idx) const {
        return m_data[m_fileName_index].at(idx).ascan_data;
    }

    double DATType::getAxisBias(int idx) const {
        (void)idx;
        auto       ret = 0.0;
        const auto opt = getOption(idx);
        if (opt == 0) {
            ret = static_cast<double>(getHead(idx).channel_param.Delay) * static_cast<double>(getHead(idx).channel_param.Speed) / (20000.0) * std::cos(getAngle(idx) * std::numbers::pi / 180.0);
        } else if (opt == 1) {
            ret = static_cast<double>(getHead(idx).channel_param.Delay) * static_cast<double>(getHead(idx).channel_param.Speed) / (20000.0) * std::sin(getAngle(idx) * std::numbers::pi / 180.0);
        } else {
            ret = static_cast<double>(getHead(idx).channel_param.Delay) * static_cast<double>(getHead(idx).channel_param.Speed) / (20000.0);
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
            ret = static_cast<double>(getHead(idx).channel_param.Range) * std::cos(getAngle(idx) * std::numbers::pi / 180.0);
        } else if (opt == 1) {
            ret = static_cast<double>(getHead(idx).channel_param.Range) * std::sin(getAngle(idx) * std::numbers::pi / 180.0);
        } else {
            ret = static_cast<double>(getHead(idx).channel_param.Range);
        }
        if (getUnit(idx) > 1.0) {
            return Union::KeepDecimals<1>(ret / (10.0 * getUnit(idx)));
        }
        return Union::KeepDecimals<1>(ret / 10.0);
    }

    double DATType::getBaseGain(int idx) const {
        (void)idx;
        return getHead(idx).channel_param.BaseGain / 10.0;
    }

    double DATType::getScanGain(int idx) const {
        (void)idx;
        return getHead(idx).channel_param.CompGain / 10.0;
    }

    double DATType::getSurfaceCompentationGain(int idx) const {
        (void)idx;
        return getHead(idx).channel_param.SurfGain / 10.0;
    }

    int DATType::getSupression(int idx) const {
        (void)idx;
        return getHead(idx).channel_param.Reject;
    }

    Union::AScan::DistanceMode DATType::getDistanceMode(int idx) const {
        const auto opt = getOption(idx);
        if (opt == 0) {
            return Union::AScan::DistanceMode_Y;
        } else if (opt == 1) {
            return Union::AScan::DistanceMode_X;
        }
        return Union::AScan::DistanceMode_S;
    }

    std::optional<Base::AVG> DATType::getAVG(int idx) const {
        (void)idx;
        // 330 不使用该参数
        return std::nullopt;
    }

    std::optional<Base::DAC> DATType::getDAC(int idx) const {
        (void)idx;
        // 330 不使用该参数
        return std::nullopt;
    }

    Union::AScan::DAC_Standard DATType::getDACStandard(int idx) const {
        (void)idx;
        return Union::AScan::DAC_Standard();
    }

    QJsonArray DATType::createGateValue(int idx, double soft_gain) const {
        QJsonArray ret = Union::AScan::AScanIntf::createGateValue(idx, soft_gain);

        QString strMRange;
        int     mode;
        auto    channelStatus = getHead(idx).channel_status;
        auto    channelParam  = getHead(idx).channel_param;
        auto    systemStatus  = getHead(idx).system_status;
        auto    dacParam      = getHead(idx).dac;
        auto    wavepara      = m_data.at(m_fileName_index).at(idx).wave_para;
        if (((channelStatus.status >> 2) & 0x01) == 1) {
            mode = (((channelStatus.option >> 28) & 0x0f) % 6) + 1;
        } else if (((channelStatus.status >> 3) & 0x01) == 1) {
            int Ax = channelStatus.option; /*标度：垂直\水平\距离2位	d3d2*/
            Ax     = (Ax >> 26);
            Ax     = (Ax & 0x03);
            mode   = Ax + 10;
        } else
            mode = -1;

        if (mode == 1) {
            if (systemStatus.unit % 4) {
                strMRange = "Φ" + QString::number(dacParam.diameter / 10.0 / 25.4, 'f', 3) + " x " + QString::number(dacParam.length / 10.0 / 25.4, 'f', 3);

            } else {
                strMRange = "Φ" + QString::number(dacParam.diameter / 10) + " x " + QString::number(dacParam.length / 10);
            }
            float fOffset = int(wavepara[3]) / 10.0;
            if (fOffset > 0) {
                strMRange += " +" + QString::number(int(wavepara[3]) / 10.0, 'f', 1);
            } else {
                strMRange += " " + QString::number(int(wavepara[3]) / 10.0, 'f', 1);
            }
            strMRange += "dB";
        } else if (mode > 1) {
            if ((mode - 2) == 0)
                strMRange = "RL";
            else if ((mode - 3) == 0)
                strMRange = "SL";
            else if ((mode - 4) == 0)
                strMRange = "EL";
            else if ((mode - 5) == 0)
                strMRange = "4L";
            else if ((mode - 6) == 0)
                strMRange = "5L";
            else if ((mode - 7) == 0)
                strMRange = "6L";
            else if ((mode - 10) == 0 || (mode - 11) == 0) {
                if (systemStatus.unit % 4)
                    strMRange = "Φ" + QString::number(((float)(10 * pow(10, (int(wavepara[3]) + channelParam.lineGain[2] + 120) / 400.0))) / 10.0 / 25.4, 'f', 2);
                else
                    strMRange = "Φ" + QString::number(((int)(10 * pow(10, (int(wavepara[3]) + channelParam.lineGain[2] + 120) / 400.0) + 0.5)) / 10.0, 'f', 1);
                strMRange += "  ";
                if (systemStatus.unit % 4)
                    strMRange += "Φ" + QString::number((float)(pow(10, (channelParam.lineGain[2] + 120) / 400.0)) / 25.4, 'f', 2);
                else
                    strMRange += "Φ" + QString::number((int)(pow(10, (channelParam.lineGain[2] + 120) / 400.0) + 0.5));
                float fOffset = int(wavepara[3]) / 10.0;
                if (fOffset > 0)
                    strMRange += " +" + QString::number(int(wavepara[3]) / 10.0, 'f', 1);
                else
                    strMRange += " " + QString::number(int(wavepara[3]) / 10.0, 'f', 1);
                strMRange += "dB";
            }

            if ((mode < 10) || (mode > 11)) {
                auto Mid        = int(wavepara[3]);
                auto strMRange1 = QString::number(wavepara[2] / (10.0), 'f', 1);
                if (Mid > 0)
                    strMRange1 = " +" + QString::number(abs(Mid / 10.0), 'f', 1);
                else
                    strMRange1 = " -" + QString::number(abs(Mid / (10.0)), 'f', 1);
                strMRange = strMRange + strMRange1 + "dB";
            }

        } else if (mode < 1) {
            QString s0 = QString::asprintf("∧%0.1f %", (float)(int(wavepara[3]) / 10.0));
            strMRange  = "";
        }
        qDebug(QLoggingCategory("DAS")) << "strMRange" << strMRange;
        qDebug(QLoggingCategory("DAS")) << ret;
        auto obj1 = ret[0].toObject();
        auto obj2 = ret[1].toObject();
        if (strMRange.isEmpty()) {
            strMRange = "-";
        }
        obj1["equi"] = strMRange;
        // obj1["dist_a"] = QString::number(wavepara[0] / 10.0, 'f', 1);
        // obj1["dist_b"] = QString::number(wavepara[1] / 10.0, 'f', 1);
        // obj1["dist_c"] = QString::number(wavepara[2] / 10.0, 'f', 1);
        obj2["equi"] = "-";
        ret.replace(0, obj1);
        ret.replace(1, obj2);
        return ret;
    }

    const std::array<QVector<QPointF>, 3> &DATType::unResolvedGetDacLines(int idx) const {
        static std::array<QVector<QPointF>, 3> ret;
        Union::Temp::Unresovled::DrawDacParam  _temp;
        _temp.m_unit         = getHead(idx).system_status.unit;
        _temp.m_sys          = getHead(idx).system_status.sys;
        _temp.m_ch_status    = getHead(idx).channel_status.status;
        _temp.m_ch_option    = getHead(idx).channel_status.option;
        _temp.m_ch_sys       = getHead(idx).channel_status.status;
        _temp.m_ch_Range     = getHead(idx).channel_param.Range;
        _temp.m_ch_Delay     = getHead(idx).channel_param.Delay;
        _temp.m_ch_Speed     = getHead(idx).channel_param.Speed;
        _temp.m_ch_Crystal_w = getHead(idx).channel_param.Crystal_w;
        _temp.m_ch_Crystal_l = getHead(idx).channel_param.Crystal_l;
        _temp.m_ch_Frequence = getHead(idx).channel_param.Frequence;
        _temp.m_ch_lineGain  = getHead(idx).channel_param.lineGain;
        _temp.m_ch_BaseGain  = getHead(idx).channel_param.BaseGain;
        _temp.m_ch_gatedB    = getHead(idx).channel_param.gatedB;
        _temp.m_dac_db       = getHead(idx).dac.db;
        _temp.m_dac_dist     = getHead(idx).dac.dist;
        _temp.m_dac_num      = getHead(idx).dac.num;
        _temp.m_range_a      = getAxisLen(idx);
        ret                  = Temp::Unresovled::DrawDac(_temp);
        return ret;
    }

    void DATType::setUnResolvedGetDacLines(const std::array<QVector<QPointF>, 3> &dat, int idx) {
        (void)dat;
        (void)idx;
    }

    std::pair<double, double> DATType::getProbeSize(int idx) const {
        (void)idx;
        return std::make_pair(getHead(idx).channel_param.Crystal_l / 1000.0,
                              getHead(idx).channel_param.Crystal_w / 1000.0);
    }

    int DATType::getReplayTimerInterval() const {
        return 800;
    }

    int DATType::getOption(int idx) const noexcept {
        (void)idx;
        auto ret = getHead(idx).channel_status.option;
        ret >>= 24;
        ret &= 0x03;
        return static_cast<int>(ret);
    }

    double DATType::getUnit(int idx) const noexcept {
        (void)idx;
        if (getHead(idx).system_status.unit) {
            return 100.0;
        }
        return 1.0;
    }

    const __DATHead &DATType::getHead(int idx) const {
        return *(m_data.at(m_fileName_index).at(idx).head);
    }

    uint8_t DATType::convertDB2GateAMP(int idx, int db) const {
        return static_cast<uint8_t>(std::pow(10.0, (200 * log10(255) - db + getBaseGain(idx) * 10.0 + getSurfaceCompentationGain(idx) * 10.0) / 200.0));
    }

    void DATType::setFileName(const QString &fileName) {
        m_fileName = fileName;
    }

    const QString &DATType::getFileName() {
        return m_fileName;
    }
} // namespace Union::__330
