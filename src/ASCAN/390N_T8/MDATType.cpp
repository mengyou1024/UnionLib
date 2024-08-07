#include "MDATType.hpp"
#include <QFile>
#include <QLoggingCategory>

namespace {
    std::vector<double> CONVERT_F_VECTOR_TO_DOUBLE(const std::vector<float>& f) {
        std::vector<double> ret;
        ret.resize(f.size());
        std::transform(f.begin(), f.end(), ret.begin(), [](float val) -> double {
            return val;
        });
        return ret;
    }
} // namespace

static Q_LOGGING_CATEGORY(TAG, "MDATType");

namespace Union::__390N_T8::MDATType {
    std::unique_ptr<AScanIntf> UnType::FromFile(const std::wstring& fileName) {
        QFile file(QString::fromStdWString(fileName));
        file.open(QIODevice::ReadOnly);
        QDataStream fileStream(&file);
        fileStream.setByteOrder(QDataStream::LittleEndian);
        fileStream.setFloatingPointPrecision(QDataStream::FloatingPointPrecision::SinglePrecision);
        uint32_t fileFlag;
        fileStream >> fileFlag;
        if (fileFlag != FILE_MAGIC_CODE) {
            return nullptr;
        }
        auto      _ret_ptr = std::make_unique<UnType>();
        auto&     ret      = *_ret_ptr;
        QFileInfo fileInfo(QString::fromStdWString(fileName));
        ret.m_fileNameList = {fileInfo.completeBaseName().toStdWString()};
        int _frames        = 0;
        while (1) {
            uint8_t frame_head;
            fileStream >> frame_head;
            if (frame_head != FRAME_HEAD) {
                break;
            }
            uint16_t class_type;
            fileStream >> class_type;
            uint32_t payload_len;
            fileStream >> payload_len;
            // 创建一个新的DataStream用于存放payload, 防止格式错误导致读取越界
            QByteArray arr;
            arr.resize(static_cast<int>(payload_len));
            fileStream.readRawData(arr.data(), static_cast<int>(payload_len));
            QDataStream payload(arr);
            payload.setByteOrder(QDataStream::LittleEndian);
            payload.setFloatingPointPrecision(QDataStream::SinglePrecision);
            switch (class_type) {
                case 0: {
                    ret.m_data.first.unserialize_payload(payload);
                    break;
                }

                case 1: {
                    auto& channel_param = std::get<ID_CHANNEL_PARAM>(ret.m_data.second[std::ssize(ret.m_data.second) - 1]);
                    channel_param.unserialize_payload(payload);
                    break;
                }

                case 2: {
                    auto& dac_param   = std::get<ID_DAC_PARAM>(ret.m_data.second[std::ssize(ret.m_data.second) - 1]);
                    auto  _insert_dac = std::make_shared<DACParam>();
                    _insert_dac->unserialize_payload(payload);
                    dac_param = _insert_dac;
                    break;
                }

                case 3: {
                    auto& avg_param   = std::get<ID_AVG_PARAM>(ret.m_data.second[std::ssize(ret.m_data.second) - 1]);
                    auto  _insert_avg = std::make_shared<AVGParam>();
                    _insert_avg->unserialize_payload(payload);
                    avg_param = _insert_avg;
                    break;
                }

                case 4: {
                    auto& performance         = std::get<ID_PERFORMANCE>(ret.m_data.second[std::ssize(ret.m_data.second) - 1]);
                    auto  _insert_performance = std::make_shared<Performance>();
                    _insert_performance->unserialize_payload(payload);
                    performance = _insert_performance;
                    break;
                }

                case 5: {
                    auto& camera_data    = std::get<ID_CAMERA_DATA>(ret.m_data.second[std::ssize(ret.m_data.second) - 1]);
                    auto  _insert_camera = std::make_shared<CameraData>();
                    _insert_camera->unserialize_payload(payload);
                    camera_data = _insert_camera;
                    break;
                }

                case 6: {
                    auto& body = ret.m_data.second;
                    body.push_back({});
                    auto current_cur = std::ssize(body) - 1;
                    if (current_cur > 0) {
                        auto& dac_param      = std::get<ID_DAC_PARAM>(body[current_cur]);
                        auto& dac_param_last = std::get<ID_DAC_PARAM>(body[current_cur - 1]);
                        dac_param            = dac_param_last;

                        auto& avg_param      = std::get<ID_AVG_PARAM>(body[current_cur]);
                        auto& avg_param_last = std::get<ID_AVG_PARAM>(body[current_cur - 1]);
                        avg_param            = avg_param_last;

                        auto& performance      = std::get<ID_PERFORMANCE>(body[current_cur]);
                        auto& performance_last = std::get<ID_PERFORMANCE>(body[current_cur - 1]);
                        performance            = performance_last;

                        auto& camera_data      = std::get<ID_CAMERA_DATA>(body[current_cur]);
                        auto& camera_data_last = std::get<ID_CAMERA_DATA>(body[current_cur - 1]);
                        camera_data            = camera_data_last;

                        auto& cmp000      = std::get<ID_CMP000>(body[current_cur]);
                        auto& cmp000_last = std::get<ID_CMP000>(body[current_cur - 1]);
                        cmp000            = cmp000_last;
                    }
                    auto& ascan_data = std::get<ID_ASCAN_DATA>(body[current_cur]);
                    ascan_data.unserialize_payload(payload, payload_len);
                    _frames++;
                    break;
                }

                case 0x8000: {
                    auto& cmp000         = std::get<ID_CMP000>(ret.m_data.second[std::ssize(ret.m_data.second) - 1]);
                    auto  _insert_cmp000 = std::make_shared<CMP000>();
                    _insert_cmp000->unserialize_payload(payload);
                    cmp000 = _insert_cmp000;
                    break;
                }

                default: {
                    qCDebug(TAG) << "unrecognized class type:" << class_type;
                    break;
                }
            }
            uint8_t frame_tail;
            fileStream >> frame_tail;
            if (frame_tail != FRAME_TAIL) {
                qCWarning(TAG) << "frame tail error:" << frame_tail << "type class:" << class_type << "frames:" << _frames;
                break;
            }
            if (file.size() <= file.pos()) {
                qCDebug(TAG) << "read file end";
                break;
            }
        }

        return _ret_ptr;
    }

