#include "Emulator.hpp"

namespace Union::Bridge::MultiChannelHardwareBridge {
    Emulator::Emulator(int channel_number, int gate_number) :
    m_channel_number(channel_number),
    m_gate_number(gate_number) {
        initParam();
        register_read_interface(std::bind(&Emulator::readOneFrame, this));
    }

    Emulator::~Emulator() {
        closeReadThreadAndWaitExit();
        Emulator::close();
    }

    bool Emulator::open() {
        return true;
    }

    bool Emulator::isOpen() {
        return true;
    }

    bool Emulator::close() {
        closeReadThreadAndWaitExit();
        return true;
    }

    bool Emulator::isDeviceExist() {
        return true;
    }

    int Emulator::getChannelNumber() const {
        return m_channel_number;
    }

    int Emulator::getGateNumber() const {
        return m_gate_number;
    }

    void Emulator::loadDefaultConfig() {
    }

    bool Emulator::setFrequency(int freq) {
        m_frequency = freq;
        return true;
    }

    bool Emulator::setVoltage(int volt) {
        m_voltage = volt;
        return true;
    }

    const QVector<QString> &Emulator::getVoltageTable() const {
        const static QVector<QString> ret = {
            QObject::tr("50V"),
            QObject::tr("100V"),
            QObject::tr("200V"),
            QObject::tr("260V"),
        };
        return ret;
    }

    bool Emulator::setChannelFlag(uint32_t flag) {
        m_channel_flag = flag;
        return true;
    }

    bool Emulator::setDamperFlag(int flag) {
        m_damper_flag = flag;
        return true;
    }

    bool Emulator::setSoundVelocity(int ch, double velocity) {
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

    bool Emulator::setZeroBias(int ch, double bias_us) {
        auto _ch                              = ch % getChannelNumber();
        m_zero_bias[_ch % getChannelNumber()] = bias_us;
        return true;
    }

    bool Emulator::setPulseWidth(int ch, double width_ns) {
        if (width_ns < 30) {
            width_ns = 30;
        }
        auto _ch           = ch % getChannelNumber();
        m_pulse_width[_ch] = width_ns;
        return true;
    }

    bool Emulator::setDelay(int ch, double delay_us) {
        auto _ch                          = ch % getChannelNumber();
        m_delay[_ch % getChannelNumber()] = delay_us;
        return true;
    }

    bool Emulator::setSampleDepth(int ch, double depth) {
        auto _ch            = ch % getChannelNumber();
        m_sample_depth[_ch] = depth;
        return true;
    }

    bool Emulator::setSampleFactor(int ch, int factor) {
        if (factor < 1) {
            factor = 1;
        } else if (factor > 127) {
            factor = 127;
        }
        auto _ch             = ch % getChannelNumber();
        m_sample_factor[_ch] = factor;
        return true;
    }

    bool Emulator::setGain(int ch, double gain) {
        auto _ch    = ch % getChannelNumber();
        m_gain[_ch] = gain;
        return true;
    }

    bool Emulator::setFilter(int ch, int filter_index) {
        auto _ch      = ch % getChannelNumber();
        m_filter[_ch] = filter_index;
        return true;
    }

    const QVector<QString> &Emulator::getFilterTable() const {
        const static QVector<QString> ret = {
            QObject::tr("不滤波"),
            QObject::tr("2.5MHz"),
            QObject::tr("5MHz"),
        };
        return ret;
    }

    bool Emulator::setDemodu(int ch, int demodu_index) {
        auto _ch      = ch % getChannelNumber();
        m_demodu[_ch] = demodu_index;
        return true;
    }

    const QVector<QString> &Emulator::getDemoduTable() const {
        const static QVector<QString> ret = {
            QObject::tr("射频"),
            QObject::tr("全波"),
            QObject::tr("正版波"),
            QObject::tr("负半波"),
        };
        return ret;
    }

    bool Emulator::setPhaseReverse(int ch, bool enable) {
        auto _ch             = ch % getChannelNumber();
        m_phase_reverse[_ch] = enable;
        return true;
    }

    bool Emulator::sync2Board(void) const {
        return true;
    }

    std::shared_ptr<ScanData> Emulator::readOneFrame(void) {
        static uint32_t g_i = 0;
        auto            ret = std::make_shared<ScanData>();
        ret->channel        = g_i++ % getChannelNumber();
        ret->package_index  = 0;
        ret->xAxis_start    = 50;
        ret->xAxis_range    = 100;
        ret->ascan.resize(1000);
        {
            std::lock_guard lock(m_param_mutex);
            ret->gate = m_gate_info[ret->channel];
        }

        int idx = 0;
        std::generate(ret->ascan.begin(), ret->ascan.end(), [&]() -> uint8_t {
            idx++;
            auto _ret = 0;
            if (idx > (200 + 5 * ret->channel) && idx < (202 + 5 * ret->channel)) {
                _ret = std::rand() % 5 + 50;
            } else {
                _ret = std::rand() % 10;
            }
            _ret = Union::CalculateGainOutput(_ret, getGain(ret->channel));
            if (_ret > 255) {
                return 255;
            }
            if (_ret < 0) {
                return 0;
            }
            return static_cast<uint8_t>(_ret);
        });
        ret->gate_result.resize(ret->gate.size());
        std::transform(ret->gate.begin(), ret->gate.end(), ret->gate_result.begin(), [=](const std::optional<Union::Base::Gate> &_gate) -> Union::Base::GateResult {
            if (!_gate.has_value()) {
                return std::nullopt;
            }
            return Union::Base::CalculateGateResult(ret->ascan, _gate.value(), true, std::nullopt, 255);
        });
        return ret;
    }

} // namespace Union::Bridge::MultiChannelHardwareBridge
