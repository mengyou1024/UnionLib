#include "json.hpp"
#include <QDateTime>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLoggingCategory>

namespace Union::__390N_T8 {
    T8_390N_JSON::T8_390N_JSON(const std::wstring &fileName) {
        QFile file(QString::fromStdWString(fileName));
        file.open(QIODevice::ReadOnly);
        QJsonParseError err;
        auto            doc = QJsonDocument::fromJson(file.readAll(), &err);
        file.close();
        if (doc.isObject()) {
            m_json = doc.object();
            m_data.clear();
            if (!m_json[CHANNEL_SCAN_VALUE.data()].isArray()) {
                throw std::runtime_error("can't find scan value data");
            }
            m_data.resize(JSON_390N_T8_ASCAN_LEN);
            for (auto i = 0; std::cmp_less(i, m_data.size()); i++) {
                m_data[i] = static_cast<uint8_t>(m_json[CHANNEL_SCAN_VALUE.data()].toArray().takeAt(i).toInt());
            }
        } else {
            qCritical(QLoggingCategory("390N&T8.JSON")) << err.errorString();
        }
    }

    size_t T8_390N_JSON::__Read(std::ifstream &file, size_t file_size) {
        (void)file;
        (void)file_size;
        throw std::runtime_error("this function should not be executed");
    }

    int T8_390N_JSON::getDataSize(void) const {
        return 1;
    }

    std::vector<std::wstring> T8_390N_JSON::getFileNameList(void) const {
        return {};
    }

    void T8_390N_JSON::setFileNameIndex(int index) {
        (void)index;
    }

    Base::Performance T8_390N_JSON::getPerformance(int idx) const {
        (void)idx;
        Base::Performance ret;
        try {
            Base::Performance temp;
            temp.horizontalLinearity = m_json[CHANNEL_YIQI_SHUIPIN.data()].toDouble();
            temp.verticalLinearity   = m_json[CHANNEL_YIQI_CHUIZHI.data()].toDouble();
            temp.resolution          = m_json[CHANNEL_YIQI_FBL.data()].toDouble();
            temp.dynamicRange        = m_json[CHANNEL_YIQI_DONGTAI.data()].toDouble();
            temp.sensitivity         = m_json[CHANNEL_YIQI_LMD.data()].toDouble();
            std::swap(ret, temp);
        } catch (...) {
            qWarning(QLoggingCategory("390N&T8.JSON")) << "error on function <" __FUNCTION__ ">";
        }
        return ret;
    }

