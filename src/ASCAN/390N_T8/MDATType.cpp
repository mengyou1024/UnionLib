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
        if (fileFlag != 0x556ee655) {
            return nullptr;
        }
        auto      _ret_ptr = std::make_unique<UnType>();
        auto&     ret      = *_ret_ptr;
        QFileInfo fileInfo(QString::fromStdWString(fileName));
        ret.m_fileNameList = {fileInfo.completeBaseName().toStdWString()};
        while (1) {
            uint8_t frame_head;
            fileStream >> frame_head;
            if (frame_head != 0x55) {
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
                case 6: {
                    auto& body = ret.m_data.second;
                    body.push_back({});
                    auto current_cur                                                                  = std::ssize(body) - 1;
                    auto& [ascan_data, channel_param, dac_param, avg_param, performance, camera_data] = body[current_cur];
                    if (current_cur > 0) {
                        auto& [ascan_data_last, channel_param_last, dac_param_last, avg_param_last, performance_last, camera_data_last] = body[current_cur - 1];

                        dac_param   = dac_param_last;
                        avg_param   = avg_param_last;
                        performance = performance_last;
                        camera_data = camera_data_last;
                    }
                    ascan_data.unserialize_payload(payload, payload_len);
                    break;
                }
                case 2: {
                    auto& [ascan_data, channel_param, dac_param, avg_param, performance, camera_data] = ret.m_data.second[std::ssize(ret.m_data.second) - 1];

                    auto _insert_dac = std::make_shared<DACParam>();
                    _insert_dac->unserialize_payload(fileStream);
                    dac_param = _insert_dac;
                    break;
                }

                case 3: {
                    auto& [ascan_data, channel_param, dac_param, avg_param, performance, camera_data] = ret.m_data.second[std::ssize(ret.m_data.second) - 1];

                    auto _insert_avg = std::make_shared<AVGParam>();
                    _insert_avg->unserialize_payload(payload);
                    avg_param = _insert_avg;
                    break;
                }

                case 4: {
                    auto& [ascan_data, channel_param, dac_param, avg_param, performance, camera_data] = ret.m_data.second[std::ssize(ret.m_data.second) - 1];

                    auto _insert_performance = std::make_shared<Performance>();
                    _insert_performance->unserialize_payload(payload);
                    performance = _insert_performance;
                    break;
                }

                case 5: {
                    auto& [ascan_data, channel_param, dac_param, avg_param, performance, camera_data] = ret.m_data.second[std::ssize(ret.m_data.second) - 1];

                    auto _insert_camera = std::make_shared<CameraData>();
                    _insert_camera->unserialize_payload(payload);
                    camera_data = _insert_camera;
                    break;
                }

                case 1: {
                    auto& [ascan_data, channel_param, dac_param, avg_param, performance, camera_data] = ret.m_data.second[std::ssize(ret.m_data.second) - 1];
                    channel_param.unserialize_payload(payload);
                    break;
                }

                default: {
                    qDebug(TAG) << "unrecognized class type:" << class_type;
                    break;
                }
            }
            uint8_t frame_tail;
            fileStream >> frame_tail;
            if (frame_tail != 0x6e) {
                qWarning(TAG) << "frame tail error:" << frame_tail;
            }
            if (file.size() <= file.pos()) {
                qDebug(TAG) << "read file end";
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

    Base::Performance UnType::getPerformance(int idx) const {
        const auto& [ascan_data, channel_param, dac_param, avg_param, performance, camera_data] = m_data.second[idx];
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
        const auto& [ascan_data, channel_param, dac_param, avg_param, performance, camera_data] = m_data.second[idx];
        return Union::Base::Probe::Index2Name_QtExtra(channel_param.probe).toStdWString();
    }

    double UnType::getProbeFrequence(int idx) const {
        const auto& [ascan_data, channel_param, dac_param, avg_param, performance, camera_data] = m_data.second[idx];
        return channel_param.probeFrequency;
    }

    std::string UnType::getProbeChipShape(int idx) const {
        const auto& [ascan_data, channel_param, dac_param, avg_param, performance, camera_data] = m_data.second[idx];
        return Union::Base::Probe::CreateProbeChipShape(channel_param.probe, channel_param.probeChipShapeWorD, channel_param.probeChipShapeLorZero);
    }

    double UnType::getAngle(int idx) const {
        const auto& [ascan_data, channel_param, dac_param, avg_param, performance, camera_data] = m_data.second[idx];
        return channel_param.angle;
    }

    double UnType::getSoundVelocity(int idx) const {
        const auto& [ascan_data, channel_param, dac_param, avg_param, performance, camera_data] = m_data.second[idx];
        return channel_param.soundVelocity;
    }

    double UnType::getFrontDistance(int idx) const {
        const auto& [ascan_data, channel_param, dac_param, avg_param, performance, camera_data] = m_data.second[idx];
        return channel_param.probeFrontDistance;
    }

    double UnType::getZeroPointBias(int idx) const {
        const auto& [ascan_data, channel_param, dac_param, avg_param, performance, camera_data] = m_data.second[idx];
        return channel_param.axisBias;
    }

    double UnType::getSamplingDelay(int idx) const {
        const auto& [ascan_data, channel_param, dac_param, avg_param, performance, camera_data] = m_data.second[idx];
        return channel_param.samplingDelay;
    }

    int UnType::getChannel(int idx) const {
        const auto& [ascan_data, channel_param, dac_param, avg_param, performance, camera_data] = m_data.second[idx];
        return channel_param.channel;
    }

    std::string UnType::getInstrumentName(void) const {
        auto& head = m_data.first;
        return head.getNameString().toStdString();
    }

    std::array<Base::Gate, 2> UnType::getGate(int idx) const {
        const auto& [ascan_data, channel_param, dac_param, avg_param, performance, camera_data] = m_data.second[idx];
        std::array<Base::Gate, 2> ret;
        ret[0].pos    = channel_param.gateAPos;
        ret[0].width  = channel_param.gateAWidth;
        ret[0].height = channel_param.gateAHeight;
        ret[0].enable = channel_param.gateAEnable;
        ret[1].pos    = channel_param.gateBPos;
        ret[1].width  = channel_param.gateBWidth;
        ret[1].height = channel_param.gateBHeight;
        ret[1].enable = channel_param.gateBEnable;
        return ret;
    }

    const std::vector<uint8_t>& UnType::getScanData(int idx) const {
        const auto& [ascan_data, channel_param, dac_param, avg_param, performance, camera_data] = m_data.second[idx];
        return ascan_data.m_data;
    }

    double UnType::getAxisBias(int idx) const {
        const auto& [ascan_data, channel_param, dac_param, avg_param, performance, camera_data] = m_data.second[idx];
        return channel_param.axisBias;
    }

    double UnType::getAxisLen(int idx) const {
        const auto& [ascan_data, channel_param, dac_param, avg_param, performance, camera_data] = m_data.second[idx];
        return channel_param.axisLen;
    }

    double UnType::getBaseGain(int idx) const {
        const auto& [ascan_data, channel_param, dac_param, avg_param, performance, camera_data] = m_data.second[idx];
        return channel_param.baseGain;
    }

    double UnType::getScanGain(int idx) const {
        const auto& [ascan_data, channel_param, dac_param, avg_param, performance, camera_data] = m_data.second[idx];
        return channel_param.scanGain;
    }

    double UnType::getSurfaceCompentationGain(int idx) const {
        const auto& [ascan_data, channel_param, dac_param, avg_param, performance, camera_data] = m_data.second[idx];
        return channel_param.compensatingGain;
    }

    double UnType::getSupression(int idx) const {
        const auto& [ascan_data, channel_param, dac_param, avg_param, performance, camera_data] = m_data.second[idx];
        return static_cast<int>(KeepDecimals<0>(channel_param.suppression));
    }

    DistanceMode UnType::getDistanceMode(int idx) const {
        const auto& [ascan_data, channel_param, dac_param, avg_param, performance, camera_data] = m_data.second[idx];
        return DistanceMode(channel_param.distanceMode);
    }

    std::optional<Base::AVG> UnType::getAVG(int idx) const {
        const auto& [ascan_data, channel_param, dac_param, avg_param, performance, camera_data] = m_data.second[idx];
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
            return ret;
        }
        return std::nullopt;
    }

    std::optional<Base::DAC> UnType::getDAC(int idx) const {
        const auto& [ascan_data, channel_param, dac_param, avg_param, performance, camera_data] = m_data.second[idx];
        if (dac_param != nullptr && dac_param->isReady) {
            Base::DAC ret;
            ret.baseGain  = dac_param->baseGain;
            ret.biasGain  = dac_param->compensatingGain;
            ret.gate      = 0;
            ret.isSubline = dac_param->onlyShowBaseLine;
            ret.index     = CONVERT_F_VECTOR_TO_DOUBLE(dac_param->index);
            ret.value     = CONVERT_F_VECTOR_TO_DOUBLE(dac_param->value);
            return ret;
        }
        return std::nullopt;
    }

    DAC_Standard UnType::getDACStandard(int idx) const {
        const auto& [ascan_data, channel_param, dac_param, avg_param, performance, camera_data] = m_data.second[idx];

        if (avg_param != nullptr) {
            DAC_Standard ret;
            ret.elBias = dac_param->criteriaBiasEL;
            ret.rlBias = dac_param->criteriaBiasRL;
            ret.slBias = dac_param->criteriaBiasSL;
            return ret;
        }
        return DAC_Standard();
    }

    std::pair<double, double> UnType::getProbeSize(int idx) const {
        const auto& [ascan_data, channel_param, dac_param, avg_param, performance, camera_data] = m_data.second[idx];
        return {channel_param.probeChipShapeWorD, channel_param.probeChipShapeLorZero};
    }

    bool UnType::showCameraImage(int idx) const {
        const auto& [ascan_data, channel_param, dac_param, avg_param, performance, camera_data] = m_data.second[idx];
        return camera_data != nullptr;
    }

    QImage UnType::getCameraImage(int idx) const {
        const auto& [ascan_data, channel_param, dac_param, avg_param, performance, camera_data] = m_data.second[idx];
        if (camera_data != nullptr) {
            return QImage(camera_data->m_data.data(), camera_data->m_width, camera_data->m_height, QImage::Format_RGB888).mirrored(true, true);
        }
        return QImage();
    }

} // namespace Union::__390N_T8::MDATType
