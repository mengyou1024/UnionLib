#include "serial_330.hpp"
#include "./_330_draw_dac.hpp"
#include <QLoggingCategory>
#include <QSerialPort>
#include <Yo/File>
#include <Yo/Types>

static Q_LOGGING_CATEGORY(TAG, "330.Serial");

namespace Yo::File {
    template <>
    size_t __Read(std::ifstream &file, Union::__330::Serial_330 &data, [[maybe_unused]] size_t file_size) {
        return data.__Read(file, file_size);
    }

} // namespace Yo::File

namespace Union::__330 {
    std::optional<QString> Serial_330::ReadSerialAndSaveFile(const QString &portName, const QString &cachePath) {
        QSerialPort port(portName);
        if (!port.open(QIODevice::ReadWrite)) {
            return std::nullopt;
        }
        QByteArray readed;
        auto       read_times = 0;
        do {
            port.write("\x30", 1);
            if (port.waitForReadyRead(100)) {
                readed += port.readAll();
                while (port.waitForReadyRead(100)) {
                    readed += port.readAll();
                }
                break;
            }
            read_times++;
        } while (read_times < 300);
        QFile file(cachePath + "/File.cod");
        file.open(QIODevice::WriteOnly);
        file.write(readed);
        file.fileName();
        auto fileName = file.fileName();
        qInfo(TAG) << "save file:" << fileName;
        return fileName;
    }

    std::unique_ptr<Union::AScan::AScanIntf> Serial_330::FromFile(const std::wstring &fileName) {
        auto ret = std::make_unique<Serial_330>();
        auto res = Yo::File::ReadFile(fileName, *(ret.get()));
        if (res) {
            return ret;
        }
        return nullptr;
    }

    size_t Serial_330::__Read(std::ifstream &file, size_t file_size) {
        try {
            uint32_t file_total = 0;
            uint32_t len_fat    = 0;
            uint32_t len_file   = 0;
            size_t   ret        = 0;
            ret += Yo::File::__Read(file, Yo::File::SkipSize(64), file_size);
            ret += Yo::File::__Read(file, file_total, file_size);
            if (file_total == 0) {
                return 0;
            }
            ret += Yo::File::__Read(file, len_fat, file_size);
            ret += Yo::File::__Read(file, len_file, file_size);
            uint8_t ins_name = 0;
            ret += Yo::File::__Read(file, ins_name, file_size);

            switch (ins_name) {
                case 0xF:
                    m_instrumentName = "PXUT-F4";
                    break;
                default:
                    m_instrumentName = "UnKnow";
            }

            ret += Yo::File::__Read(file, Yo::File::SkipSize(3), file_size);

            uint32_t horizontal  = 0;
            uint32_t vertical    = 0;
            uint32_t resolveing  = 0;
            uint32_t sensitivity = 0;
            uint32_t dynamic     = 0;

            ret += Yo::File::__Read(file, horizontal, file_size);
            ret += Yo::File::__Read(file, vertical, file_size);
            ret += Yo::File::__Read(file, resolveing, file_size);
            ret += Yo::File::__Read(file, sensitivity, file_size);
            ret += Yo::File::__Read(file, dynamic, file_size);

            m_data.resize(file_total);

            for (auto &it : m_data) {
                ret += Yo::File::__Read(file, it.fat, file_size);
            }

            for (auto &it : m_data) {
                ret += Yo::File::__Read(file, Yo::File::MakeStructSub(&(it.systemStatus), &(it.ascanData), 0), file_size);
                it.ascanData.resize(480);
                it.dacData.resize(480);
                ret += Yo::File::__Read(file, it.ascanData, file_size);
                ret += Yo::File::__Read(file, it.dacData, file_size);
                auto residual = len_file - Yo::File::MakeStructSub(&(it.systemStatus), &(it.ascanData), 0).size() - (480 * 2);
                ret += Yo::File::__Read(file, Yo::File::SkipSize(residual), file_size);
            }

            return ret;
        } catch (std::exception &e) {
            qCCritical(TAG) << e.what();
            return 0;
        }
    }

