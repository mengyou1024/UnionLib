#include "_8CH_USB.hpp"
#include <QLoggingCategory>

static Q_LOGGING_CATEGORY(TAG, "Bridge.8CH");

namespace {
    struct GainTable {
        int                 point1 = {};
        int                 point2 = {};
        std::array<int, 42> val    = {};
    };

    consteval GainTable CreateGainTable() {
        std::array v = {30, 40, 49, 58, 67, 76, 85, 94, 103, 112, 121, 130, 139, 148, 157, 166, 175, 184, 193, 202, 211, 220, 229, 238, 247, 256, 265, 274, 283, 292, 301, 310, 319, 328, 337, 346, 355, 364, 373, 382, 391, 400};
        GainTable  ret;
        ret.point1 = 34;
        ret.point2 = 70;
        ret.val    = v;
        return ret;
    }
    static constinit GainTable g_gainTable = CreateGainTable();
} // namespace

namespace Union::Bridge::MultiChannelHardwareBridge {
    _8CH_USB::_8CH_USB(int gate_number) :
    m_gate_number(gate_number) {
        // 创建并查找设备句柄
        DWORD     numDevs  = 0;
        FT_STATUS ftStatus = FT_CreateDeviceInfoList(&numDevs);
        if (ftStatus != FT_OK || numDevs == 0) {
            qFatal("create device info list error");
        }
        std::vector<FT_DEVICE_LIST_INFO_NODE> infos(numDevs);
        ftStatus = FT_GetDeviceInfoList(&infos[0], &numDevs);
        if (ftStatus != FT_OK) {
            qFatal("get device info list error");
        }
        for (int i = 0; i < std::ssize(infos); ++i) {
            std::string describe = infos.at(i).Description;
            if (describe.find("Serial Cable A") != std::string::npos) {
                m_device_list.emplace_back(std::make_tuple(i, std::make_shared<FT_DEVICE_LIST_INFO_NODE>(infos.at(i)), infos.at(i).ftHandle != nullptr, std::make_shared<UDA_CFG>()));
            }
        }

        // 检查序列号是否重复
        std::vector<std::vector<uint8_t>> _serial(m_device_list.size());
        std::transform(m_device_list.begin(), m_device_list.end(), _serial.begin(), [](_T_DL it) -> std::vector<uint8_t> {
            auto                 ptr = std::get<ID_DEVICE_NODE>(it);
            std::vector<uint8_t> ret(16);
            memcpy(ret.data(), ptr->SerialNumber, 16);
            return ret;
        });
        std::sort(_serial.begin(), _serial.end());
        auto uniq = std::unique(_serial.begin(), _serial.end());
        if (uniq != _serial.end()) {
            qFatal("device serial number duplicated");
        }

        // 设置通道数量
        m_channel_number = 8 * std::ssize(m_device_list);

        initParam();

        register_read_interface(std::bind(&_8CH_USB::readAllFrame, this));
    }

    _8CH_USB::~_8CH_USB() {
        closeReadThreadAndWaitExit();
        _8CH_USB::close();
    }

    bool _8CH_USB::open() {
        using namespace std::chrono_literals;
        do {
            auto i = 0;
            for (i = 0; i < std::ssize(m_device_list); i++) {
                auto &it = m_device_list.at(i);
                if (std::get<ID_DEVICE_IS_OPEN>(it) != true) {
                    auto &handle = (std::get<ID_DEVICE_NODE>(it)->ftHandle);
                    if (FT_Open(std::get<ID_DEVICE_INDEX>(it), &handle) != FT_OK) {
                        break;
                    }
                    // reset
                    if (FT_SetBitMode(handle, 0xFF, 0x00) != FT_OK) {
                        break;
                    }
                    std::this_thread::sleep_for(10ms);
                    // sysc fifo mode
                    if (FT_SetBitMode(handle, 0xFF, 0x40) != FT_OK) {
                        break;
                    }
                    std::this_thread::sleep_for(10ms);
                    if (FT_SetLatencyTimer(handle, 16) != FT_OK) {
                        break;
                    }
                    std::this_thread::sleep_for(10ms);
                    if (FT_SetUSBParameters(handle, 4096 * 8, 512) != FT_OK) {
                        break;
                    }
                    std::this_thread::sleep_for(10ms);
                    if (FT_SetTimeouts(handle, 1000, 200) != FT_OK) {
                        break;
                    }
                    std::this_thread::sleep_for(10ms);
                    if (FT_SetFlowControl(handle, FT_FLOW_RTS_CTS, 0, 0) != FT_OK) {
                        break;
                    }
                    std::this_thread::sleep_for(10ms);
                    if (FT_Purge(handle, FT_PURGE_RX | FT_PURGE_TX) != FT_OK) {
                        break;
                    }
                    std::this_thread::sleep_for(10ms);
                    std::get<ID_DEVICE_IS_OPEN>(it) = true;
                }
            }
            if (i < std::ssize(m_device_list)) {
                break;
            }
            return true;
        } while (0);
        // 如果其中有一个模块打开失败, 则将所有已打开的模块关闭
        close();
        return false;
    }

