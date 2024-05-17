#include "json.hpp"
#include "../common/common.hpp"
#include <QDateTime>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLoggingCategory>

namespace Union::__390N_T8 {
    T8_390N_JSON::T8_390N_JSON(const std::wstring &fileName) {
        m_ascan = __390N_T8_JSON_READ(fileName);
        if (!m_ascan.has_value()) {
            throw std::runtime_error("can't resolve json file.");
        }
        QFileInfo info(QString::fromStdWString(fileName));
        m_fileNameList.push_back(info.baseName().toStdWString());
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
        return m_fileNameList;
    }

    void T8_390N_JSON::setFileNameIndex(int index) {
        (void)index;
    }

    Base::Performance T8_390N_JSON::getPerformance(int idx) const {
        (void)idx;
        return m_ascan->performance;
    }

    std::string T8_390N_JSON::getDate(int idx) const {
        (void)idx;
#if USE_CURRENT_TIME
        auto const        now = std::chrono::system_clock::now();
        std::time_t       t   = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        tm                t_m = {};
        localtime_s(&t_m, &t);
        ss << std::put_time(&t_m, "%Y-%m-%d %H:%M:%S");
        return ss.str();
#else
        return "未传入参数";
#endif
    }

    std::wstring T8_390N_JSON::getProbe(int idx) const {
        (void)idx;
        return m_ascan->probe;
    }

    double T8_390N_JSON::getProbeFrequence(int idx) const {
        (void)idx;
        return m_ascan->probeFrequence;
    }

    std::string T8_390N_JSON::getProbeChipShape(int idx) const {
        (void)idx;
        return m_ascan->probeChipShape;
    }

    double T8_390N_JSON::getAngle(int idx) const {
        (void)idx;
        return m_ascan->angle;
    }

    double T8_390N_JSON::getSoundVelocity(int idx) const {
        (void)idx;
        return m_ascan->soundVelocity;
    }

    double T8_390N_JSON::getFrontDistance(int idx) const {
        (void)idx;
        return m_ascan->frontDistance;
    }

    double T8_390N_JSON::getZeroPointBias(int idx) const {
        (void)idx;
        return KeepDecimals(m_ascan->zeroPointBias / 1000.0);
    }

    double T8_390N_JSON::getSamplingDelay(int idx) const {
        (void)idx;
        return KeepDecimals<1>(m_ascan->samplingDelay);
    }

    int T8_390N_JSON::getChannel(int idx) const {
        (void)idx;
        return m_ascan->channel;
    }

    std::string T8_390N_JSON::getInstrumentName(void) const {
        return "390N&T8 Single";
    }

    std::array<Base::Gate, 2> T8_390N_JSON::getGate(int idx) const {
        (void)idx;
        return m_ascan->data[0].gate;
    }

    const std::vector<uint8_t> &T8_390N_JSON::getScanData(int idx) const {
        (void)idx;
        return m_ascan->data[0].ascan;
    }

    double T8_390N_JSON::getAxisBias(int idx) const {
        (void)idx;
        return m_ascan->data[0].axisBias;
    }

    double T8_390N_JSON::getAxisLen(int idx) const {
        (void)idx;
        return m_ascan->data[0].axisLen;
    }

    double T8_390N_JSON::getBaseGain(int idx) const {
        (void)idx;
        return m_ascan->data[0].baseGain;
    }

    double T8_390N_JSON::getScanGain(int idx) const {
        (void)idx;
        return m_ascan->data[0].scanGain;
    }

    double T8_390N_JSON::getSurfaceCompentationGain(int idx) const {
        (void)idx;
        return m_ascan->data[0].surfaceCompentationGain;
    }

    int T8_390N_JSON::getSupression(int idx) const {
        (void)idx;
        return m_ascan->data[0].suppression;
    }

    Union::AScan::DistanceMode T8_390N_JSON::getDistanceMode(int idx) const {
        (void)idx;
        return m_ascan->data[0].distanceMode;
    }