    size_t UnType::__Read(std::ifstream& file, size_t file_size) {
        Q_UNUSED(file)
        Q_UNUSED(file_size)
        throw std::runtime_error("this function will not be called");
    }

    int UnType::getDataSize(void) const {
        return std::ssize(m_data.second);
    }

    std::vector<std::wstring> UnType::getFileNameList(void) const {
        return m_fileNameList;
    }

    void UnType::setFileNameIndex(int index) {
        (void)index;
    }

    int UnType::getFileNameIndex() const {
        return 0;
    }

    Base::Performance UnType::getPerformance(int idx) const {
        const auto& performance = std::get<ID_PERFORMANCE>(m_data.second.at(idx));
        if (performance != nullptr) {
            Base::Performance ret;
            ret.horizontalLinearity = performance->horizontalLinearity;
            ret.verticalLinearity   = performance->verticalLinearity;
            ret.dynamicRange        = performance->dynamicRange;
            ret.resolution          = performance->resolution;
            ret.sensitivity         = performance->surplusSensitivity;

            return ret;
        }
        return {};
    }

    std::string UnType::getDate(int idx) const {
        (void)idx;
        const auto& head = m_data.first;
        return head.getTime().toStdString();
    }

    std::wstring UnType::getProbe(int idx) const {
        const auto& channel_param = std::get<ID_CHANNEL_PARAM>(m_data.second.at(idx));
        return Union::Base::Probe::Index2Name_QtExtra(channel_param.probe).toStdWString();
    }

    double UnType::getProbeFrequence(int idx) const {
        const auto& channel_param = std::get<ID_CHANNEL_PARAM>(m_data.second.at(idx));
        return channel_param.probeFrequency;
    }

    std::string UnType::getProbeChipShape(int idx) const {
        const auto& channel_param = std::get<ID_CHANNEL_PARAM>(m_data.second.at(idx));
        return Union::Base::Probe::CreateProbeChipShape(channel_param.probe, channel_param.probeChipShapeWorD, channel_param.probeChipShapeLorZero);
    }

    double UnType::getAngle(int idx) const {
        const auto& channel_param = std::get<ID_CHANNEL_PARAM>(m_data.second.at(idx));
        return channel_param.angle;
    }

    double UnType::getSoundVelocity(int idx) const {
        const auto& channel_param = std::get<ID_CHANNEL_PARAM>(m_data.second.at(idx));
        return channel_param.soundVelocity;
    }