    int Serial_330::getDataSize(void) const {
        return 1;
    }

    std::vector<std::wstring> Serial_330::getFileNameList(void) const {
        std::vector<std::wstring> ret;
        for (auto &it : m_data) {
            std::wstring s;
            for (auto i = 0; i < it.fat.name[0]; i++) {
                s += it.fat.name[i + 1];
            }
            ret.emplace_back(s);
        }
        return ret;
    }

    void Serial_330::setFileNameIndex(int index) {
        if (index < 0 || std::cmp_greater_equal(index, m_data.size())) {
            return;
        }
        m_fileName_index = index;
    }

    int Serial_330::getFileNameIndex() const {
        return m_fileName_index;
    }

    Base::Performance Serial_330::getPerformance(int idx) const {
        (void)idx;
        return m_performance;
    }

    std::string Serial_330::getDate(int idx) const {
        (void)idx;
        char           buf[11];
        const uint8_t *bias = reinterpret_cast<const uint8_t *>(&(m_data[m_fileName_index].fat.data));
        snprintf(buf, sizeof(buf), "%02x%02x-%02x-%02x", bias[0], bias[1], bias[2], bias[3]);
        return std::string(buf, 10);
    }

    std::wstring Serial_330::getProbe(int idx) const {
        (void)idx;
        auto sys = m_data[m_fileName_index].channelStatus.sys;
        return Union::Base::Probe::Index2Name_QtExtra((sys >> 12) & 0x07).toStdWString();
    }

    double Serial_330::getProbeFrequence(int idx) const {
        (void)idx;
        auto freq = m_data[m_fileName_index].channelParam.Frequence;
        return freq / 100.0;
    }

    std::string Serial_330::getProbeChipShape(int idx) const {
        (void)idx;
        auto sys       = m_data[m_fileName_index].channelStatus.sys;
        auto crystal_l = m_data[m_fileName_index].channelParam.Crystal_l;
        auto crystal_w = m_data[m_fileName_index].channelParam.Crystal_w;
        return Union::Base::Probe::CreateProbeChipShape(((sys >> 12) & 0x07), crystal_l / 1000, crystal_w / 1000);
    }

    double Serial_330::getAngle(int idx) const {
        (void)idx;
        auto angle = m_data[m_fileName_index].channelParam.Angle;
        return angle / 10.0;
    }

    double Serial_330::getSoundVelocity(int idx) const {
        (void)idx;
        auto speed = m_data[m_fileName_index].channelParam.Speed;
        return speed;
    }

    double Serial_330::getFrontDistance(int idx) const {
        (void)idx;
        auto forward = m_data[m_fileName_index].channelParam.Forward;
        return forward / 10.0;
    }

    double Serial_330::getZeroPointBias(int idx) const {
        (void)idx;
        auto offset = m_data[m_fileName_index].channelParam.Offset;
        return offset / 160.0;
    }

    double Serial_330::getSamplingDelay(int idx) const {
        (void)idx;
        auto delay = m_data[m_fileName_index].channelParam.Delay;
        return delay;
    }

    int Serial_330::getChannel(int idx) const {
        (void)idx;
        auto sys = m_data[m_fileName_index].channelStatus.sys;
        return ((sys >> 12) & 0x0F);
    }

    std::string Serial_330::getInstrumentName(void) const {
        return m_instrumentName;
    }

    std::array<Base::Gate, 2> Serial_330::getGate(int idx) const {
        (void)idx;
        const auto               &gateParam = m_data[m_fileName_index].gatePraram;
        std::array<Base::Gate, 2> ret;
        for (auto i = 0; i < 2; i++) {
            ret[i].pos    = gateParam[i].gate.Position / 480.0;
            ret[i].width  = gateParam[i].gate.Width / 480.0;
            ret[i].height = gateParam[i].gate.Height / 200.0;
            if (ret[i].height > 0.0) {
                ret[i].enable = true;
            }
        }
        return ret;
    }