    std::string T8_390N_JSON::getDate(int idx) const {
        (void)idx;
        auto const        now = std::chrono::system_clock::now();
        std::time_t       t   = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        tm                t_m = {};
        localtime_s(&t_m, &t);
        ss << std::put_time(&t_m, "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }

    std::wstring T8_390N_JSON::getProbe(int idx) const {
        (void)idx;
        try {
            auto probeIndex = m_json[CHANNEL_PROBE_TYPE.data()].toInt();
            return Union::Base::Probe::Index2Name_QtExtra(probeIndex).toStdWString();
        } catch (...) {
            qWarning(QLoggingCategory("390N&T8.JSON")) << "error on function <" __FUNCTION__ ">";
            return {};
        }
    }

    double T8_390N_JSON::getProbeFrequence(int idx) const {
        (void)idx;
        return m_json[CHANNEL_PROBE_FREQ.data()].toDouble();
    }

    std::string T8_390N_JSON::getProbeChipShape(int idx) const {
        (void)idx;
        try {
            auto probeIndex = m_json[CHANNEL_PROBE_TYPE.data()].toInt();
            if (!m_json[CHANNEL_PROBE_SIZE.data()].isArray()) {
                throw std::runtime_error("CHANNEL_PROBE_SIZE is not array!");
            }
            auto _chipSize = m_json[CHANNEL_PROBE_SIZE.data()].toArray();
            return Union::Base::Probe::CreateProbeChipShape(probeIndex, _chipSize[0].toInt(), _chipSize[1].toInt());
        } catch (...) {
            qWarning(QLoggingCategory("390N&T8.JSON")) << "error on function <" __FUNCTION__ ">";
            return {};
        }
    }

    double T8_390N_JSON::getAngle(int idx) const {
        (void)idx;
        if (!m_json[CHANNEL_K_VALUE.data()].isArray()) {
            throw std::runtime_error("CHANNEL_K_VALUE is not array!");
        }
        return m_json[CHANNEL_K_VALUE.data()].toArray()[1].toDouble();
    }

    double T8_390N_JSON::getSoundVelocity(int idx) const {
        (void)idx;
        return m_json[CHANNEL_SOUND_SPEED.data()].toDouble();
    }

    double T8_390N_JSON::getFrontDistance(int idx) const {
        (void)idx;
        return m_json[CHANNEL_PROBE_FRONTIER.data()].toDouble();
    }

    double T8_390N_JSON::getZeroPointBias(int idx) const {
        (void)idx;
        return m_json[CHANNEL_ZEROPOINT.data()].toDouble();
    }

    double T8_390N_JSON::getSamplingDelay(int idx) const {
        (void)idx;
        return m_json[CHANNEL_SAMPLING_DELAY.data()].toDouble();
    }

    int T8_390N_JSON::getChannel(int idx) const {
        (void)idx;
        return m_json[SYS_CHANNEL_ID.data()].toInt(-1);
    }

    std::string T8_390N_JSON::getInstrumentName(void) const {
        return "390N&T8 Single";
    }

    std::array<Base::Gate, 2> T8_390N_JSON::getGate(int idx) const {
        (void)idx;
        std::array<Base::Gate, 2> ret;
        ret[0].enable = m_json[CHANNEL_GATEA_ACTIVITY.data()].toBool();
        ret[0].pos    = (m_json[CHANNEL_GATEA_POS.data()].toDouble() - getAxisBias(0)) / getAxisLen(0);
        ret[0].width  = (m_json[CHANNEL_GATEA_WIDTH.data()].toDouble() - getAxisBias(0)) / getAxisLen(0);
        ret[0].height = m_json[CHANNEL_GATEA_HEIGHT.data()].toDouble() / 100.0;
        ret[1].enable = m_json[CHANNEL_GATEB_ACTIVITY.data()].toBool();
        ret[1].pos    = (m_json[CHANNEL_GATEB_POS.data()].toDouble() - getAxisLen(0)) / getAxisLen(0);
        ret[1].width  = (m_json[CHANNEL_GATEB_WIDTH.data()].toDouble() - getAxisBias(0)) / getAxisLen(0);
        ret[1].height = m_json[CHANNEL_GATEB_HEIGHT.data()].toDouble() / 100.0;
        return ret;
    }

    const std::vector<uint8_t> &T8_390N_JSON::getScanData(int idx) const {
        (void)idx;
        return m_data;
    }

    double T8_390N_JSON::getAxisBias(int idx) const {
        (void)idx;
        return m_json["ch_timedelay"].toDouble();
    }

    double T8_390N_JSON::getAxisLen(int idx) const {
        (void)idx;
        try {
            const auto sdt = m_json[CHANNEL_SOUNDDIST_TYPE.data()].toInt();
            if (!m_json[CHANNEL_SOUNDDIST.data()].isArray()) {
                throw std::runtime_error("CHANNEL_SOUNNDIST is not array!");
            }
            return m_json[CHANNEL_SOUNDDIST.data()].toArray()[sdt].toDouble();
        } catch (...) {
            qWarning(QLoggingCategory("390N&T8.JSON")) << "error on function <" __FUNCTION__ ">";
            return 0.0;
        }
    }

    double T8_390N_JSON::getBaseGain(int idx) const {
        (void)idx;
        return m_json[CHANNEL_BASE_GAIN.data()].toDouble();
    }

    double T8_390N_JSON::getScanGain(int idx) const {
        (void)idx;
        return m_json[CHANNEL_SCAN_GAIN.data()].toDouble();
    }

    double T8_390N_JSON::getSurfaceCompentationGain(int idx) const {
        (void)idx;
        return m_json[CHANNEL_OFFSET_GAIN.data()].toDouble();
    }

    int T8_390N_JSON::getSupression(int idx) const {
        (void)idx;
        return m_json[CHANNEL_SUPPRESSION.data()].toInt();
    }

    Union::AScan::DistanceMode T8_390N_JSON::getDistanceMode(int idx) const {
        (void)idx;
        return Union::AScan::DistanceMode(m_json[CHANNEL_SOUNDDIST_TYPE.data()].toInt());
    }

    std::optional<Base::AVG> T8_390N_JSON::getAVG(int idx) const {
        (void)idx;
        try {
            if (!m_json[CHANNEL_ALREADY_AVG.data()].toBool()) {
                return std::nullopt;
            }
            Base::AVG ret;
            ret.baseGain           = m_json[CHANNEL_AVG_BASE_GAIN.data()].toDouble();
            ret.biasGain           = m_json[CHANNEL_AVG_OFFSET_GAIN.data()].toDouble();
            ret.equivalent         = m_json[CHANNEL_AVG_DANGLIANG.data()].toDouble();
            ret.diameter           = m_json[CHANNEL_AVG_DIAMETER.data()].toDouble();
            ret.isSubline          = m_json[CHANNEL_AVG_IS_SUBLINE.data()].toBool();
            ret.reflectorDianmeter = m_json[CHANNEL_REFLECTOR_DIAMETER.data()].toDouble();
            ret.reflectorMaxDepth  = m_json[CHANNEL_REFLECTOR_MAX_DEPTH.data()].toDouble();
            ret.index.clear();
            ret.value.clear();
            if (!m_json[CHANNEL_AVG_SAMPLE_INDEX.data()].isArray() || !m_json[CHANNEL_AVG_SAMPLE_VALUE.data()].isArray()) {
                throw std::runtime_error("CHANNEL_AVG_SAMPLE is not array!");
            }
            if (!m_json[CHANNEL_AVG_SAMPLE_INDEX.data()].isArray() || m_json[CHANNEL_AVG_SAMPLE_INDEX.data()].toArray().size() < m_json[CHANNEL_AVG_SAMPLE_LENGTH.data()].toInt()) {
                throw std::runtime_error("ch_dac_sample_index is not array or array will overflow");
            }
            if (!m_json[CHANNEL_AVG_SAMPLE_VALUE.data()].isArray() || m_json[CHANNEL_AVG_SAMPLE_VALUE.data()].toArray().size() < m_json[CHANNEL_AVG_SAMPLE_LENGTH.data()].toInt()) {
                throw std::runtime_error("ch_dac_sample_value is not array or array will overflow");
            }
            auto avg_index = m_json[CHANNEL_AVG_SAMPLE_INDEX.data()].toArray();
            auto avg_value = m_json[CHANNEL_AVG_SAMPLE_VALUE.data()].toArray();
            for (auto i = 0; i < m_json[CHANNEL_AVG_SAMPLE_LENGTH.data()].toInt(); i++) {
                ret.index.emplace_back(avg_index[i].toInt());
                ret.value.emplace_back(static_cast<uint8_t>(avg_value[i].toInt()));
            }
            ret.samplingDepth    = m_json[CHANNEL_AVG_SAMPLE_DEPTH.data()].toInt();
            ret.decimationFactor = m_json[CHANNEL_AVG_SAMPLING_FACTOR.data()].toDouble();
            ret.scanGain         = m_json[CHANNEL_AVG_SCAN_GAIN.data()].toDouble();
            return ret;
        } catch (...) {
            return std::nullopt;
        }
    }

    std::optional<Base::DAC> T8_390N_JSON::getDAC(int idx) const {
        (void)idx;
        try {
            if (!m_json[CHANNEL_ALREADY_DAC.data()].toBool()) {
                return std::nullopt;
            }
            Base::DAC ret;
            ret.baseGain  = m_json[CHANNEL_DAC_BASE_GAIN.data()].toDouble();
            ret.gate      = m_json[CHANNEL_DAC_GATE.data()].toInt();
            ret.isSubline = m_json[CHANNEL_DAC_IS_SUBLINE.data()].toBool();
            ret.index.clear();
            ret.value.clear();
            if (!m_json[CHANNEL_DAC_SAMPLE_INDEX.data()].isArray() || !m_json[CHANNEL_DAC_SAMPLE_VALUE.data()].isArray()) {
                throw std::runtime_error("CHANNEL_DAC_SAMPLE is not array!");
            }

            if (!m_json[CHANNEL_DAC_SAMPLE_INDEX.data()].isArray() || m_json[CHANNEL_DAC_SAMPLE_INDEX.data()].toArray().size() < m_json[CHANNEL_DAC_SAMPLE_LENGTH.data()].toInt()) {
                throw std::runtime_error("ch_dac_sample_index is not array or array will overflow");
            }
            if (!m_json[CHANNEL_DAC_SAMPLE_VALUE.data()].isArray() || m_json[CHANNEL_DAC_SAMPLE_VALUE.data()].toArray().size() < m_json[CHANNEL_DAC_SAMPLE_LENGTH.data()].toInt()) {
                throw std::runtime_error("ch_dac_sample_value is not array or array will overflow");
            }
            auto dac_index = m_json[CHANNEL_DAC_SAMPLE_INDEX.data()].toArray();
            auto dac_value = m_json[CHANNEL_DAC_SAMPLE_VALUE.data()].toArray();
            for (auto i = 0; i < m_json[CHANNEL_DAC_SAMPLE_LENGTH.data()].toInt(); i++) {
                ret.index.emplace_back(dac_index[i].toInt());
                ret.value.emplace_back(static_cast<uint8_t>(dac_value[i].toInt()));
            }
            ret.samplingDepth    = m_json[CHANNEL_DAC_SAMPLE_DEPTH.data()].toInt();
            ret.decimationFactor = m_json[CHANNEL_DAC_SAMPLING_FACTOR.data()].toDouble();
            return ret;
        } catch (...) {
            return std::nullopt;
        }
    }

    Union::AScan::DAC_Standard T8_390N_JSON::getDACStandard(int idx) const {
        (void)idx;
        Union::AScan::DAC_Standard ret;
        ret.rlBias = m_json[CHANNEL_XUAN_RL.data()].toDouble();
        ret.slBias = m_json[CHANNEL_XUAN_SL.data()].toDouble();
        ret.elBias = m_json[CHANNEL_XUAN_EL.data()].toDouble();
        return ret;
    }

    std::function<double(double)> T8_390N_JSON::getAVGLineExpr(int idx) const {
        auto func = std::bind(Union::EchoDbDiffOfHole, std::placeholders::_1, 2.0, std::placeholders::_2, 2.0);
        return getLineExpr(idx, getAVG(idx)->index, getAVG(idx)->value, {0.0, 160.0}, {0.0, 520.0}, func);
    }

    std::function<double(double)> T8_390N_JSON::getDACLineExpr(int idx) const {
        return getLineExpr(idx, getDAC(idx)->index, getDAC(idx)->value, {0.0, 106.59}, {0.0, 520.0});
    }

    std::unique_ptr<Union::AScan::AScanIntf> T8_390N_JSON::FromFile(const std::wstring &file_name) {
        try {
            return std::make_unique<T8_390N_JSON>(file_name);
        } catch (const std::exception &e) {
            qCritical(QLoggingCategory("390N&T8.JSON")) << e.what();
        }
        return nullptr;
    }

} // namespace Union::__390N_T8
