#include "HDBridge.hpp"

namespace Union::Bridge::MultiChannelHardwareBridge {
    HDBridgeIntf::HDBridgeIntf() {}

    HDBridgeIntf::~HDBridgeIntf() {
        closeReadThreadAndWaitExit();
    }

    void HDBridgeIntf::paramCopy(int src, std::vector<int> dist, int max_gate_number) {
        auto freq          = getFrequency();
        auto voltage       = getVoltage();
        auto channelFlag   = getChannelFlag();
        auto damperFlag    = getDamperFlag();
        auto soundVelocity = getSoundVelocity(src);
        auto zeroBias      = getZeroBias(src);
        auto pulseWidth    = getPulseWidth(src);
        auto delay         = getDelay(src);
        auto sampleDepth   = getSampleDepth(src);
        auto sampleFactor  = getSampleFactor(src);
        auto gain          = getGain(src);
        auto filter        = getFilter(src);
        auto demodu        = getDemodu(src);
        auto phaseReverse  = getPhaseReverse(src);

        std::vector<std::optional<Union::Base::Gate>> gate(static_cast<size_t>(max_gate_number));
        for (int i = 0; i < max_gate_number; i++) {
            gate.at(i) = getGateInfo(src, i);
        }

        setFrequency(freq);
        setVoltage(voltage);
        setChannelFlag(channelFlag);
        setDamperFlag(damperFlag);

        for (auto i : dist) {
            setSoundVelocity(i, soundVelocity);
            setZeroBias(i, zeroBias);
            setPulseWidth(i, pulseWidth);
            setDelay(i, delay);
            setSampleDepth(i, sampleDepth);
            setSampleFactor(i, sampleFactor);
            setGain(i, gain);
            setFilter(i, filter);
            setDemodu(i, demodu);
            setPhaseReverse(i, phaseReverse);
            for (int j = 0; j < max_gate_number; j++) {
                setGateInfo(i, j, gate[i]);
            }
        }
        sync2Board();
    }

    bool HDBridgeIntf::appendCallback(InftCallbackFunc func) {
        std::lock_guard lock(m_callback_mutex);
        m_callback_list.emplace_back(func);
        return true;
    }

    bool HDBridgeIntf::clearCallback(void) {
        std::lock_guard lock(m_callback_mutex);
        m_callback_list.clear();
        return true;
    }

    bool HDBridgeIntf::storeCallback(void) {
        std::lock_guard lock(m_callback_mutex);
        m_callback_stack.push(m_callback_list);
        return true;
    }

    bool HDBridgeIntf::restoreCallback(void) {
        std::lock_guard lock(m_callback_mutex);
        if (!m_callback_stack.empty()) {
            m_callback_list = m_callback_stack.top();
            m_callback_stack.pop();
            return true;
        }
        return false;
    }

    void HDBridgeIntf::runReadThread(void) {
        if (m_thread_running != true) {
            m_thread_running = true;
            m_read_thread    = std::thread(&HDBridgeIntf::readThread, this);
        }
    }

    void HDBridgeIntf::closeReadThreadAndWaitExit(void) noexcept {
        if (m_thread_running == true) {
            m_thread_running = false;
            m_read_thread.join();
        }
    }

    void HDBridgeIntf::readThread(void) {
        while (m_thread_running) {
            auto            data = readOneFrame();
            std::lock_guard lock(m_callback_mutex);
            for (auto &func : m_callback_list) {
                func(*data, *this);
            }
            std::this_thread::yield();
        }
    }

    void HDBridgeIntf::initParam() {
        m_velocity.resize(getChannelNumber());
        m_zero_bias.resize(getChannelNumber());
        m_pulse_width.resize(getChannelNumber());
        m_delay.resize(getChannelNumber());
        m_sample_depth.resize(getChannelNumber());
        m_sample_factor.resize(getChannelNumber());
        m_gain.resize(getChannelNumber());
        m_filter.resize(getChannelNumber());
        m_demodu.resize(getChannelNumber());
        m_phase_reverse.resize(getChannelNumber());
        m_gate_info.resize(getChannelNumber());
        for (int i = 0; i < getChannelNumber(); i++) {
            m_gate_info[i].resize(getGateNumber());
        }
        m_param_is_init = true;
    }

    int HDBridgeIntf::getFrequency() const {
        return m_frequency;
    }

    int HDBridgeIntf::getVoltage() const {
        return m_voltage;
    }

    uint32_t HDBridgeIntf::getChannelFlag() const {
        return m_channel_flag;
    }

    int HDBridgeIntf::getDamperFlag() const {
        return m_damper_flag;
    }

    double HDBridgeIntf::getSoundVelocity(int ch) const {
        return m_velocity[ch % getChannelNumber()];
    }

    double HDBridgeIntf::getZeroBias(int ch) const {
        return m_zero_bias[ch % getChannelNumber()];
    }

    double HDBridgeIntf::getPulseWidth(int ch) const {
        return m_pulse_width[ch % getChannelNumber()];
    }

    double HDBridgeIntf::getDelay(int ch) const {
        return m_delay[ch % getChannelNumber()];
    }

    double HDBridgeIntf::getSampleDepth(int ch) const {
        return m_sample_depth[ch % getChannelNumber()];
    }

    int HDBridgeIntf::getSampleFactor(int ch) const {
        return m_sample_factor[ch % getChannelNumber()];
    }

    double HDBridgeIntf::getGain(int ch) const {
        return m_gain[ch % getChannelNumber()];
    }

    int HDBridgeIntf::getFilter(int ch) const {
        return m_filter[ch % getChannelNumber()];
    }

    int HDBridgeIntf::getDemodu(int ch) const {
        return m_demodu[ch % getChannelNumber()];
    }

    bool HDBridgeIntf::getPhaseReverse(int ch) const {
        return m_phase_reverse[ch % getChannelNumber()];
    }

    bool HDBridgeIntf::setGateInfo(int ch, int idx, const std::optional<Union::Base::Gate> &gate) {
        m_gate_info[ch % getChannelNumber()][idx % getGateNumber()] = gate;
        return true;
    }

    const std::optional<Union::Base::Gate> &HDBridgeIntf::getGateInfo(int ch, int idx) const {
        return m_gate_info[ch % getChannelNumber()][idx % getGateNumber()];
    }

} // namespace Union::Bridge::MultiChannelHardwareBridge