    const std::vector<uint8_t> &Serial_330::getScanData(int idx) const {
        (void)idx;
        return m_data[m_fileName_index].ascanData;
    }

    double Serial_330::getAxisBias(int idx) const {
        (void)idx;
        auto        ret          = 0.0;
        const auto  opt          = getOption();
        const auto &channelParam = m_data[m_fileName_index].channelParam;
        if (opt == 0) {
            ret = static_cast<double>(channelParam.Delay) * static_cast<double>(channelParam.Speed) / (20000.0) * std::cos(getAngle(idx) * std::numbers::pi / 180.0);
        } else if (opt == 1) {
            ret = static_cast<double>(channelParam.Delay) * static_cast<double>(channelParam.Speed) / (20000.0) * std::sin(getAngle(idx) * std::numbers::pi / 180.0);
        } else {
            ret = static_cast<double>(channelParam.Delay) * static_cast<double>(channelParam.Speed) / (20000.0);
        }
        if (getUnit() > 1.0) {
            return Union::KeepDecimals(ret / (10.0 * getUnit()));
        }
        return Union::KeepDecimals(ret / (10.0 * getUnit()));
    }

    double Serial_330::getAxisLen(int idx) const {
        (void)idx;
        const auto  opt          = getOption();
        auto        ret          = 0.0;
        const auto &channelParam = m_data[m_fileName_index].channelParam;
        if (opt == 0) {
            ret = static_cast<double>(channelParam.Range) * std::cos(getAngle(idx) * std::numbers::pi / 180.0);
        } else if (opt == 1) {
            ret = static_cast<double>(channelParam.Range) * std::sin(getAngle(idx) * std::numbers::pi / 180.0);
        } else {
            ret = static_cast<double>(channelParam.Range);
        }
        if (getUnit() > 1.0) {
            return Union::KeepDecimals<1>(ret / (10.0 * getUnit()));
        }
        return Union::KeepDecimals<1>(ret / 10.0);
    }

    double Serial_330::getBaseGain(int idx) const {
        (void)idx;
        auto baseGain = m_data[m_fileName_index].channelParam.BaseGain;
        return baseGain / 10.0;
    }

    double Serial_330::getScanGain(int idx) const {
        (void)idx;
        auto compGain = m_data[m_fileName_index].channelParam.CompGain;
        return compGain / 10.0;
    }

    double Serial_330::getSurfaceCompentationGain(int idx) const {
        (void)idx;
        auto surGain = m_data[m_fileName_index].channelParam.SurfGain;
        return surGain / 10.0;
    }

    double Serial_330::getSupression(int idx) const {
        (void)idx;
        auto reject = m_data[m_fileName_index].channelParam.Reject;
        return reject;
    }

    Union::AScan::DistanceMode Serial_330::getDistanceMode(int idx) const {
        (void)idx;
        const auto opt = getOption();
        if (opt == 0) {
            return Union::AScan::DistanceMode_Y;
        } else if (opt == 1) {
            return Union::AScan::DistanceMode_X;
        }
        return Union::AScan::DistanceMode_S;
    }

    std::optional<Base::AVG> Serial_330::getAVG(int idx) const {
        (void)idx;
        // 330不使用该参数
        return std::nullopt;
    }

    std::optional<Base::DAC> Serial_330::getDAC(int idx) const {
        (void)idx;
        // 330 不使用该参数
        return Base::DAC();
    }

    Union::AScan::DAC_Standard Serial_330::getDACStandard(int idx) const {
        (void)idx;
        // 330 不使用该参数
        return Union::AScan::DAC_Standard();
    }

