#include "das.hpp"
#include <QFileInfo>
#include <Yo/File>
#include <Yo/Types>
#include <cstdio>
#include <numbers>

#include "_330_draw_dac.hpp"

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
            auto                                  _raw = dynamic_cast<DASType *>(ret.get());
            Union::Temp::Unresovled::DrawDacParam _temp;
            _temp.m_unit         = _raw->systemStatus.unit;
            _temp.m_sys          = _raw->systemStatus.sys;
            _temp.m_ch_status    = _raw->channelStatus.status;
            _temp.m_ch_option    = _raw->channelStatus.option;
            _temp.m_ch_sys       = _raw->channelStatus.status;
            _temp.m_ch_Range     = _raw->channelParam.range;
            _temp.m_ch_Delay     = _raw->channelParam.delay;
            _temp.m_ch_Speed     = _raw->channelParam.speed;
            _temp.m_ch_Crystal_w = _raw->channelParam.crystal_w;
            _temp.m_ch_Crystal_l = _raw->channelParam.crystal_l;
            _temp.m_ch_Frequence = _raw->channelParam.freqence;
            _temp.m_ch_lineGain  = _raw->channelParam.lineGain;
            _temp.m_ch_BaseGain  = _raw->channelParam.baseGain;
            _temp.m_ch_gatedB    = _raw->channelParam.gateDB;
            _temp.m_dac_db       = _raw->dacParam.dB;
            _temp.m_dac_dist     = _raw->dacParam.dist;
            _temp.m_dac_num      = _raw->dacParam.num;
            _temp.m_range_a      = _raw->getAxisLen(0);
            _raw->setUnResolvedGetDacLines(DrawDac(_temp), 0);
            QFileInfo info(QString::fromStdWString(fileName));
            _raw->m_fileNameList.push_back(info.baseName().toStdWString());
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
        return m_fileNameList;
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
        return Union::Base::Probe::Index2Name_QtExtra((channelStatus.sys >> 12) & 0x07).toStdWString();
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
        return channelParam.offset / 160.0;
    }

    double DASType::getSamplingDelay(int idx) const {
        (void)idx;
        return channelParam.delay;
    }

    int DASType::getChannel(int idx) const {
        (void)idx;
        return ((systemStatus.sys >> 12) & 0x0F) + 1;
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
        (void)idx;
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
        (void)idx;
        const auto opt = getOption();
        if (opt == 0) {
            return Union::AScan::DistanceMode_Y;
        } else if (opt == 1) {
            return Union::AScan::DistanceMode_X;
        }
        return Union::AScan::DistanceMode_S;
    }

    std::optional<Base::AVG> DASType::getAVG(int idx) const {
        (void)idx;
        // 330 不使用该参数
        return std::nullopt;
    }

    std::optional<Base::DAC> DASType::getDAC(int idx) const {
        (void)idx;
        // 330 不使用该参数
        return std::nullopt;
    }

    Union::AScan::DAC_Standard DASType::getDACStandard(int idx) const {
        (void)idx;
        // 330 不使用该参数
        return Union::AScan::DAC_Standard();
    }

    std::function<double(double)> DASType::getAVGLineExpr(int idx) const {
        (void)idx;
        // 330 不使用该参数
        return [](double) { return 0.0; };
    }

    std::function<double(double)> DASType::getDACLineExpr(int idx) const {
        (void)idx;
        // 330 不使用该参数
        return [](double) { return 0.0; };
    }

    const std::array<QVector<QPointF>, 3> &DASType::unResolvedGetDacLines(int idx) const {
        (void)idx;
        return m_dacLines;
    }

    void DASType::setUnResolvedGetDacLines(const std::array<QVector<QPointF>, 3> &dat, int idx) {
        (void)idx;
        m_dacLines = dat;
    }

    QJsonArray DASType::createGateValue(int idx, double soft_gain) const {
        QJsonArray ret = Union::AScan::AScanIntf::createGateValue(idx, soft_gain);

        QString strMRange;
        int     mode;
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
            float fOffset = int(channelParam.wavepara[3]) / 10.0;
            if (fOffset > 0) {
                strMRange += " +" + QString::number(int(channelParam.wavepara[3]) / 10.0, 'f', 1);
            } else {
                strMRange += " " + QString::number(int(channelParam.wavepara[3]) / 10.0, 'f', 1);
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
                    strMRange = "Φ" + QString::number(((float)(10 * pow(10, (int(channelParam.wavepara[3]) + channelParam.lineGain[2] + 120) / 400.0))) / 10.0 / 25.4, 'f', 2);
                else
                    strMRange = "Φ" + QString::number(((int)(10 * pow(10, (int(channelParam.wavepara[3]) + channelParam.lineGain[2] + 120) / 400.0) + 0.5)) / 10.0, 'f', 1);
                strMRange += "  ";
                if (systemStatus.unit % 4)
                    strMRange += "Φ" + QString::number((float)(pow(10, (channelParam.lineGain[2] + 120) / 400.0)) / 25.4, 'f', 2);
                else
                    strMRange += "Φ" + QString::number((int)(pow(10, (channelParam.lineGain[2] + 120) / 400.0) + 0.5));
                float fOffset = int(channelParam.wavepara[3]) / 10.0;
                if (fOffset > 0)
                    strMRange += " +" + QString::number(int(channelParam.wavepara[3]) / 10.0, 'f', 1);
                else
                    strMRange += " " + QString::number(int(channelParam.wavepara[3]) / 10.0, 'f', 1);
                strMRange += "dB";
            }

            if ((mode < 10) || (mode > 11)) {
                auto Mid        = int(channelParam.wavepara[3]);
                auto strMRange1 = QString::number(channelParam.wavepara[2] / (10.0), 'f', 1);
                if (Mid > 0)
                    strMRange1 = " +" + QString::number(abs(Mid / 10.0), 'f', 1);
                else
                    strMRange1 = " -" + QString::number(abs(Mid / (10.0)), 'f', 1);
                strMRange = strMRange + strMRange1 + "dB";
            }

        } else if (mode < 1) {
            QString s0 = QString::asprintf("∧%0.1f %", (float)(int(channelParam.wavepara[3]) / 10.0));
            (void)s0;
            strMRange = "";
        }
        qDebug(QLoggingCategory("DAS")) << "strMRange" << strMRange;
        qDebug(QLoggingCategory("DAS")) << ret;
        auto obj1      = ret[0].toObject();
        auto obj2      = ret[1].toObject();
        obj1["equi"]   = strMRange;
        obj1["dist_c"] = QString::number(channelParam.wavepara[0] / 10.0, 'f', 1);
        obj1["dist_a"] = QString::number(channelParam.wavepara[1] / 10.0, 'f', 1);
        obj1["dist_b"] = QString::number(channelParam.wavepara[2] / 10.0, 'f', 1);
        obj2["equi"]   = "-";
        ret.replace(0, obj1);
        ret.replace(1, obj2);
        return ret;
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

    uint8_t DASType::convertDB2GateAMP(int db) const {
        return static_cast<uint8_t>(std::pow(10.0, (200 * log10(255) - db + channelParam.baseGain + getSurfaceCompentationGain(0)) / 200.0));
    }

} // namespace Union::__330