    double UnType::getFrontDistance(int idx) const {
        const auto& channel_param = std::get<ID_CHANNEL_PARAM>(m_data.second.at(idx));
        return channel_param.probeFrontDistance;
    }

    double UnType::getZeroPointBias(int idx) const {
        const auto& channel_param = std::get<ID_CHANNEL_PARAM>(m_data.second.at(idx));
        return channel_param.zeroPoint;
    }

    double UnType::getSamplingDelay(int idx) const {
        return getAxisBias(idx);
    }

    int UnType::getChannel(int idx) const {
        const auto& channel_param = std::get<ID_CHANNEL_PARAM>(m_data.second.at(idx));
        return channel_param.channel;
    }

    std::string UnType::getInstrumentName(void) const {
        auto& head = m_data.first;
        return head.getNameString().toStdString();
    }

    std::array<Base::Gate, 2> UnType::getGate(int idx) const {
        const auto&               channel_param = std::get<ID_CHANNEL_PARAM>(m_data.second.at(idx));
        std::array<Base::Gate, 2> ret;
        ret[0].pos    = channel_param.gateAPos;
        ret[0].width  = channel_param.gateAWidth;
        ret[0].height = channel_param.gateAHeight;
        ret[0].enable = channel_param.gateAEnable;
        ret[1].pos    = channel_param.gateBPos;
        ret[1].width  = channel_param.gateBWidth;
        ret[1].height = channel_param.gateBHeight;
        ret[1].enable = channel_param.gateBEnable;
        if (ret[1].enable == false) {
            if (ret[1].width > 0.01 && ret[1].height > 0.01) {
                ret[1].enable = true;
            }
        }
        return ret;
    }

    const std::vector<uint8_t>& UnType::getScanData(int idx) const {
        const auto& ascan_data = std::get<ID_ASCAN_DATA>(m_data.second.at(idx));
        return ascan_data.m_data;
    }

    double UnType::getAxisBias(int idx) const {
        const auto& channel_param = std::get<ID_CHANNEL_PARAM>(m_data.second.at(idx));
        auto        delay_s       = channel_param.axisBias;
        auto        ret           = delay_s;
        const auto  angle         = getAngle(idx);

        switch (getDistanceMode(idx)) {
            case Union::AScan::DistanceMode::DistanceMode_X:
                ret = delay_s * std::sin(angle * M_PI / 180.0);
                break;
            case Union::AScan::DistanceMode::DistanceMode_Y:
                ret = delay_s * std::cos(angle * M_PI / 180.0);
                break;
            default:
                break;
        }
        return ret;
    }

    double UnType::getAxisLen(int idx) const {
        const auto& channel_param = std::get<ID_CHANNEL_PARAM>(m_data.second.at(idx));
        return channel_param.axisLen;
    }

    double UnType::getBaseGain(int idx) const {
        const auto& channel_param = std::get<ID_CHANNEL_PARAM>(m_data.second.at(idx));
        return channel_param.baseGain;
    }

    double UnType::getScanGain(int idx) const {
        const auto& channel_param = std::get<ID_CHANNEL_PARAM>(m_data.second.at(idx));
        return channel_param.scanGain;
    }

    double UnType::getSurfaceCompentationGain(int idx) const {
        const auto& channel_param = std::get<ID_CHANNEL_PARAM>(m_data.second.at(idx));
        return channel_param.compensatingGain;
    }

    double UnType::getSupression(int idx) const {
        const auto& channel_param = std::get<ID_CHANNEL_PARAM>(m_data.second.at(idx));
        return channel_param.suppression;
    }

    DistanceMode UnType::getDistanceMode(int idx) const {
        const auto& channel_param = std::get<ID_CHANNEL_PARAM>(m_data.second.at(idx));
        return DistanceMode(channel_param.distanceMode);
    }

    std::optional<Base::AVG> UnType::getAVG(int idx) const {
        const auto& avg_param = std::get<ID_AVG_PARAM>(m_data.second.at(idx));
        if (avg_param != nullptr && avg_param->isReady) {
            Base::AVG ret;
            ret.equivalent         = avg_param->equivalent;
            ret.diameter           = avg_param->diameter;
            ret.reflectorDianmeter = avg_param->reflectorDiameter;
            ret.reflectorMaxDepth  = avg_param->reflectorMaxDepth;
            ret.scanGain           = avg_param->scanGain;
            ret.baseGain           = avg_param->baseGain;
            ret.biasGain           = avg_param->compensatingGain;
            ret.gate               = 0;
            ret.isSubline          = avg_param->onlyShowBaseLine;
            ret.index              = CONVERT_F_VECTOR_TO_DOUBLE(avg_param->index);
            ret.value              = CONVERT_F_VECTOR_TO_DOUBLE(avg_param->value);
            ret.samplingAxis       = std::make_pair(avg_param->samplingXAxisBias, avg_param->samplingXAxisBias + avg_param->samplingXAxisLen);
            return ret;
        }
        return std::nullopt;
    }