    bool _8CH_USB::isOpen() {
        for (const auto &it : m_device_list) {
            if (std::get<ID_DEVICE_IS_OPEN>(it) == false) {
                return false;
            }
        }
        return true;
    }

    bool _8CH_USB::close() {
        closeReadThreadAndWaitExit();
        for (auto &it : m_device_list) {
            if (std::get<ID_DEVICE_IS_OPEN>(it) == true) {
                if (FT_Close(std::get<ID_DEVICE_NODE>(it)->ftHandle) != FT_OK) {
                    qWarning(TAG) << "close device:" << std::get<ID_DEVICE_INDEX>(it) << "error!";
                }
                std::get<ID_DEVICE_IS_OPEN>(it) = false;
            }
        }
        return true;
    }

    bool _8CH_USB::isDeviceExist() {
        return m_device_list.size() > 0;
    }

    int _8CH_USB::getChannelNumber() const {
        return m_channel_number;
    }

    int _8CH_USB::getGateNumber() const {
        return m_gate_number;
    }

    void _8CH_USB::loadDefaultConfig() {
    }

    bool _8CH_USB::setFrequency(int freq) {
        if (freq < 50) {
            freq = 50;
        }
        if (freq > 10000) {
            freq = 10000;
        }
        m_frequency = freq;
        return true;
    }

    bool _8CH_USB::setVoltage(int volt) {
        m_voltage = volt;
        return true;
    }

    const QVector<QString> &_8CH_USB::getVoltageTable() const {
        const static QVector<QString> ret = {
            QObject::tr("50V"),
            QObject::tr("100V"),
            QObject::tr("200V"),
            QObject::tr("400V"),
        };
        return ret;
    }

    bool _8CH_USB::setChannelFlag(uint32_t flag) {
        Q_UNUSED(flag);
        return false;
    }

    bool _8CH_USB::setDamperFlag(int flag) {
        m_damper_flag = flag;
        return true;
    }

    bool _8CH_USB::setSoundVelocity(int ch, double velocity) {
        if (velocity < 1000 || velocity > 8000) {
#if defined(QT_DEBUG)
            qFatal("velocity must be between 1000 and 8000");
#else
            return false;
#endif
        }
        m_velocity[ch % getChannelNumber()] = velocity;
        return true;
    }

    bool _8CH_USB::setZeroBias(int ch, double bias_us) {
        auto _ch                              = ch % getChannelNumber();
        m_zero_bias[_ch % getChannelNumber()] = bias_us;
        return true;
    }

    bool _8CH_USB::setPulseWidth(int ch, double width_ns) {
        if (width_ns < 30) {
            width_ns = 30;
        }
        auto _ch           = ch % getChannelNumber();
        m_pulse_width[_ch] = width_ns;
        return true;
    }

    bool _8CH_USB::setDelay(int ch, double delay_us) {
        auto _ch                          = ch % getChannelNumber();
        m_delay[_ch % getChannelNumber()] = delay_us;
        return true;
    }

    bool _8CH_USB::setSampleDepth(int ch, double depth) {
        auto _ch            = ch % getChannelNumber();
        m_sample_depth[_ch] = depth;
        return true;
    }