    QJsonArray Serial_330::createGateValue(int idx, double soft_gain) const {
        QJsonArray ret = Union::AScan::AScanIntf::createGateValue(idx, soft_gain);

        auto channelStatus = m_data[m_fileName_index].channelStatus;
        auto channelParam  = m_data[m_fileName_index].channelParam;
        auto systemStatus  = m_data[m_fileName_index].systemStatus;
        auto dacParam      = m_data[m_fileName_index].dac;

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
        qCDebug(TAG) << "strMRange" << strMRange;
        qCDebug(TAG) << ret;
        auto obj1      = ret[0].toObject();
        auto obj2      = ret[1].toObject();
        obj1["equi"]   = strMRange;
        obj2["equi"]   = "-";
        obj1["dist_c"] = QString::number(channelParam.wavepara[0] / 10.0, 'f', 1);
        obj1["dist_a"] = QString::number(channelParam.wavepara[1] / 10.0, 'f', 1);
        obj1["dist_b"] = QString::number(channelParam.wavepara[2] / 10.0, 'f', 1);
        ret.replace(0, obj1);
        ret.replace(1, obj2);
        return ret;
    }

    const std::array<QVector<QPointF>, 3> &Serial_330::unResolvedGetDacLines(int idx) const {
        if (m_dac_map.contains(m_fileName_index)) {
            return m_dac_map.at(m_fileName_index);
        } else {
            Union::Temp::Unresovled::DrawDacParam _temp;
            _temp.m_unit         = m_data.at(m_fileName_index).systemStatus.unit;
            _temp.m_sys          = m_data.at(m_fileName_index).systemStatus.sys;
            _temp.m_ch_status    = m_data.at(m_fileName_index).channelStatus.status;
            _temp.m_ch_option    = m_data.at(m_fileName_index).channelStatus.option;
            _temp.m_ch_sys       = m_data.at(m_fileName_index).channelStatus.status;
            _temp.m_ch_Range     = m_data.at(m_fileName_index).channelParam.Range;
            _temp.m_ch_Delay     = m_data.at(m_fileName_index).channelParam.Delay;
            _temp.m_ch_Speed     = m_data.at(m_fileName_index).channelParam.Speed;
            _temp.m_ch_Crystal_w = m_data.at(m_fileName_index).channelParam.Crystal_w;
            _temp.m_ch_Crystal_l = m_data.at(m_fileName_index).channelParam.Crystal_l;
            _temp.m_ch_Frequence = m_data.at(m_fileName_index).channelParam.Frequence;
            _temp.m_ch_lineGain  = m_data.at(m_fileName_index).channelParam.lineGain;
            _temp.m_ch_BaseGain  = m_data.at(m_fileName_index).channelParam.BaseGain;
            _temp.m_ch_gatedB    = m_data.at(m_fileName_index).channelParam.gatedB;
            _temp.m_dac_db       = m_data.at(m_fileName_index).dac.db;
            _temp.m_dac_dist     = m_data.at(m_fileName_index).dac.dist;
            _temp.m_dac_num      = m_data.at(m_fileName_index).dac.num;
            _temp.m_range_a      = getAxisLen(idx);
            m_dac_map.insert(std::make_pair(m_fileName_index, Temp::Unresovled::DrawDac(_temp)));
            return m_dac_map.at(m_fileName_index);
        }
    }

    void Serial_330::setUnResolvedGetDacLines(const std::array<QVector<QPointF>, 3> &dat, int idx) {
        (void)dat;
        (void)idx;
    }

    std::pair<double, double> Serial_330::getProbeSize(int idx) const {
        (void)idx;
        auto crystal_l = m_data[m_fileName_index].channelParam.Crystal_l;
        auto crystal_w = m_data[m_fileName_index].channelParam.Crystal_w;
        return std::make_pair(crystal_l / 1000.0, crystal_w / 1000.0);
    }

    int Serial_330::getReplayTimerInterval() const {
        return 800;
    }

    int Serial_330::getOption(void) const noexcept {
        auto ret = m_data[m_fileName_index].systemStatus.option;
        ret >>= 24;
        ret &= 0x03;
        return ret;
    }

    double Serial_330::getUnit(void) const noexcept {
        if (m_data[m_fileName_index].systemStatus.unit) {
            return 100.0;
        }
        return 1.0;
    }

} // namespace Union::__330
