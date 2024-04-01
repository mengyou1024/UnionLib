#include "json.hpp"

#if __has_include("QtCore")
    #include <QDateTime>
    #include <QJsonArray>
    #include <QJsonDocument>
    #include <QJsonObject>
    #include <QLoggingCategory>

namespace Union::__390N_T8 {
    std::optional<Union::AScan::AScanType> __390N_T8_JSON_READ(const std::wstring &fileName) {
        QFile file(QString::fromStdWString(fileName));
        file.open(QIODevice::ReadOnly);
        QJsonParseError err;
        auto            doc = QJsonDocument::fromJson(file.readAll(), &err);
        if (doc.isObject()) {
            try {
                Union::AScan::AScanType ret = {};
                auto                    obj = doc.object();
                // TODO: 反序列化数据
                ret.data.resize(1);
                auto &ascan = ret.data[0];
                // scan data
                const auto _axisBias = obj["ch_timedelay"].toDouble();
                const auto sdt       = obj[CHANNEL_SOUNDDIST_TYPE.data()].toInt();
                if (!obj[CHANNEL_SOUNDDIST.data()].isArray() || obj[CHANNEL_SOUNDDIST.data()].toArray().size() < sdt) {
                    throw std::runtime_error("ch_sound_distance is not array or array will overflow");
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
                        throw std::runtime_error("ch_dac_sample_index is not array or array will overflow");
                    }
                    if (!obj[CHANNEL_DAC_SAMPLE_VALUE.data()].isArray() || obj[CHANNEL_DAC_SAMPLE_VALUE.data()].toArray().size() < obj[CHANNEL_DAC_SAMPLE_LENGTH.data()].toInt()) {
                        throw std::runtime_error("ch_dac_sample_value is not array or array will overflow");
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
                        throw std::runtime_error("ch_dac_sample_index is not array or array will overflow");
                    }
                    if (!obj[CHANNEL_AVG_SAMPLE_VALUE.data()].isArray() || obj[CHANNEL_AVG_SAMPLE_VALUE.data()].toArray().size() < obj[CHANNEL_AVG_SAMPLE_LENGTH.data()].toInt()) {
                        throw std::runtime_error("ch_dac_sample_value is not array or array will overflow");
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
    #if __has_include("QString")
                ret.probe = Union::Base::Probe::Index2Name_QtExtra(probeIndex).toStdWString();
    #else
                ret.probe = QString::fromStdString(std::string(Union::Base::Probe::Index2Name(probeIndex))).toStdWString();
    #endif
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
                ret.zeroPointBias  = obj[CHANNEL_SAMPLING_DELAY.data()].toDouble();
                ret.channel        = obj[SYS_CHANNEL_ID.data()].toInt(-1);
                ret.instrumentName = "390N&T8 Single";

                ascan.ascan.resize(JSON_390N_T8_ASCAN_LEN);
                if (!obj[CHANNEL_SCAN_VALUE.data()].isArray()) {
                    throw std::runtime_error("ch_scan_value is not array");
                }
                for (auto i = 0; std::cmp_less(i, ascan.ascan.size()); i++) {
                    ascan.ascan[i] = static_cast<uint8_t>(obj[CHANNEL_SCAN_VALUE.data()].toArray()[i].toInt());
                }
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
#endif