    bool _8CH_USB::setSampleFactor(int ch, int factor) {
        if (factor < 1) {
            factor = 1;
        } else if (factor > 127) {
            factor = 127;
        }
        auto _ch             = ch % getChannelNumber();
        m_sample_factor[_ch] = factor;
        return true;
    }

    bool _8CH_USB::setGain(int ch, double gain) {
        auto _ch    = ch % getChannelNumber();
        m_gain[_ch] = gain;
        return true;
    }

    bool _8CH_USB::setFilter(int ch, int filter_index) {
        auto _ch      = ch % getChannelNumber();
        m_filter[_ch] = filter_index;
        return true;
    }

    const QVector<QString> &_8CH_USB::getFilterTable() const {
        const static QVector<QString> ret = {
            QObject::tr("宽频"),
            QObject::tr("频带一"),
            QObject::tr("频带二"),
            QObject::tr("频带三"),
            QObject::tr("频带四"),
            QObject::tr("频带五"),
            QObject::tr("频带六"),
            QObject::tr("频带七"),
            QObject::tr("频带八"),
            QObject::tr("频带九"),
        };
        return ret;
    }

    bool _8CH_USB::setDemodu(int ch, int demodu_index) {
        auto _ch      = ch % getChannelNumber();
        m_demodu[_ch] = demodu_index;
        return true;
    }

    const QVector<QString> &_8CH_USB::getDemoduTable() const {
        const static QVector<QString> ret = {
            QObject::tr("射频"),
            QObject::tr("全波"),
            QObject::tr("正半波"),
            QObject::tr("负半波"),
        };
        return ret;
    }

    bool _8CH_USB::setPhaseReverse(int ch, bool enable) {
        Q_UNUSED(ch)
        Q_UNUSED(enable)
        return false;
    }

    bool _8CH_USB::sync2Board(void) const {
        for (int _int_part = 0; _int_part < getChannelNumber() / 8; ++_int_part) {
            auto &_param_ptr = std::get<ID_DEVCIE_CONFIG>(m_device_list.at(_int_part));
            if (_param_ptr == nullptr) {
                qFatal("config pointer is nullptr!");
            }
            // 系统参数:
            std::lock_guard lock(m_param_mutex);

            auto &_sys_param       = _param_ptr->sysParam;
            _sys_param.Scan_Unit   = 0;
            _sys_param.Encoder_Rst = 2;
            if (m_frequency >= 100) {
                _sys_param.RepeatFreq = (100000000 / m_frequency) - 1;
            } else {
                _sys_param.RepeatFreq = 1000000 - 1;
            }
            _sys_param.HV_Sel    = m_voltage; // 电压
            _sys_param.Chl_Cycle = 8;         // 通道循环总数
            _sys_param.Digit_IO  = 0;

            for (auto _rem_part = 0; _rem_part < 8; ++_rem_part) {
                int   _ch             = _int_part * 8 + _rem_part;
                auto &_ch_param       = _param_ptr->chParam.at(_rem_part);
                _ch_param.Tx_Chl_No   = _rem_part;
                _ch_param.Pulse_Width = m_pulse_width.at(_ch);
                _ch_param.Rx_Chl_No   = _rem_part;

                uint16_t _da_gain = 0;
                uint8_t  _fixgain = 0;

                double _gain = m_gain.at(_ch);
                if (_gain < 0) {
                    _gain = 0.0;
                } else if (_gain > 110.0) {
                    _gain = 110.0;
                }

                int    _offset = static_cast<int>(_gain * 10.0 + 0.5) / 10; // 整数部分
                double _odd    = static_cast<int>(_gain * 10.0 + 0.5) % 10; // 余数部分
                if (_gain < g_gainTable.point1) {
                    _fixgain = 0;
                } else if (_gain < g_gainTable.point2) {
                    _offset -= g_gainTable.point1;
                    _fixgain = 1;
                } else {
                    _offset -= g_gainTable.point2;
                    _fixgain = 3;
                }

                _odd *= (g_gainTable.val.at(_offset + 1) - g_gainTable.val.at(_offset)) / 10.0;
                _da_gain = static_cast<uint16_t>((g_gainTable.val.at(_offset) + _odd) * 10.0);

                _ch_param.DA_Gain = _da_gain;
                _ch_param.Fixgain = _fixgain;

                _ch_param.DemoduSel   = static_cast<uint8_t>((m_demodu.at(_ch) & 0x0F) | ((m_filter.at(_ch) & 0x0F) << 4));
                _ch_param.Echo_Reject = 0;
                _ch_param.SampLen     = us2mm(m_sample_depth.at(_ch), m_velocity.at(_ch)) * 100.0;
                _ch_param.SampDelay   = m_zero_bias.at(_ch) * 100.0 + m_delay.at(_ch) * 100.0;
            }
        }

        for (const auto &it : m_device_list) {
            const auto &node   = std::get<ID_DEVICE_NODE>(it);
            const auto &cfg    = std::get<ID_DEVCIE_CONFIG>(it);
            bool        isOpen = std::get<ID_DEVICE_IS_OPEN>(it);
            auto        config = *cfg;
            Q_UNUSED(config);
            if (isOpen) {
                DWORD           bytes_write = 0;
                std::lock_guard lock(m_usb_mutex);
                if (FT_Write(node->ftHandle, cfg.get(), STRUCT_CONFIG_SIZE, &bytes_write) != FT_OK || bytes_write != STRUCT_CONFIG_SIZE) {
                    qFatal("write config error");
                }
            }
        }

        return true;
    }

