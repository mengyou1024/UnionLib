#pragma once

#include "_12CH_USB.hpp"
#include "TOFDPort.h"

using namespace TOFDPort;

namespace Union::Bridge::MultiChannelHardwareBridge {
    _12CH_USB::_12CH_USB(int gate_number, int type) :
    m_type(type),
    m_gate_number(gate_number) {
        initParam();
    }

    _12CH_USB::~_12CH_USB() {}

    bool _12CH_USB::open() {
        return TOFD_PORT_OpenDevice(0);
    }

    bool _12CH_USB::isOpen() {
        return TOFD_PORT_IsOpen();
    }

    bool _12CH_USB::close() {
        return TOFD_PORT_CloseDevice();
    }

    bool _12CH_USB::isDeviceExist() {
        return TOFD_PORT_IsDeviceExist();
    }

    int _12CH_USB::getChannelNumber() const {
        return 12;
    }

    int _12CH_USB::getGateNumber() const {
        return m_gate_number;
    }

    void _12CH_USB::loadDefaultConfig() {
    }

    bool _12CH_USB::setFrequency(int freq) {
        if (freq < 50) {
            freq = 50;
        }
        if (freq > 10000) {
            freq = 10000;
        }
        m_frequency = freq;
        return TOFD_PORT_SetFrequency(freq);
    }

    bool _12CH_USB::setVoltage(int volt) {
        m_voltage = volt;
        return TOFD_PORT_SetVoltage(volt);
    }

    const QVector<QString> &_12CH_USB::getVoltageTable() const {
        const static QVector<QString> ret = {
            QObject::tr("50V"),
            QObject::tr("100V"),
            QObject::tr("200V"),
            QObject::tr("260V"),
        };
        return ret;
    }

    bool _12CH_USB::setChannelFlag(uint32_t flag) {
        if (flag == 0) {
            flag = 0xFFF0FFF;
        }
        m_channel_flag = flag;
        return TOFD_PORT_SetChannelFlag(static_cast<int>(flag));
    }

    bool _12CH_USB::setDamperFlag(int flag) {
        m_damper_flag = flag;
        return TOFD_PORT_SetDamperFlag(flag);
    }

    bool _12CH_USB::setSoundVelocity(int ch, double velocity) {
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

    bool _12CH_USB::setZeroBias(int ch, double bias_us) {
        auto _ch                              = ch % getChannelNumber();
        m_zero_bias[_ch % getChannelNumber()] = bias_us;
        auto val                              = m_zero_bias[_ch] + m_delay[_ch];
        return TOFD_PORT_SetDelay(_ch, val);
    }

    bool _12CH_USB::setPulseWidth(int ch, double width_us) {
        if (width_us < 30) {
            width_us = 30;
        }
        auto _ch           = ch % getChannelNumber();
        m_pulse_width[_ch] = width_us;
        return TOFD_PORT_SetPulseWidth(_ch, width_us);
    }

    bool _12CH_USB::setDelay(int ch, double delay_us) {
        auto _ch                          = ch % getChannelNumber();
        m_delay[_ch % getChannelNumber()] = delay_us;
        auto val                          = m_zero_bias[_ch] + m_delay[_ch];
        return TOFD_PORT_SetDelay(_ch, val);
    }

    bool _12CH_USB::setSampleDepth(int ch, double depth) {
        auto _ch            = ch % getChannelNumber();
        m_sample_depth[_ch] = depth;
        auto val            = m_sample_depth[_ch] + m_zero_bias[_ch];
        return TOFD_PORT_SetSampleDepth(_ch, val);
    }

    bool _12CH_USB::setSampleFactor(int ch, int factor) {
        if (factor < 1) {
            factor = 1;
        } else if (factor > 127) {
            factor = 127;
        }
        auto _ch             = ch % getChannelNumber();
        m_sample_factor[_ch] = factor;
        return TOFD_PORT_SetSampleFactor(_ch, factor);
    }

    bool _12CH_USB::setGain(int ch, double gain) {
        auto _ch    = ch % getChannelNumber();
        m_gain[_ch] = gain;
        return TOFD_PORT_SetGain(_ch, gain);
    }

    bool _12CH_USB::setFilter(int ch, int filter_index) {
        auto _ch      = ch % getChannelNumber();
        m_filter[_ch] = filter_index;
        return TOFD_PORT_SetFilter(_ch, filter_index);
    }

    const QVector<QString> &_12CH_USB::getFilterTable() const {
        const static QVector<QString> ret = {
            QObject::tr("不滤波"),
            QObject::tr("2.5MHz"),
            QObject::tr("5MHz"),
        };
        return ret;
    }

    bool _12CH_USB::setDemodu(int ch, int demodu_index) {
        auto _ch      = ch % getChannelNumber();
        m_demodu[_ch] = demodu_index;
        return TOFD_PORT_SetDemodu(_ch, demodu_index);
    }

    const QVector<QString> &_12CH_USB::getDemoduTable() const {
        const static QVector<QString> ret = {
            QObject::tr("射频"),
            QObject::tr("全波"),
            QObject::tr("正版波"),
            QObject::tr("负半波"),
        };
        return ret;
    }

    bool _12CH_USB::setPhaseReverse(int ch, bool enable) {
        auto _ch             = ch % getChannelNumber();
        m_phase_reverse[_ch] = enable;
        return TOFD_PORT_SetPhaseReverse(_ch, enable);
    }

    bool _12CH_USB::sync2Board(void) const {
        return TOFD_PORT_FlushSetting();
    }

    std::shared_ptr<ScanData> _12CH_USB::readOneFrame(void) {
        auto dat = TOFD_PORT_ReadDatasFormat();
        TOFD_PORT_Free_NM_DATA(dat);
        auto ret           = std::make_shared<ScanData>();
        ret->channel       = dat->iChannel;
        ret->package_index = dat->iPackage;
        ret->ascan.resize(dat->iAScanSize);
        ret->gate = m_gate_info[ret->channel];
        memcpy_s(ret->ascan.data(), ret->ascan.size(), dat->pAscan, dat->iAScanSize);

        ret->gate_result.resize(getGateNumber());
        auto gate = m_gate_info[ret->channel % getChannelNumber()];
        std::transform(gate.begin(), gate.end(), ret->gate_result.begin(), [=](const std::optional<Union::Base::Gate> &_gate) -> Union::Base::GateResult {
            if (!_gate.has_value()) {
                return std::nullopt;
            }
            return Union::Base::CalculateGateResult(ret->ascan, _gate.value(), true, std::nullopt, 255);
        });

        return ret;
    }

} // namespace Union::Bridge::MultiChannelHardwareBridge