    std::optional<Base::DAC> UnType::getDAC(int idx) const {
        const auto& dac_param = std::get<ID_DAC_PARAM>(m_data.second.at(idx));
        if (dac_param != nullptr && dac_param->isReady) {
            Base::DAC ret;
            ret.baseGain     = dac_param->baseGain;
            ret.biasGain     = dac_param->compensatingGain;
            ret.gate         = 0;
            ret.isSubline    = dac_param->onlyShowBaseLine;
            ret.index        = CONVERT_F_VECTOR_TO_DOUBLE(dac_param->index);
            ret.value        = CONVERT_F_VECTOR_TO_DOUBLE(dac_param->value);
            ret.samplingAxis = std::make_pair(dac_param->samplingXAxisBias, dac_param->samplingXAxisBias + dac_param->samplingXAxisLen);
            return ret;
        }
        return std::nullopt;
    }

    DAC_Standard UnType::getDACStandard(int idx) const {
        const auto& dac_param = std::get<ID_DAC_PARAM>(m_data.second.at(idx));
        if (dac_param != nullptr) {
            DAC_Standard ret;
            ret.elBias = dac_param->criteriaBiasEL;
            ret.rlBias = dac_param->criteriaBiasRL;
            ret.slBias = dac_param->criteriaBiasSL;
            return ret;
        }
        return DAC_Standard();
    }

    std::pair<double, double> UnType::getProbeSize(int idx) const {
        const auto& channel_param = std::get<ID_CHANNEL_PARAM>(m_data.second.at(idx));
        return {channel_param.probeChipShapeWorD, channel_param.probeChipShapeLorZero};
    }

    int UnType::getReplayTimerInterval() const {
        return 200;
    }

    QJsonArray UnType::createGateValue(int idx, double soft_gain) const {
        QJsonArray ret = Union::AScan::AScanIntf::createGateValue(idx, soft_gain);

        std::array<QString, 2> m_equi = {"-", "-"};
        std::array<QString, 2> m_a    = {"-", "-"};
        std::array<QString, 2> m_b    = {"-", "-"};
        std::array<QString, 2> m_c    = {"-", "-"};

        const auto& dac_param = std::get<ID_DAC_PARAM>(m_data.second.at(idx));
        const auto& avg_param = std::get<ID_AVG_PARAM>(m_data.second.at(idx));

        if (dac_param != nullptr && dac_param->isReady) {
            auto                 index      = dac_param->criteria;
            double               equivalent = dac_param->equivalent;
            constexpr std::array lstrequi   = {" ", "RL", "SL", "EL"};
            m_equi[0]                       = QString::asprintf("%s %+.1fdB", lstrequi[index], equivalent);
            m_equi[1]                       = getDacEquivalent(idx, 1);
        } else if (avg_param != nullptr && avg_param->isReady) {
            auto reflector_diameter = avg_param->reflectorDiameter;
            auto equivalent         = avg_param->equivalent;
            auto avg_diameter       = avg_param->diameter;
            m_equi[0]               = QString::asprintf("Φ%.1f  Φ%.1f%+.1fdB", avg_diameter, reflector_diameter, equivalent);
            m_equi[1]               = getAvgEquivalent(idx, 1);
        }

        auto obj1    = ret[0].toObject();
        auto obj2    = ret[1].toObject();
        obj1["equi"] = m_equi[0];
        obj2["equi"] = m_equi[1];
        ret.replace(0, obj1);
        ret.replace(1, obj2);
        return ret;
    }

    bool UnType::showCameraImage(int idx) const {
        const auto& camera_data = std::get<ID_CAMERA_DATA>(m_data.second.at(idx));
        return camera_data != nullptr;
    }