    std::vector<std::shared_ptr<ScanData>> _8CH_USB::readAllFrame(void) {
        std::vector<std::shared_ptr<ScanData>> ret = {};
        for (int i = 0; i < std::ssize(m_device_list); ++i) {
            const auto &it     = m_device_list.at(i);
            const auto &node   = std::get<ID_DEVICE_NODE>(it);
            bool        isOpen = std::get<ID_DEVICE_IS_OPEN>(it);
            if (isOpen) {
                DWORD                      byte_read = 0;
                std::vector<UDA_CHAN_DATA> _all_ch_data(8);
                {
                    std::lock_guard lock(m_usb_mutex);
                    if (FT_Read(node->ftHandle, _all_ch_data.data(), STRUCT_CH_DATA_SIZE * 8, &byte_read) != FT_OK || byte_read != STRUCT_CH_DATA_SIZE * 8) {
                        qFatal("read channel data error");
                    }
                }
                for (int j = 0; j < std::ssize(_all_ch_data); ++j) {
                    int  _ch                = i * 8 + _all_ch_data.at(j).Chan_No;
                    auto _ch_data           = std::make_shared<ScanData>();
                    _ch_data->package_index = -1;
                    _ch_data->channel       = _ch;
                    _ch_data->xAxis_start   = us2mm(getDelay(_ch), getSoundVelocity(_ch));
                    _ch_data->xAxis_range   = us2mm(getSampleDepth(_ch), getSoundVelocity(_ch));
                    _ch_data->ascan         = std::vector<uint8_t>(_all_ch_data.at(j).AcanData.begin(), _all_ch_data.at(j).AcanData.end());
                    {
                        std::lock_guard lock(m_param_mutex);
                        _ch_data->gate = m_gate_info[_ch_data->channel % getChannelNumber()];
                    }
                    _ch_data->gate_result.resize(getGateNumber());
                    const auto &gate = _ch_data->gate;
                    std::transform(gate.begin(), gate.end(), _ch_data->gate_result.begin(), [=](const std::optional<Union::Base::Gate> &_gate) -> Union::Base::GateResult {
                        if (!_gate.has_value()) {
                            return std::nullopt;
                        }
                        return Union::Base::CalculateGateResult(_ch_data->ascan, _gate.value(), true, std::nullopt, 255);
                    });
                    ret.emplace_back(_ch_data);
                }
            }
        }

        std::sort(ret.begin(), ret.end(), [](const std::shared_ptr<ScanData> &a, const std::shared_ptr<ScanData> &b) {
            return a->channel < b->channel;
        });

        return ret;
    }

} // namespace Union::Bridge::MultiChannelHardwareBridge