    std::optional<Base::AVG> T8_390N_JSON::getAVG(int idx) const {
        (void)idx;
        return m_ascan->data[0].avg;
    }

    std::optional<Base::DAC> T8_390N_JSON::getDAC(int idx) const {
        (void)idx;
        return m_ascan->data[0].dac;
    }

    Union::AScan::DAC_Standard T8_390N_JSON::getDACStandard(int idx) const {
        (void)idx;
        return m_ascan->data[0].std;
    }

    std::function<double(double)> T8_390N_JSON::getAVGLineExpr(int idx) const {
        auto func = std::bind(Union::EchoDbDiffOfHole, std::placeholders::_1, 2.0, std::placeholders::_2, 2.0);
        auto _ret = getLineExpr(idx, getAVG(idx)->index, getAVG(idx)->value, {0.0, 160.0}, {0.0, 520.0}, func);
        return [=, this](double val) -> double {
            auto modifyGain = getBaseGain(idx) + getScanGain(idx) + getSurfaceCompentationGain(idx) - getAVG(idx)->baseGain + getAVG(idx)->biasGain;
            return Union::CalculateGainOutput(_ret(val), modifyGain);
        };
    }

    std::function<double(double)> T8_390N_JSON::getDACLineExpr(int idx) const {
        auto _ret = getLineExpr(idx, getDAC(idx)->index, getDAC(idx)->value, {0.0, 106.59}, {0.0, 520.0});
        return [=, this](double val) -> double {
            auto modifyGain = getBaseGain(idx) + getScanGain(idx) - getDAC(idx)->baseGain + getDAC(idx)->biasGain;
            return Union::CalculateGainOutput(_ret(val), modifyGain);
        };
    }

    std::unique_ptr<Union::AScan::AScanIntf> T8_390N_JSON::FromFile(const std::wstring &file_name) {
        try {
            return std::make_unique<T8_390N_JSON>(file_name);
        } catch (const std::exception &e) {
            qCritical(QLoggingCategory("390N&T8.JSON")) << e.what();
        }
        return nullptr;
    }

    QJsonArray T8_390N_JSON::createGateValue(int idx, double soft_gain) const {
        QJsonArray ret  = Union::AScan::AScanIntf::createGateValue(idx, soft_gain);
        auto       obj1 = ret[0].toObject();
        auto       obj2 = ret[1].toObject();
        obj1["equi"]    = m_equi[0];
        obj1["dist_c"]  = m_c[0];
        obj1["dist_a"]  = m_a[0];
        obj1["dist_b"]  = m_b[0];
        obj2["equi"]    = m_equi[1];
        obj2["dist_c"]  = m_c[1];
        obj2["dist_a"]  = m_a[1];
        obj2["dist_b"]  = m_b[1];
        ret.replace(0, obj1);
        ret.replace(1, obj2);
        return ret;
    }