    QImage UnType::getCameraImage(int idx) const {
        const auto& camera_data = std::get<ID_CAMERA_DATA>(m_data.second.at(idx));
        if (camera_data != nullptr) {
            return QImage(camera_data->m_data.data(), camera_data->m_width, camera_data->m_height, QImage::Format_RGB888).mirrored(true, true);
        }
        return QImage();
    }

    bool UnType::isSpecialEnabled(int idx) const {
        const auto& cmp000 = std::get<ID_CMP000>(m_data.second.at(idx));
        if (cmp000 != nullptr) {
            return true;
        }
        return false;
    }

    int UnType::getDacLineNumber(int idx) const {
        const auto& cmp000 = std::get<ID_CMP000>(m_data.second.at(idx));
        if (cmp000 != nullptr) {
            return cmp000->LineNumber;
        }
        return 0;
    }

    double UnType::getDACLineBias(int idx, int lineIdx) const {
        const auto& cmp000  = std::get<ID_CMP000>(m_data.second.at(idx));
        const auto& dac_std = getDACStandard(idx);
        if (cmp000 != nullptr) {
            switch (lineIdx) {
                case 0:
                    return dac_std.rlBias;
                case 1:
                    return dac_std.slBias;
                case 2:
                    return dac_std.elBias;
                case 3:
                    return cmp000->criteriaBiasLine4;
                case 4:
                    return cmp000->criteriaBiasLine5;
                case 5:
                    return cmp000->criteriaBiasLine6;
            }
        }
        return 0.0;
    }

    bool UnType::gateBIsLostType(int idx) const {
        const auto& cmp000 = std::get<ID_CMP000>(m_data.second.at(idx));
        if (cmp000 != nullptr) {
            if (cmp000->gateBType == 0) {
                return true;
            } else {
                return false;
            }
        }
        return false;
    }

    std::optional<double> UnType::getWorkPieceThickness(int idx) const {
        auto cmp000 = std::get<ID_CMP000>(m_data.second.at(idx));
        if (cmp000 == nullptr) {
            return std::nullopt;
        }
        return cmp000->workPieceThickness;
    }

    QString UnType::getDacEquivalent(int idx, int gate_idx) const {
        if (!getDAC(idx).has_value() || !(getGate(idx).at(gate_idx % 2).enable)) {
            return "-";
        }

        auto gate_res = Union::Base::CalculateGateResult(getScanData(idx), getGate(idx).at(gate_idx % 2), false, getSupression(idx));
        if (!gate_res.has_value()) {
            return "-";
        }
        const auto& [pos, amp]   = gate_res.value();
        const auto& dac_param    = std::get<ID_DAC_PARAM>(m_data.second.at(idx));
        auto        dac_standard = getDACStandard(idx);

        auto             index     = dac_param->criteria;
        const std::array modify    = {0.0, dac_standard.rlBias, dac_standard.slBias, dac_standard.elBias};
        auto             dac_value = getDACLineExpr(idx)(getScanData(idx).size() * pos);
        if (!dac_value.has_value()) {
            return "-";
        }
        dac_value       = Union::CalculateGainOutput(dac_value.value(), modify[index]);
        auto equivalent = Union::CalculatedGain(dac_value.value(), amp);

        constexpr std::array lstrequi = {" ", "RL", "SL", "EL"};
        return QString::asprintf("%s %+.1fdB", lstrequi[index], KeepDecimals<1>(equivalent));
    }

    QString UnType::getAvgEquivalent(int idx, int gate_idx) const {
        if (!getAVG(idx).has_value() || !(getGate(idx).at(gate_idx % 2).enable)) {
            return "-";
        }
        auto gate_res = Union::Base::CalculateGateResult(getScanData(idx), getGate(idx).at(gate_idx % 2), false, getSupression(idx));
        if (!gate_res.has_value()) {
            return "-";
        }
        const auto& [pos, amp]         = gate_res.value();
        const auto& avg_param          = std::get<ID_AVG_PARAM>(m_data.second.at(idx));
        auto        reflector_diameter = avg_param->reflectorDiameter;
        auto        avg_diameter       = avg_param->diameter;
        auto        avg_value          = getAVGLineExpr(idx)(getScanData(idx).size() * pos);
        if (!avg_value.has_value()) {
            return "-";
        }
        auto equivalent = Union::CalculatedGain(avg_value.value(), amp);
        return QString::asprintf("Φ%.1f  Φ%.1f%+.1fdB", avg_diameter, reflector_diameter, KeepDecimals<1>(equivalent));
    }

} // namespace Union::__390N_T8::MDATType
