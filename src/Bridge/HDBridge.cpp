#include "HDBridge.hpp"
#include <QCoreApplication>
#include <QDataStream>
#include <QEventLoop>
#include <QFile>
#include <QLoggingCategory>
#include <QThread>
#include <chrono>
#include <future>
#include <numeric>

#if !defined(QT_DEBUG)
Q_LOGGING_CATEGORY(TAG, "Union.HDBridge");
#endif

namespace Union::Bridge::MultiChannelHardwareBridge {
    HDBridgeIntf::HDBridgeIntf() {}

    HDBridgeIntf::~HDBridgeIntf() {}

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

    bool HDBridgeIntf::serializeScanDataAppendToFile(const QString &file_name) const {
        _T_DataV copy = {};
        {
            for (const auto &it : m_scan_data) {
                if (it == nullptr) {
                    return false;
                }
            }
            std::lock_guard lock(m_scan_data_mutex);
            copy = m_scan_data;
        }

        auto ret = std::async(std::launch::deferred, [=]() -> bool {
            try {
                QFile file(file_name);
                file.open(QIODevice::WriteOnly | QIODevice::Append);
                QDataStream file_stream(&file);
                file_stream << copy.size();
                for (auto &it : copy) {
                    file_stream << it->package_index;
                    file_stream << it->channel;
                    file_stream << it->xAxis_start;
                    file_stream << it->xAxis_range;
                    file_stream << it->ascan.size();
                    file_stream.writeRawData(reinterpret_cast<const char *>(it->ascan.data()), std::ssize(it->ascan));
                    file_stream << it->gate.size();
                    for (auto g : it->gate) {
                        if (g.has_value()) {
                            file_stream << 1;
                            file_stream << g->pos;
                            file_stream << g->width;
                            file_stream << g->height;
                            file_stream << g->enable;
                        } else {
                            file_stream << 0;
                        }
                    }
                }
                file.flush();
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

    std::vector<std::vector<std::shared_ptr<ScanData>>> HDBridgeIntf::deserializeScanData(const QString &file_name) const {
        std::vector<_T_DataV> _ret;
        try {
            QFile file(file_name);
            file.open(QIODevice::ReadOnly);
            QDataStream file_stream(&file);
            while (file.pos() < file.size()) {
                auto _temp = unserializeOneFreame(file_stream);
                _ret.emplace_back(std::move(_temp));
            }

        } catch (std::exception &e) {
#if defined(QT_DEBUG)
            qFatal(e.what());
#else
            qCritical(TAG) << e.what();

#endif
        }
        return _ret;
    }

    bool HDBridgeIntf::serializeConfigData(const QString &file_name) const {
        try {
            std::lock_guard lock(m_param_mutex);
            QFile           file(file_name);
            file.open(QIODevice::WriteOnly);
            QDataStream file_stream(&file);
            file_stream << m_frequency;
            file_stream << m_voltage;
            file_stream << m_channel_flag;
            file_stream << m_damper_flag;
            file_stream << getChannelNumber();
            for (auto i = 0; std::cmp_less(i, getChannelNumber()); i++) {
                file_stream << m_velocity[i];
                file_stream << m_zero_bias[i];
                file_stream << m_pulse_width[i];
                file_stream << m_delay[i];
                file_stream << m_sample_depth[i];
                file_stream << m_sample_factor[i];
                file_stream << m_gain[i];
                file_stream << m_filter[i];
                file_stream << m_demodu[i];
                file_stream << m_phase_reverse[i];

                file_stream << m_gate_info.size();
                for (auto j = 0; std::cmp_less(j, m_gate_info.size()); i++) {
                    if (m_gate_info[i][j].has_value()) {
                        file_stream << 1;
                        file_stream << m_gate_info[i][j]->pos;
                        file_stream << m_gate_info[i][j]->width;
                        file_stream << m_gate_info[i][j]->height;
                        file_stream << m_gate_info[i][j]->enable;
                    } else {
                        file_stream << 0;
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

    bool HDBridgeIntf::deserializeConfigData(const QString &file_name) {
        try {
            std::lock_guard lock(m_param_mutex);
            QFile           file(file_name);
            file.open(QIODevice::ReadOnly);
            QDataStream file_stream(&file);
            file_stream >> m_frequency;
            file_stream >> m_voltage;
            file_stream >> m_channel_flag;
            file_stream >> m_damper_flag;
            int channel_number;
            file_stream >> channel_number;
            for (auto i = 0; std::cmp_less(i, channel_number); i++) {
                file_stream >> m_velocity[i];
                file_stream >> m_zero_bias[i];
                file_stream >> m_pulse_width[i];
                file_stream >> m_delay[i];
                file_stream >> m_sample_depth[i];
                file_stream >> m_sample_factor[i];
                file_stream >> m_gain[i];
                file_stream >> m_filter[i];
                file_stream >> m_demodu[i];
                bool phase_reverse;
                file_stream >> phase_reverse;
                m_phase_reverse[i] = phase_reverse;

                size_t gate_info_number;
                file_stream >> gate_info_number;
                for (auto j = 0; std::cmp_less(j, gate_info_number); i++) {
                    int gate_has_value;
                    file_stream >> gate_has_value;
                    if (gate_has_value) {
                        Union::Base::Gate gate;
                        file_stream >> gate.pos;
                        file_stream >> gate.width;
                        file_stream >> gate.height;
                        file_stream >> gate.enable;
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

    void HDBridgeIntf::runHardwareReadThread(void) {
        if (m_thread_type != SUB_THREAD_TYPE::IDLE && m_thread_type != SUB_THREAD_TYPE::HARDWARE) {
            throw std::runtime_error("thread already running, and the thread type is not hardware");
        }
        if (m_thread_running != true) {
            m_thread_type    = SUB_THREAD_TYPE::HARDWARE;
            m_thread_running = true;
            m_read_thread    = std::unique_ptr<QThread>(QThread::create(&HDBridgeIntf::hardwareReadThread, this));
            m_read_thread->start();
        }
    }

    void HDBridgeIntf::runFileReadThread(const QString &file_name, double fps) {
        if (m_thread_type != SUB_THREAD_TYPE::IDLE && m_thread_type != SUB_THREAD_TYPE::FILE_STREAM) {
            throw std::runtime_error("thread already running, and the thread type is not file stream");
        }
        if (m_thread_running != true) {
            m_thread_type    = SUB_THREAD_TYPE::FILE_STREAM;
            m_thread_running = true;
            m_read_thread    = std::unique_ptr<QThread>(QThread::create(&HDBridgeIntf::fileReadThread, this, file_name, fps));
            m_read_thread->start();
        }
    }

    void HDBridgeIntf::closeReadThreadAndWaitExit(void) noexcept {
        if (m_thread_running == true) {
            m_thread_type    = SUB_THREAD_TYPE::IDLE;
            m_thread_running = false;
            m_read_thread->wait();
            m_read_thread = nullptr;
        }
    }

    void HDBridgeIntf::hardwareReadThread(void) {
        auto              _last_invoke_time = std::chrono::system_clock::now();
        std::vector<bool> _ch_update_flag; // 用于标记通道是否更新
        _ch_update_flag.resize(getChannelNumber());
        std::generate(_ch_update_flag.begin(), _ch_update_flag.end(), []() -> bool {
            return false;
        });
        _T_DataV _mirror_scan_data = {}; // 扫查数据镜像
        _mirror_scan_data.resize(getChannelNumber());
        std::generate(_mirror_scan_data.begin(), _mirror_scan_data.end(), []() -> std::shared_ptr<ScanData> {
            return nullptr;
        });

        while (m_thread_running) {
            auto data = readOneFrame();
            if (data != nullptr && data->channel >= 0 && data->channel < getChannelNumber()) {
                _mirror_scan_data.at(data->channel) = data;
                _ch_update_flag.at(data->channel)   = true;
                if (std::accumulate(_ch_update_flag.begin(), _ch_update_flag.end(), 0) != getChannelNumber()) {
                    // 如果有通道没有更新完成，则继续读取
                    continue;
                }
                invokeCallback(_mirror_scan_data); // 执行回调函数, 默认异步执行
                {
                    std::lock_guard lock(m_scan_data_mutex);
                    m_scan_data = _mirror_scan_data; // 更新镜像数据(作用域是为了减少lock的持有时间)
                }
                std::generate(_ch_update_flag.begin(), _ch_update_flag.end(), []() -> bool {
                    return false;
                }); // 重新清空标志位
                auto _current_invoke_time = std::chrono::system_clock::now();
                using namespace std::chrono_literals;
                // 限制最大帧率为100帧
                if (_current_invoke_time - _last_invoke_time > 10ms) {
                    std::this_thread::sleep_until(_last_invoke_time + 10ms);
                }
                _last_invoke_time = _current_invoke_time;
            }
        }
    }

    void HDBridgeIntf::fileReadThread(const QString &file_name, std::optional<double> fps) {
        _T_DataV _mirror_scan_data = {}; // 扫查数据镜像
        auto     _last_invoke_time = std::chrono::system_clock::now();
        QFile    file(file_name);
        file.open(QIODevice::ReadOnly);
        QDataStream file_stream(&file);
        while (file.pos() < file.size() && m_thread_running) {
            try {
                _mirror_scan_data = unserializeOneFreame(file_stream);
            } catch (std::exception &e) {
#if defined(QT_DEBUG)
                qFatal(e.what());
#else
                qCritical(TAG) << e.what();
#endif
            }

            invokeCallback(_mirror_scan_data); // 执行回调函数, 默认异步执行
            using namespace std::chrono_literals;
            auto sleep_time = 10000us / 1.0;
            if (fps.has_value()) {
                sleep_time = 1000000us / fps.value();
            }
            std::this_thread::sleep_until(_last_invoke_time + sleep_time);
            _last_invoke_time = std::chrono::system_clock::now();
        }
        if (file.pos() > file.size()) {
#if defined(QT_DEBUG)
            qFatal("File read overflow");
#else
            qCritical(TAG) << "File read overflow";
#endif
        }
        m_thread_running = false;
        m_thread_type    = SUB_THREAD_TYPE::IDLE;
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
            m_velocity[i] = 5920.0;
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

    void HDBridgeIntf::invokeCallback(IntfInvokeParam_1 data, std::launch launtch_type) {
        try {
            std::list<InftCallbackFunc> mirror = {};
            {
                std::lock_guard lock(m_callback_mutex);
                mirror = m_callback_list;
            }
            std::vector<std::future<void>> futures = {};
            for (auto &func : mirror) {
                futures.emplace_back(std::async(launtch_type, func, data, std::ref(*this)));
            }
            for (auto &f : futures) {
                f.get();
            }
        } catch (std::exception &e) {
#if defined(QT_DEBUG)
            qFatal(e.what());
#else
            qCritical(TAG) << e.what();
#endif
        }
    }

    HDBridgeIntf::_T_DataV HDBridgeIntf::unserializeOneFreame(QDataStream &file) const {
        _T_DataV _temp;
        size_t   scan_data_size;
        file >> scan_data_size;
        _temp.resize(scan_data_size);
        for (auto it = _temp.begin(); it != _temp.end(); it++) {
            auto scan_data = std::make_shared<ScanData>();
            file >> scan_data->package_index;
            file >> scan_data->channel;
            file >> scan_data->xAxis_start;
            file >> scan_data->xAxis_range;
            size_t ascan_size;
            file >> ascan_size;
            scan_data->ascan.resize(ascan_size);
            file.readRawData(reinterpret_cast<char *>(scan_data->ascan.data()), static_cast<int>(ascan_size));
            size_t gate_size;
            file >> gate_size;
            scan_data->gate.resize(gate_size);
            for (int i = 0; std::cmp_less(i, gate_size); i++) {
                int gate_has_value;
                file >> gate_has_value;
                if (gate_has_value == 1) {
                    Union::Base::Gate gate;
                    file >> gate.pos;
                    file >> gate.width;
                    file >> gate.height;
                    file >> gate.enable;
                    scan_data->gate[i] = gate;
                } else {
                    scan_data->gate[i] = std::nullopt;
                }
            }
            *it = std::move(scan_data);
        }
        return _temp;
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
