#include "HDBridge.hpp"
#include <QLoggingCategory>
#include <fstream>
#include <future>
#if !defined(QT_DEBUG)
Q_LOGGING_CATEGORY(TAG, "Union.HDBridge");
#endif

namespace Union::Bridge::MultiChannelHardwareBridge {
    HDBridgeIntf::HDBridgeIntf() {}

    HDBridgeIntf::~HDBridgeIntf() {
        closeReadThreadAndWaitExit();
    }

    void HDBridgeIntf::paramCopy(int src, std::vector<int> dist, int max_gate_number) {
        std::lock_guard lock(m_param_mutex);

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

    bool HDBridgeIntf::serializeScanData(const std::wstring &file_name) const {
        _T_DataV copy = {};
        {
            std::lock_guard lock(m_scan_data_mutex);
            copy = m_scan_data;
        }

        auto ret = std::async(std::launch::deferred, [=]() -> bool {
            try {
                std::ofstream file(file_name, std::ios::out | std::ios::binary);
                file << copy.size();
                for (auto it : copy) {
                    file << it->package_index;
                    file << it->channel;
                    file << it->ascan.size();
                    file.write(reinterpret_cast<const char *>(it->ascan.data()), it->ascan.size());
                    file << it->gate.size();
                    for (auto g : it->gate) {
                        if (g.has_value()) {
                            file << 1;
                            file << g->pos;
                            file << g->width;
                            file << g->height;
                            file << g->enable;
                        } else {
                            file << 0;
                        }
                    }
                }
                return true;
            } catch (std::exception &e) {
#if defined(QT_DEBUG)
                qFatal(e.what());
#else
            qCritical(TAG) << e.what();

#endif
                return false;
            }
        });
        return ret.get();
    }

    bool HDBridgeIntf::deserializeScanData(const std::wstring &file_name) {
        try {
            std::ifstream   file(file_name, std::ios::in | std::ios::binary);
            std::lock_guard lock(m_scan_data_mutex);
            size_t          scan_data_size;
            file >> scan_data_size;
            m_scan_data.resize(scan_data_size);
            for (auto it = m_scan_data.begin(); it != m_scan_data.end(); it++) {
                ScanData scan_data;
                file >> scan_data.package_index;
                file >> scan_data.channel;
                size_t ascan_size;
                file >> ascan_size;
                scan_data.ascan.resize(ascan_size);
                file.read(reinterpret_cast<char *>(scan_data.ascan.data()), ascan_size);
                size_t gate_size;
                file >> gate_size;
                for (int i = 0; std::cmp_less(i, gate_size); i++) {
                    int gate_has_value;
                    file >> gate_has_value;
                    if (gate_has_value == 1) {
                        Union::Base::Gate gate;
                        file >> gate.pos;
                        file >> gate.width;
                        file >> gate.height;
                        file >> gate.enable;
                        scan_data.gate[i] = gate;
                    } else {
                        scan_data.gate[i] = std::nullopt;
                    }
                }
            }
            return true;
        } catch (std::exception &e) {
#if defined(QT_DEBUG)
            qFatal(e.what());
#else
            qCritical(TAG) << e.what();

#endif
            return false;
        }
    }

    bool HDBridgeIntf::serializeConfigData(const std::wstring &file_name) const {
        try {
            std::lock_guard lock(m_param_mutex);
            std::ofstream   file(file_name, std::ios::out | std::ios::binary);
            file << m_frequency;
            file << m_voltage;
            file << m_channel_flag;
            file << m_damper_flag;
            file << getChannelNumber();
            for (auto i = 0; std::cmp_less(i, getChannelNumber()); i++) {
                file << m_velocity[i];
                file << m_zero_bias[i];
                file << m_pulse_width[i];
                file << m_delay[i];
                file << m_sample_depth[i];
                file << m_sample_factor[i];
                file << m_gain[i];
                file << m_filter[i];
                file << m_demodu[i];
                if (m_phase_reverse[i]) {
                    file << 1;
                } else {
                    file << 0;
                }

                file << m_gate_info.size();
                for (auto j = 0; std::cmp_less(j, m_gate_info.size()); i++) {
                    if (m_gate_info[i][j].has_value()) {
                        file << 1;
                        file << m_gate_info[i][j]->pos;
                        file << m_gate_info[i][j]->width;
                        file << m_gate_info[i][j]->height;
                        file << m_gate_info[i][j]->enable;
                    } else {
                        file << 0;
                    }
                }
            }
            return true;
        } catch (std::exception &e) {
#if defined(QT_DEBUG)
            qFatal(e.what());
#else
            qCritical(TAG) << e.what();

#endif
            return false;
        }
    }

    bool HDBridgeIntf::deserializeConfigData(const std::wstring &file_name) {
        try {
            std::lock_guard lock(m_param_mutex);
            std::ifstream   file(file_name, std::ios::in | std::ios::binary);
            file >> m_frequency;
            file >> m_voltage;
            file >> m_channel_flag;
            file >> m_damper_flag;
            int channel_number;
            file >> channel_number;
            for (auto i = 0; std::cmp_less(i, channel_number); i++) {
                file >> m_velocity[i];
                file >> m_zero_bias[i];
                file >> m_pulse_width[i];
                file >> m_delay[i];
                file >> m_sample_depth[i];
                file >> m_sample_factor[i];
                file >> m_gain[i];
                file >> m_filter[i];
                file >> m_demodu[i];
                int phase_reverse;
                file >> phase_reverse;
                if (phase_reverse == 1) {
                    m_phase_reverse[i] = true;
                } else {
                    m_phase_reverse[i] = false;
                }

                size_t gate_info_number;
                file >> gate_info_number;
                for (auto j = 0; std::cmp_less(j, gate_info_number); i++) {
                    int gate_has_value;
                    file >> gate_has_value;
                    if (gate_has_value) {
                        Union::Base::Gate gate;
                        file >> gate.pos;
                        file >> gate.width;
                        file >> gate.height;
                        file >> gate.enable;
                        m_gate_info[i][j] = gate;
                    } else {
                        m_gate_info[i][j] = std::nullopt;
                    }
                }
            }
            return true;
        } catch (std::exception &e) {
#if defined(QT_DEBUG)
            qFatal(e.what());
#else
            qCritical(TAG) << e.what();

#endif
            return false;
        }
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

    bool HDBridgeIntf::operator==(const HDBridgeIntf &rhs) const {
        if (m_frequency != rhs.m_frequency) {
            return false;
        }
        if (m_voltage != rhs.m_voltage) {
            return false;
        }
        if (m_channel_flag != rhs.m_channel_flag) {
            return false;
        }
        if (m_damper_flag != rhs.m_damper_flag) {
            return false;
        }
        for (auto i = 0; std::cmp_less(i, getChannelNumber()); i++) {
            if (m_velocity[i] != rhs.m_velocity[i]) {
                return false;
            }
            if (m_zero_bias[i] != rhs.m_zero_bias[i]) {
                return false;
            }
            if (m_pulse_width[i] != rhs.m_pulse_width[i]) {
                return false;
            }
            if (m_delay[i] != rhs.m_delay[i]) {
                return false;
            }
            if (m_sample_depth[i] != rhs.m_sample_depth[i]) {
                return false;
            }
            if (m_sample_factor[i] != rhs.m_sample_factor[i]) {
                return false;
            }
            if (m_gain[i] != rhs.m_gain[i]) {
                return false;
            }
            if (m_filter[i] != rhs.m_filter[i]) {
                return false;
            }
            if (m_demodu[i] != rhs.m_demodu[i]) {
                return false;
            }
            if (m_phase_reverse[i] != rhs.m_phase_reverse[i]) {
                return false;
            }
            for (auto j = 0; std::cmp_less(j, getGateNumber()); i++) {
                if (m_gate_info[i][j] != rhs.m_gate_info[i][j]) {
                    return false;
                }
            }
        }
        return true;
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
            auto data = readOneFrame();
            if (data != nullptr) {
                if (data->channel > 0 && data->channel < getChannelNumber()) {
                    std::lock_guard lock(m_scan_data_mutex);
                    m_scan_data[data->channel] = data;
                }
                std::lock_guard lock(m_callback_mutex);
                for (auto &func : m_callback_list) {
                    func(*data, *this);
                }
            }

            std::this_thread::yield();
        }
    }

    void HDBridgeIntf::initParam() {
        std::lock_guard lock(m_param_mutex);
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
        m_scan_data.resize(getChannelNumber());
        m_param_is_init = true;
    }

    void HDBridgeIntf::lock_param(void) {
        m_param_mutex.lock();
    }

    void HDBridgeIntf::unlock_param(void) {
        m_param_mutex.unlock();
    }

    int HDBridgeIntf::getFrequency() const {
        std::lock_guard lock(m_param_mutex);
        return m_frequency;
    }

    int HDBridgeIntf::getVoltage() const {
        std::lock_guard lock(m_param_mutex);
        return m_voltage;
    }

    uint32_t HDBridgeIntf::getChannelFlag() const {
        std::lock_guard lock(m_param_mutex);
        return m_channel_flag;
    }

    int HDBridgeIntf::getDamperFlag() const {
        std::lock_guard lock(m_param_mutex);
        return m_damper_flag;
    }

    double HDBridgeIntf::getSoundVelocity(int ch) const {
        std::lock_guard lock(m_param_mutex);
        return m_velocity[ch % getChannelNumber()];
    }

    double HDBridgeIntf::getZeroBias(int ch) const {
        std::lock_guard lock(m_param_mutex);
        return m_zero_bias[ch % getChannelNumber()];
    }

    double HDBridgeIntf::getPulseWidth(int ch) const {
        std::lock_guard lock(m_param_mutex);
        return m_pulse_width[ch % getChannelNumber()];
    }

    double HDBridgeIntf::getDelay(int ch) const {
        std::lock_guard lock(m_param_mutex);
        return m_delay[ch % getChannelNumber()];
    }

    double HDBridgeIntf::getSampleDepth(int ch) const {
        std::lock_guard lock(m_param_mutex);
        return m_sample_depth[ch % getChannelNumber()];
    }

    int HDBridgeIntf::getSampleFactor(int ch) const {
        std::lock_guard lock(m_param_mutex);
        return m_sample_factor[ch % getChannelNumber()];
    }

    double HDBridgeIntf::getGain(int ch) const {
        std::lock_guard lock(m_param_mutex);
        return m_gain[ch % getChannelNumber()];
    }

    int HDBridgeIntf::getFilter(int ch) const {
        std::lock_guard lock(m_param_mutex);
        return m_filter[ch % getChannelNumber()];
    }

    int HDBridgeIntf::getDemodu(int ch) const {
        std::lock_guard lock(m_param_mutex);
        return m_demodu[ch % getChannelNumber()];
    }

    bool HDBridgeIntf::getPhaseReverse(int ch) const {
        std::lock_guard lock(m_param_mutex);
        return m_phase_reverse[ch % getChannelNumber()];
    }

    bool HDBridgeIntf::setGateInfo(int ch, int idx, const std::optional<Union::Base::Gate> &gate) {
        std::lock_guard lock(m_param_mutex);
        m_gate_info[ch % getChannelNumber()][idx % getGateNumber()] = gate;
        return true;
    }

    const std::optional<Union::Base::Gate> &HDBridgeIntf::getGateInfo(int ch, int idx) const {
        std::lock_guard lock(m_param_mutex);
        return m_gate_info[ch % getChannelNumber()][idx % getGateNumber()];
    }

} // namespace Union::Bridge::MultiChannelHardwareBridge
