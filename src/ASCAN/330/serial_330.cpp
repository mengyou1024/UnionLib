#include "serial_330.hpp"
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
        uint32_t file_total = 0;
        uint32_t len_fat    = 0;
        uint32_t len_file   = 0;
        size_t   ret        = 0;
        ret += Yo::File::__Read(file, Yo::File::SkipSize(64), file_size);
        ret += Yo::File::__Read(file, file_total, file_size);
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
        return offset;
    }

    double Serial_330::getSamplingDelay(int idx) const {
        (void)idx;
        auto delay = m_data[m_fileName_index].channelParam.Delay;
        return delay;
    }

    int Serial_330::getChannel(int idx) const {
        (void)idx;
        auto sys = m_data[m_fileName_index].channelStatus.sys;
        return ((sys >> 12) & 0x0F) + 1;
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

    int Serial_330::getSupression(int idx) const {
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
        return std::nullopt;
    }

    std::optional<Base::DAC> Serial_330::getDAC(int idx) const {
        return std::nullopt;
    }

    Union::AScan::DAC_Standard Serial_330::getDACStandard(int idx) const {
        return Union::AScan::DAC_Standard();
    }

    std::function<double(double)> Serial_330::getAVGLineExpr(int idx) const {
        return std::function<double(double)>();
    }

    std::function<double(double)> Serial_330::getDACLineExpr(int idx) const {
        return std::function<double(double)>();
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