    std::optional<T8_390N_JSON::AScanType> T8_390N_JSON::__390N_T8_JSON_READ(const std::wstring &fileName) {
        QFile file(QString::fromStdWString(fileName));
        file.open(QIODevice::ReadOnly);
        QJsonParseError err;
        auto            doc = QJsonDocument::fromJson(file.readAll(), &err);
        if (doc.isObject()) {
            try {
                AScanType ret = {};
                auto      obj = doc.object();
                // TODO: 反序列化数据
                ret.data.resize(1);
                auto &ascan = ret.data[0];
                // scan data
                const auto _axisBias = obj["ch_timedelay"].toDouble();
                const auto sdt       = obj[CHANNEL_SOUNDDIST_TYPE.data()].toInt();
                if (!obj[CHANNEL_SOUNDDIST.data()].isArray() || obj[CHANNEL_SOUNDDIST.data()].toArray().size() < sdt) {
                    throw std::exception("ch_sound_distance is not array or array will overflow");
                }
                const auto _axisLen           = obj[CHANNEL_SOUNDDIST.data()].toArray()[sdt].toDouble();
                ascan.gate[0].enable          = obj[CHANNEL_GATEA_ACTIVITY.data()].toBool();
                ascan.gate[0].pos             = (obj[CHANNEL_GATEA_POS.data()].toDouble() - _axisBias) / _axisLen;
                ascan.gate[0].width           = (obj[CHANNEL_GATEA_WIDTH.data()].toDouble() - _axisBias) / _axisLen;
                ascan.gate[0].height          = obj[CHANNEL_GATEA_HEIGHT.data()].toDouble() / 100.0;
                ascan.gate[1].enable          = obj[CHANNEL_GATEB_ACTIVITY.data()].toBool();
                ascan.gate[1].pos             = (obj[CHANNEL_GATEB_POS.data()].toDouble() - _axisBias) / _axisLen;
                ascan.gate[1].width           = (obj[CHANNEL_GATEB_WIDTH.data()].toDouble() - _axisBias) / _axisLen;
                ascan.gate[1].height          = obj[CHANNEL_GATEB_HEIGHT.data()].toDouble() / 100.0;
                ascan.axisBias                = _axisBias;
                ascan.axisLen                 = _axisLen;
                ascan.baseGain                = obj[CHANNEL_BASE_GAIN.data()].toDouble();
                ascan.scanGain                = obj[CHANNEL_SCAN_GAIN.data()].toDouble();
                ascan.surfaceCompentationGain = obj[CHANNEL_OFFSET_GAIN.data()].toDouble();
                ascan.suppression             = obj[CHANNEL_SUPPRESSION.data()].toInt();
                ascan.distanceMode            = Union::AScan::DistanceMode(sdt);
                if (obj[CHANNEL_ALREADY_DAC.data()].toBool()) {
                    ascan.dac            = Base::DAC();
                    ascan.dac->baseGain  = obj[CHANNEL_DAC_BASE_GAIN.data()].toDouble();
                    ascan.dac->gate      = obj[CHANNEL_DAC_GATE.data()].toInt();
                    ascan.dac->isSubline = obj[CHANNEL_DAC_IS_SUBLINE.data()].toBool();
                    ascan.dac->index.clear();
                    ascan.dac->value.clear();
                    if (!obj[CHANNEL_DAC_SAMPLE_INDEX.data()].isArray() || obj[CHANNEL_DAC_SAMPLE_INDEX.data()].toArray().size() < obj[CHANNEL_DAC_SAMPLE_LENGTH.data()].toInt()) {
                        throw std::exception("ch_dac_sample_index is not array or array will overflow");
                    }
                    if (!obj[CHANNEL_DAC_SAMPLE_VALUE.data()].isArray() || obj[CHANNEL_DAC_SAMPLE_VALUE.data()].toArray().size() < obj[CHANNEL_DAC_SAMPLE_LENGTH.data()].toInt()) {
                        throw std::exception("ch_dac_sample_value is not array or array will overflow");
                    }
                    auto dac_index = obj[CHANNEL_DAC_SAMPLE_INDEX.data()].toArray();
                    auto dac_value = obj[CHANNEL_DAC_SAMPLE_VALUE.data()].toArray();
                    for (auto i = 0; i < obj[CHANNEL_DAC_SAMPLE_LENGTH.data()].toInt(); i++) {
                        ascan.dac->index.emplace_back(dac_index[i].toInt());
                        ascan.dac->value.emplace_back(static_cast<uint8_t>(dac_value[i].toInt()));
                    }
                    ascan.dac->samplingDepth    = obj[CHANNEL_DAC_SAMPLE_DEPTH.data()].toInt();
                    ascan.dac->decimationFactor = obj[CHANNEL_DAC_SAMPLING_FACTOR.data()].toDouble();
                }
                if (obj[CHANNEL_ALREADY_AVG.data()].toBool()) {
                    ascan.avg                     = Base::AVG();
                    ascan.avg->baseGain           = obj[CHANNEL_AVG_BASE_GAIN.data()].toDouble();
                    ascan.avg->biasGain           = obj[CHANNEL_AVG_OFFSET_GAIN.data()].toDouble();
                    ascan.avg->equivalent         = obj[CHANNEL_AVG_DANGLIANG.data()].toDouble();
                    ascan.avg->diameter           = obj[CHANNEL_AVG_DIAMETER.data()].toDouble();
                    ascan.avg->isSubline          = obj[CHANNEL_AVG_IS_SUBLINE.data()].toBool();
                    ascan.avg->reflectorDianmeter = obj[CHANNEL_REFLECTOR_DIAMETER.data()].toDouble();
                    ascan.avg->reflectorMaxDepth  = obj[CHANNEL_REFLECTOR_MAX_DEPTH.data()].toDouble();
                    ascan.avg->index.clear();
                    ascan.avg->value.clear();
                    if (!obj[CHANNEL_AVG_SAMPLE_INDEX.data()].isArray() || obj[CHANNEL_AVG_SAMPLE_INDEX.data()].toArray().size() < obj[CHANNEL_AVG_SAMPLE_LENGTH.data()].toInt()) {
                        throw std::exception("ch_dac_sample_index is not array or array will overflow");
                    }
                    if (!obj[CHANNEL_AVG_SAMPLE_VALUE.data()].isArray() || obj[CHANNEL_AVG_SAMPLE_VALUE.data()].toArray().size() < obj[CHANNEL_AVG_SAMPLE_LENGTH.data()].toInt()) {
                        throw std::exception("ch_dac_sample_value is not array or array will overflow");
                    }
                    auto avg_index = obj[CHANNEL_AVG_SAMPLE_INDEX.data()].toArray();
                    auto avg_value = obj[CHANNEL_AVG_SAMPLE_VALUE.data()].toArray();
                    for (auto i = 0; i < obj[CHANNEL_AVG_SAMPLE_LENGTH.data()].toInt(); i++) {
                        ascan.avg->index.emplace_back(avg_index[i].toInt());
                        ascan.avg->value.emplace_back(static_cast<uint8_t>(avg_value[i].toInt()));
                    }
                    ascan.avg->samplingDepth    = obj[CHANNEL_AVG_SAMPLE_DEPTH.data()].toInt();
                    ascan.avg->decimationFactor = obj[CHANNEL_AVG_SAMPLING_FACTOR.data()].toDouble();
                    ascan.avg->scanGain         = obj[CHANNEL_AVG_SCAN_GAIN.data()].toDouble();
                }
                ascan.std.rlBias = obj[CHANNEL_XUAN_RL.data()].toDouble();
                ascan.std.slBias = obj[CHANNEL_XUAN_SL.data()].toDouble();
                ascan.std.elBias = obj[CHANNEL_XUAN_EL.data()].toDouble();
                // performance
                ret.performance.horizontalLinearity = obj[CHANNEL_YIQI_SHUIPIN.data()].toDouble();
                ret.performance.verticalLinearity   = obj[CHANNEL_YIQI_CHUIZHI.data()].toDouble();
                ret.performance.resolution          = obj[CHANNEL_YIQI_FBL.data()].toDouble();
                ret.performance.dynamicRange        = obj[CHANNEL_YIQI_DONGTAI.data()].toDouble();
                ret.performance.sensitivity         = obj[CHANNEL_YIQI_LMD.data()].toDouble();
                // time
                ret.time = obj["time"].toString(QDateTime::currentDateTime().toString("yyyy-M-d H:m:s")).toStdString();
                // probe
                auto probeIndex = obj[CHANNEL_PROBE_TYPE.data()].toInt();
                ret.probe       = Union::Base::Probe::Index2Name_QtExtra(probeIndex).toStdWString();
                // probe freq
                ret.probeFrequence = obj[CHANNEL_PROBE_FREQ.data()].toDouble();
                // probe chip size
                auto _chipSize     = obj[CHANNEL_PROBE_SIZE.data()].toArray();
                ret.probeChipShape = Union::Base::Probe::CreateProbeChipShape(probeIndex, _chipSize[0].toInt(), _chipSize[1].toInt());
                // angle
                ret.angle = obj[CHANNEL_K_VALUE.data()].toArray()[1].toDouble();
                // sound velocity
                ret.soundVelocity = obj[CHANNEL_SOUND_SPEED.data()].toDouble();
                // front distance
                ret.frontDistance = obj[CHANNEL_PROBE_FRONTIER.data()].toDouble();
                // zero point bias
                ret.zeroPointBias = obj[CHANNEL_ZEROPOINT.data()].toDouble();
                // scanpling delay
                ret.samplingDelay  = obj["ch_timedelay"].toDouble();
                ret.channel        = obj[SYS_CHANNEL_ID.data()].toInt(-1);
                ret.instrumentName = "390N&T8 Single";

                ascan.ascan.resize(JSON_390N_T8_ASCAN_LEN);
                if (!obj[CHANNEL_SCAN_VALUE.data()].isArray()) {
                    throw std::exception("ch_scan_value is not array");
                }
                for (auto i = 0; std::cmp_less(i, ascan.ascan.size()); i++) {
                    ascan.ascan[i] = static_cast<uint8_t>(obj[CHANNEL_SCAN_VALUE.data()].toArray()[i].toInt());
                }
                if (obj[CHANNEL_ALREADY_DAC.data()].toBool()) {
                    int                  index        = obj[CHANNEL_EQUIVALENT_STANDARD.data()].toInt();
                    double               equivalent   = obj[CHANNEL_FLAW_EQUIVALENT.data()].toDouble();
                    double               equivalent_b = obj[CHANNEL_FLAW_EQUIVALENT_B.data()].toDouble();
                    constexpr std::array lstrequi     = {" ", "RL", "SL", "EL"};
                    m_equi[0]                         = QString::asprintf("%s %+.1fdB", lstrequi[index], equivalent);
                    m_equi[1]                         = QString::asprintf("%s %+.1fdB", lstrequi[index], equivalent_b);
                } else if (obj[CHANNEL_ALREADY_AVG.data()].toBool()) {
                    auto reflector_diameter = obj[CHANNEL_AVG_REFLECTOR_DIAMETER.data()].toDouble();
                    auto equivlant          = obj[CHANNEL_FLAW_EQUIVALENT.data()].toDouble();
                    auto avg_diameter       = obj[CHANNEL_AVG_DIAMETER.data()].toDouble();
                    auto equivlant_b        = obj[CHANNEL_FLAW_EQUIVALENT_B.data()].toDouble();
                    m_equi[0]               = QString::asprintf("Φ%.1f Φ%.1f %+.1fdB", avg_diameter, reflector_diameter, equivlant);
                    m_equi[1]               = QString::asprintf("Φ%.1f Φ%.1f %+.1fdB", avg_diameter, reflector_diameter, equivlant_b);
                }

                m_c[0] = QString::asprintf("%.1f", obj[CHANNEL_FLAW_ACTUAL_DIST.data()].toDouble());
                m_c[1] = QString::asprintf("%.1f", obj[CHANNEL_FLAW_ACTUAL_DIST_B.data()].toDouble());
                m_a[0] = QString::asprintf("%.1f", obj[CHANNEL_FLAW_HORIZONTAL_DIST.data()].toDouble());
                m_a[1] = QString::asprintf("%.1f", obj[CHANNEL_FLAW_HORIZONTAL_DIST_B.data()].toDouble());
                m_b[0] = QString::asprintf("%.1f", obj[CHANNEL_FLAW_DEPTH.data()].toDouble());
                m_b[1] = QString::asprintf("%.1f", obj[CHANNEL_FLAW_DEPTH_B.data()].toDouble());

                return ret;
            } catch (std::exception &e) {
                qCritical(QLoggingCategory("390N&T8.JSON")) << e.what();
                return std::nullopt;
            }
        } else {
            qWarning(QLoggingCategory("390N&T8.JSON")) << "parser 390N & T8 json data error";
            qCritical(QLoggingCategory("390N&T8.JSON")) << err.errorString();
        }
        return std::nullopt;
    }

} // namespace Union::__390N_T8
