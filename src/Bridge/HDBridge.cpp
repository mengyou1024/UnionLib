#include "HDBridge.hpp"
#include "HDBridge.pb.h"
#include <QCoreApplication>
#include <QDataStream>
#include <QEventLoop>
#include <QFile>
#include <QLoggingCategory>
#include <QThread>
#include <array>
#include <chrono>
#include <fstream>
#include <future>
#include <numeric>

Q_LOGGING_CATEGORY(TAG, "Union.HDBridge");

namespace Union::Bridge::MultiChannelHardwareBridge {
    HDBridgeIntf::HDBridgeIntf() {}

    HDBridgeIntf::~HDBridgeIntf() {
        google::protobuf::ShutdownProtobufLibrary();
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

    bool HDBridgeIntf::serializeScanDataAppendToFile(const QString &file_name) const {
        _T_DataV _copy = {};
        {
            std::lock_guard lock(m_scan_data_mutex);
            for (const auto &it : m_scan_data) {
                if (it == nullptr) {
                    qCWarning(TAG) << "channel parameter is nullptr!";
                    return false;
                }
                if (it->ascan.size() == 0) {
                    qCWarning(TAG) << "channel: " << it->channel << "'s data size is 0!";
                    return false;
                }
            }
            _copy = m_scan_data;
        }

        auto ret = std::async(std::launch::deferred, [&]() -> bool {
            try {
                std::ofstream file(file_name.toStdWString().c_str(), std::ios::binary | std::ios::app);
                if (!file.is_open()) {
                    qCCritical(TAG) << "failed to open file:" << file_name;
                    return false;
                }
                Proto::Union::HDBridge::ScanDataVector scan_data_vector;
                auto                                   _scan_data = scan_data_vector.add_scan_data();
                for (int i = 0; i < std::min(getChannelNumber(), static_cast<int>(_copy.size())); i++) {
                    auto _channel_scan_data = _scan_data->add_channel_data();
                    auto _ch_to_write       = _copy[i];
                    if (_ch_to_write == nullptr) {
                        continue;
                    }
                    _channel_scan_data->set_pakcage_index(_ch_to_write->package_index);
                    if (i != _ch_to_write->channel) {
                        qCWarning(TAG).noquote() << QString::asprintf("current index(%d) != channel(%d)", i, _ch_to_write->channel);
                    }
                    _channel_scan_data->set_channel(_ch_to_write->channel);
                    _channel_scan_data->set_axis_offset(_ch_to_write->xAxis_start);
                    _channel_scan_data->set_axis_length(_ch_to_write->xAxis_range);
                    _channel_scan_data->set_ascan_data(_ch_to_write->ascan.data(), _ch_to_write->ascan.size());

                    for (int j = 0; j < std::ssize(_ch_to_write->gate); j++) {
                        if (_ch_to_write->gate[j].has_value() && (_ch_to_write->gate[j]->enable == true)) {
                            auto _gate = _channel_scan_data->add_gates();
                            _gate->set_pos(_ch_to_write->gate[j]->pos);
                            _gate->set_width(_ch_to_write->gate[j]->width);
                            _gate->set_height(_ch_to_write->gate[j]->height);
                            _gate->set_index(j);
                        }
                    }
                }
                auto ret = scan_data_vector.SerializeToOstream(&file);
                file.flush();
                return ret;
            } catch (std::exception &e) {
                qCCritical(TAG) << e.what();
                return false;
            }
        });
        return ret.get();
    }

    std::vector<std::vector<std::shared_ptr<ScanData>>> HDBridgeIntf::deserializeScanData(const QString &file_name) const {
        std::vector<_T_DataV> _ret;
        try {
            std::ifstream file(file_name.toStdWString().c_str(), std::ios::binary);
            if (!file.is_open()) {
                qCCritical(TAG) << "failed to open file:" << file_name;
                return _ret;
            }
            Proto::Union::HDBridge::ScanDataVector scan_data_vector;
            if (scan_data_vector.ParseFromIstream(&file) != true) {
                qCritical(TAG) << "failed to parse file:" << file_name;
                return _ret;
            }
            qCDebug(TAG) << "parse data length:" << scan_data_vector.scan_data_size();

            for (int sd_sz = 0; sd_sz < scan_data_vector.scan_data_size(); sd_sz++) {
                auto                                   ch_data       = scan_data_vector.scan_data(sd_sz);
                std::vector<std::shared_ptr<ScanData>> _sc_to_insert = {};
                for (int _ch = 0; _ch < std::min(ch_data.channel_data_size(), getChannelNumber()); _ch++) {
                    auto _ch_data_to_insert           = std::make_shared<ScanData>();
                    _ch_data_to_insert->package_index = ch_data.channel_data(_ch).pakcage_index();
                    _ch_data_to_insert->channel       = ch_data.channel_data(_ch).channel();
                    _ch_data_to_insert->xAxis_start   = ch_data.channel_data(_ch).axis_offset();
                    _ch_data_to_insert->xAxis_range   = ch_data.channel_data(_ch).axis_length();
                    _ch_data_to_insert->ascan.resize(ch_data.channel_data(_ch).ascan_data().size());
                    std::memcpy(_ch_data_to_insert->ascan.data(), ch_data.channel_data(_ch).ascan_data().data(), ch_data.channel_data(_ch).ascan_data().size());
                    _ch_data_to_insert->gate.resize(getGateNumber());
                    std::generate(_ch_data_to_insert->gate.begin(), _ch_data_to_insert->gate.end(), []() { return std::nullopt; });
                    for (int _gate_sz = 0; _gate_sz < std::min(ch_data.channel_data(_ch).gates_size(), getGateNumber()); _gate_sz++) {
                        auto _gate                              = ch_data.channel_data(_ch).gates(_gate_sz);
                        _ch_data_to_insert->gate[_gate.index()] = std::make_optional<Union::Base::Gate>(_gate.pos(), _gate.width(), _gate.height(), true);
                    }
                    _ch_data_to_insert->gate_result.resize(_ch_data_to_insert->gate.size());
                    std::transform(_ch_data_to_insert->gate.begin(), _ch_data_to_insert->gate.end(), _ch_data_to_insert->gate_result.begin(), [=](const std::optional<Union::Base::Gate> &_gate) -> Union::Base::GateResult {
                        if (!_gate.has_value()) {
                            return std::nullopt;
                        }
                        return Union::Base::CalculateGateResult(_ch_data_to_insert->ascan, _gate.value(), true, std::nullopt, 255);
                    });
                    _sc_to_insert.emplace_back(_ch_data_to_insert);
                }
                _ret.emplace_back(_sc_to_insert);
            }

        } catch (std::exception &e) {
            qCCritical(TAG) << e.what();
        }
        return _ret;
    }

    bool HDBridgeIntf::serializeConfigData(const QString &file_name) const {
        try {
            Proto::Union::HDBridge::ConfigData config_data;
            {
                std::lock_guard lock(m_param_mutex);
                config_data.set_repeat_freqency(m_frequency);
                config_data.set_emit_voltage(m_voltage);
                config_data.set_channel_flag(m_channel_flag);
                config_data.set_damper_flag(m_damper_flag);

                auto channel_params = config_data.mutable_channel_parameters();
                channel_params->Reserve(getChannelNumber());
                for (auto i = 0; std::cmp_less(i, getChannelNumber()); i++) {
                    Proto::Union::HDBridge::ChannelParameter channel_param;
                    channel_param.set_sound_velocity(m_velocity[i]);
                    channel_param.set_zero_bias(m_zero_bias[i]);
                    channel_param.set_pulse_width(m_pulse_width[i]);
                    channel_param.set_sampling_delay(m_delay[i]);
                    channel_param.set_sampling_depth(m_sample_depth[i]);
                    channel_param.set_sampling_factor(m_sample_factor[i]);
                    channel_param.set_gain(m_gain[i]);
                    channel_param.set_filter_band(m_filter[i]);
                    channel_param.set_demodu_mode(m_demodu[i]);
                    channel_param.set_phase_reverse(m_phase_reverse[i]);

                    auto mutable_gates = channel_param.mutable_gates();
                    for (auto j = 0; std::cmp_less(j, m_gate_info[i].size()); j++) {
                        Proto::Union::HDBridge::Gate gate;
                        if (m_gate_info[i][j].has_value() && (m_gate_info[i][j]->enable == true)) {
                            gate.set_pos(m_gate_info[i][j]->pos);
                            gate.set_width(m_gate_info[i][j]->width);
                            gate.set_height(m_gate_info[i][j]->height);
                            gate.set_index(j);
                            mutable_gates->Add(std::move(gate));
                        }
                    }
                    channel_params->Add(std::move(channel_param));
                }
            }
            std::ofstream file(file_name.toStdWString(), std::ios::binary);
            if (!file.is_open()) {
                throw std::runtime_error("open file failed");
            }
            auto ret = config_data.SerializePartialToOstream(&file);
            file.flush();
            return ret;
        } catch (std::exception &e) {
            qCCritical(TAG) << e.what();
            return false;
        }
    }

    bool HDBridgeIntf::deserializeConfigData(const QString &file_name) {
        try {
            std::ifstream file(file_name.toStdWString(), std::ios::binary);
            if (!file.is_open()) {
                qCWarning(TAG) << "failed to open file:" << file_name;
                return false;
            }
            Proto::Union::HDBridge::ConfigData config_data;
            if (config_data.ParseFromIstream(&file) != true) {
                qCWarning(TAG) << "failed to parse file:" << file_name;
                return false;
            }

            {
                std::lock_guard lock(m_param_mutex);
                if (getChannelNumber() != config_data.channel_parameters_size()) {
                    auto msg = QString::asprintf("The number of channels does not match, current HDBridge's channel is %d,"
                                                 "but the config file's channel is %d",
                                                 getChannelNumber(), config_data.channel_parameters_size());
                    qCWarning(TAG) << msg.toStdString().c_str();
                }
                m_frequency    = config_data.repeat_freqency();
                m_voltage      = config_data.emit_voltage();
                m_channel_flag = config_data.channel_flag();
                m_damper_flag  = config_data.damper_flag();
                for (auto i = 0; std::cmp_less(i, std::min(getChannelNumber(), config_data.channel_parameters_size())); i++) {
                    const auto channel_param = config_data.channel_parameters(i);
                    m_velocity[i]            = channel_param.sound_velocity();
                    m_zero_bias[i]           = channel_param.zero_bias();
                    m_pulse_width[i]         = channel_param.pulse_width();
                    m_delay[i]               = channel_param.sampling_delay();
                    m_sample_depth[i]        = channel_param.sampling_depth();
                    m_sample_factor[i]       = channel_param.sampling_factor();
                    m_gain[i]                = channel_param.gain();
                    m_filter[i]              = channel_param.filter_band();
                    m_demodu[i]              = channel_param.demodu_mode();
                    m_phase_reverse[i]       = channel_param.phase_reverse();
                    std::generate(m_gate_info[i].begin(), m_gate_info[i].end(), [&]() { return std::nullopt; });
                    for (auto j = 0; std::cmp_less(j, std::min(channel_param.gates_size(), getGateNumber())); j++) {
                        const auto gate              = channel_param.gates(j);
                        m_gate_info[i][gate.index()] = std::make_optional<Union::Base::Gate>(gate.pos(), gate.width(), gate.height(), true);
                    }
                }
            }
            return true;
        } catch (std::exception &e) {
            qCCritical(TAG) << e.what();
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

    bool HDBridgeIntf::autoGain(int ch, int gate_idx, double target, int timeout_ms, uint8_t max_value) {
        if (target <= 0.0) {
            qCWarning(TAG) << "error target";
            return false;
        }
        const auto           _ch           = ch % getChannelNumber();
        const auto           _gate_idx     = gate_idx % getGateNumber();
        constexpr std::array MAP_GAIN_STEP = {
            std::make_pair(2.0, 6.0),
            std::make_pair(1.25, 2.0),
            std::make_pair(1.12, 1.0),
            std::make_pair(1.05, 0.5),
            std::make_pair(1.02, 0.2),
            std::make_pair(1.01, 0.1),
        };
        {
            std::lock_guard lock(m_scan_data_mutex);
            if (!m_scan_data[_ch]->gate[_gate_idx].has_value()) {
                qCWarning(TAG) << "no gate on channel:" << ch << "gate num:" << gate_idx;
                return false;
            }
        }
        auto _current_time = std::chrono::system_clock::now();
        while (std::chrono::system_clock::now() - _current_time <= std::chrono::milliseconds(timeout_ms)) {
            Union::Base::GateResult gate_res = std::nullopt;
            {
                std::lock_guard lock(m_scan_data_mutex);
                gate_res = m_scan_data[_ch]->gate_result[_gate_idx];
            }
            if (!gate_res.has_value()) {
                qCDebug(TAG) << "no gate value";
                break;
            }
            double rate = (std::get<Base::GATE_MAX_AMP>(gate_res.value()) / static_cast<double>(max_value)) / (target / 100.0);
            double sign = rate > 1.0 ? -1.0 : 1.0;
            rate        = rate > 1.0 ? rate : 1.0 / rate;
            double step = 0.0;
            for (auto i = 0; i < std::ssize(MAP_GAIN_STEP); i++) {
                if (rate >= MAP_GAIN_STEP[i].first) {
                    step = MAP_GAIN_STEP[i].second;
                    break;
                }
            }
            if (step == 0.0) {
                return true;
            }
            auto gain = getGain(ch);
            gain += sign * step;
            setGain(ch, gain);
            auto _c = std::chrono::system_clock::now();
            sync2Board();
            using namespace std::chrono_literals;
            std::this_thread::sleep_until(_c + 200ms);
        }
        if (std::chrono::system_clock::now() - _current_time > std::chrono::milliseconds(timeout_ms)) {
            qCDebug(TAG) << "timeout";
        }
        return false;
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
        auto     _last_invoke_time = std::chrono::system_clock::now();
        _T_DataV _mirror_scan_data = {}; // 扫查数据镜像
        _mirror_scan_data.resize(getChannelNumber());
        std::generate(_mirror_scan_data.begin(), _mirror_scan_data.end(), []() -> std::shared_ptr<ScanData> {
            return nullptr;
        });

        auto RUN_HELPER = [&](auto func) {
            if constexpr (std::is_same_v<decltype(func), _T_R_ONE>) {
                // 读取函数接口的形式为: 一次读取单个通道的数据
                std::vector<bool> _ch_update_flag; // 用于标记通道是否更新
                _ch_update_flag.resize(getChannelNumber());
                std::generate(_ch_update_flag.begin(), _ch_update_flag.end(), []() -> bool {
                    return false;
                });
                while (m_thread_running) {
                    auto data = func();
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
                        std::this_thread::sleep_until(_last_invoke_time + 10ms);
                        _last_invoke_time = _current_invoke_time;
                    }
                }
            } else if constexpr (std::is_same_v<decltype(func), _T_R_ALL>) {
                // 读取函数接口的形式为: 一次读取所有通道的数据
                while (m_thread_running) {
                    _mirror_scan_data = func();
                    if (std::ssize(_mirror_scan_data) == getChannelNumber()) {
                        invokeCallback(_mirror_scan_data); // 执行回调函数, 默认异步执行
                    }
                    {
                        std::lock_guard lock(m_scan_data_mutex);
                        m_scan_data = _mirror_scan_data; // 更新镜像数据(作用域是为了减少lock的持有时间)
                    }
                    auto _current_invoke_time = std::chrono::system_clock::now();
                    using namespace std::chrono_literals;
                    // 限制最大帧率为100帧
                    std::this_thread::sleep_until(_last_invoke_time + 10ms);
                    _last_invoke_time = _current_invoke_time;
                }
            }
        };
        if (!m_read_intf.has_value()) {
            throw std::runtime_error("no read interface");
        }
        std::visit(RUN_HELPER, m_read_intf.value());
    }

    void HDBridgeIntf::fileReadThread(const QString &file_name, std::optional<double> fps) {
        auto _last_invoke_time = std::chrono::system_clock::now();
        auto _file_Data        = deserializeScanData(file_name);
        qCDebug(TAG) << "file Data size: " << _file_Data.size();

#if EXIT_ON_EOF
        // 文件结束时退出线程
        for (int i = 0; (i < std::ssize(_file_Data)) && m_thread_running; i++) {
#else
        // 文件结束时从头读取
        for (int i = 0; m_thread_running; i++) {
#endif
            invokeCallback(_file_Data[i % std::ssize(_file_Data)]); // 执行回调函数, 默认异步执行
            using namespace std::chrono_literals;
            auto sleep_time = 10000us / 1.0;
            if (fps.has_value()) {
                sleep_time = 1000000us / fps.value();
            }
            std::this_thread::sleep_until(_last_invoke_time + sleep_time);
            _last_invoke_time = std::chrono::system_clock::now();
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

    std::mutex &HDBridgeIntf::getParamLock() const noexcept {
        return m_param_mutex;
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
            qCCritical(TAG) << e.what();
        }
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

    bool HDBridgeIntf::setAxisBias(int ch, double mm) {
        return setDelay(ch, mm2us(mm, getSoundVelocity(ch)));
    }

    double HDBridgeIntf::getDelay(int ch) const {
        std::lock_guard lock(m_param_mutex);
        return m_delay[ch % getChannelNumber()];
    }

    double HDBridgeIntf::getAxisBias(int ch) const {
        return us2mm(getDelay(ch), getSoundVelocity(ch));
    }

    bool HDBridgeIntf::setAxisLength(int ch, double mm) {
        return setSampleDepth(ch, mm2us(mm, getSoundVelocity(ch)));
    }

    double HDBridgeIntf::getSampleDepth(int ch) const {
        std::lock_guard lock(m_param_mutex);
        return m_sample_depth[ch % getChannelNumber()];
    }

    double HDBridgeIntf::getAxisLength(int ch) const {
        return us2mm(getSampleDepth(ch), getSoundVelocity(ch));
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
