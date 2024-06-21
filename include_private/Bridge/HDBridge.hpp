#pragma once

#include "../common/common.hpp"
#include <QObject>
#include <array>
#include <atomic>
#include <cstdint>
#include <map>
#include <mutex>
#include <stack>
#include <string_view>
#include <vector>

namespace Union::Bridge::MultiChannelHardwareBridge {

    struct ScanData {
        using _GateR  = std::vector<std::optional<Union::Base::GateResult>>;
        using _GateV  = std::vector<std::optional<Union::Base::Gate>>;
        using _AScanV = std::vector<uint8_t>;

        int     package_index = {}; ///< 包序号
        int     channel       = {}; ///< 通道号
        double  xAxis_start   = {}; ///< X轴开始位置
        double  xAxis_range   = {}; ///< X轴长度
        _AScanV ascan         = {}; ///< A扫数据
        _GateV  gate          = {}; ///< 波门
        _GateR  gate_result   = {}; ///< 波门计算结果
    };

    class HDBridgeIntf {
    public:
        explicit HDBridgeIntf();
        virtual ~HDBridgeIntf();

        using InftCallbackFunc = std::function<void(const ScanData &, const HDBridgeIntf &)>;

        /**
         * @brief 打开设备
         * @return true if success
         */
        virtual bool open() = 0;

        /**
         * @brief 判断设备是否是打开状态
         * @return true if is open
         */
        virtual bool isOpen() = 0;

        /**
         * @brief 关闭设备
         * @return true if success
         */
        virtual bool close() = 0;

        /**
         * @brief 判断设备是否存在
         * @return true if exist
         */
        virtual bool isDeviceExist() = 0;

        /**
         * @brief 获取通道数
         *
         * @return int
         */
        virtual int getChannelNumber() const = 0;

        /**
         * @brief 获取波门数量
         *
         * @return int
         */
        virtual int getGateNumber() const = 0;

        /**
         * @brief 加载默认配置
         *
         */
        virtual void loadDefaultConfig() = 0;

        /**
         * @brief 设置重复频率
         * @param freq 重复频率(MHz)
         * @return true if success
         */
        virtual bool setFrequency(int freq) = 0;

        /**
         * @brief 获取重复频率
         * @return 重复频率(MHz)
         */
        virtual int getFrequency() const final;

        /**
         * @brief 设置发射电压
         * @param volt 发射电压 enum
         * @return true if success
         */
        virtual bool setVoltage(int volt) = 0;

        /**
         * @brief 获取发射电压
         * @return 发射电压 enum
         */
        virtual int getVoltage() const final;

        /**
         * @brief 获取电压表
         *
         * @return const std::vector<int>
         */
        virtual const QVector<QString> &getVoltageTable() const = 0;

        /**
         * @brief 设置通道标志
         * @param flag 通道标志
         * @return true if success
         */
        virtual bool setChannelFlag(uint32_t flag) = 0;

        /**
         * @brief 获取通道标志
         * @return 通道标志
         */
        virtual uint32_t getChannelFlag() const final;

        /**
         * @brief 设置阻尼标志
         * @param flag 阻尼标志
         * @return true if success
         */
        virtual bool setDamperFlag(int flag) = 0;

        /**
         * @brief 获取阻尼标志
         * @return 阻尼标志
         */
        virtual int getDamperFlag() const final;

        /**
         * @brief 设置声速
         * @param ch 通道号
         * @param velocity 声速(m/s)
         * @return true if success
         */
        virtual bool setSoundVelocity(int ch, double velocity) = 0;

        /**
         * @brief 获取声速
         *
         * @param ch 通道号
         * @return 声速(m/s)
         */
        virtual double getSoundVelocity(int ch) const final;

        /**
         * @brief 设置零点偏移
         * @param ch 通道号
         * @param bias_us 零点偏移(μs)
         * @return true if success
         */
        virtual bool setZeroBias(int ch, double bias_us) = 0;

        /**
         * @brief 获取零点偏移
         *
         * @param ch 通道号
         * @return double 零点偏移(μs)
         */
        virtual double getZeroBias(int ch) const final;

        /**
         * @brief 设置脉冲宽度
         * @param ch 通道号
         * @param width 脉冲宽度(μs)
         * @return true if success
         */
        virtual bool setPulseWidth(int ch, double width_us) = 0;

        /**
         * @brief 获取脉冲宽度
         *
         * @param ch 通道号
         * @return 脉冲宽度(μs)
         */
        virtual double getPulseWidth(int ch) const final;

        /**
         * @brief 设置延时
         * @param ch 通道号
         * @param delay 延时(μs)
         * @return true if success
         */
        virtual bool setDelay(int ch, double delay_us) = 0;

        /**
         * @brief 获取延时
         *
         * @param ch 通道号
         * @return 延时(μs)
         */
        virtual double getDelay(int ch) const final;

        /**
         * @brief 设置采样深度
         * @param ch 通道号
         * @param depth 采样深度
         * @return true if success
         */
        virtual bool setSampleDepth(int ch, double depth) = 0;

        /**
         * @brief 获取采样深度
         *
         * @param ch 通道号
         * @return 采样深度
         */
        virtual double getSampleDepth(int ch) const final;

        /**
         * @brief 设置采样因子
         * @param ch 通道号
         * @param factor 采样因子
         * @return true if success
         */
        virtual bool setSampleFactor(int ch, int factor) = 0;

        /**
         * @brief 获取采样因子
         *
         * @param ch 通道号
         * @return 采样因子
         */
        virtual int getSampleFactor(int ch) const final;

        /**
         * @brief 设置增益
         * @param ch 通道号
         * @param gain 增益
         * @return true if success
         */
        virtual bool setGain(int ch, double gain) = 0;

        /**
         * @brief 获取增益
         *
         * @param ch 通道号
         * @return 增益
         */
        virtual double getGain(int ch) const final;

        /**
         * @brief 设置滤波器
         * @param ch 通道号
         * @param filter_index 滤波器索引
         * @return true if success
         */
        virtual bool setFilter(int ch, int filter_index) = 0;

        /**
         * @brief 获取滤波器
         *
         * @param ch 通道号
         * @return 滤波器索引
         */
        virtual int getFilter(int ch) const final;

        /**
         * @brief 获取滤波频带表
         *
         * @return QVector<QString>&
         */
        virtual const QVector<QString> &getFilterTable() const = 0;

        /**
         * @brief 设置检波方式
         * @param ch 通道号
         * @param demodu_index 检波方式索引
         * @return true if success
         */
        virtual bool setDemodu(int ch, int demodu_index) = 0;

        /**
         * @brief 获取检波方式
         *
         * @param ch 通道号
         * @return 检波方式索引
         */
        virtual int getDemodu(int ch) const final;

        /**
         * @brief 获取检波方式表
         *
         * @return QVector<QString>&
         */
        virtual const QVector<QString> &getDemoduTable() const = 0;

        /**
         * @brief 设置相位反转
         * @param ch 通道号
         * @param enable 是否开启
         * @return true if success
         */
        virtual bool setPhaseReverse(int ch, bool enable) = 0;

        /**
         * @brief 获取相位反转
         *
         * @param ch 通道号
         * @return 是否开启
         */
        virtual bool getPhaseReverse(int ch) const final;

        /**
         * @brief 设置波门信息
         * @param ch 通道号
         * @param idx 波门索引
         * @param info 波门信息
         * @return true if success
         */
        virtual bool setGateInfo(int ch, int idx, const std::optional<Union::Base::Gate> &gate) final;

        /**
         * @brief 获取波门信息
         *
         * @param ch 通道号
         * @param idx 波门索引
         * @return 波门信息
         */
        virtual const std::optional<Union::Base::Gate> &getGateInfo(int ch, int idx) const final;

        /**
         * @brief 同步参数至板卡
         * @return true if success
         */
        virtual bool sync2Board(void) const = 0;

        /**
         * @brief 通道参数复制参数
         * @param src 复制源
         * @param dist 复制目标列表
         * @param max_gate_number 最大波门数量
         */
        virtual void paramCopy(int src, std::vector<int> dist, int max_gate_number) final;

        /**
         * @brief 序列化扫查数据
         *
         * @param file_name 文件名
         * @return bool
         */
        virtual bool serializeScanData(const std::wstring &file_name) const;

        /**
         * @brief 反序列化扫查数据
         *
         * @param file_name 文件名
         * @return bool
         */
        virtual bool deserializeScanData(const std::wstring &file_name);

        /**
         * @brief 序列化配置文件
         *
         * @param file_name 文件名
         * @return bool
         */
        virtual bool serializeConfigData(const std::wstring &file_name) const;

        /**
         * @brief 反序列化配置文件
         *
         * @param file_name 文件名
         * @return bool
         */
        virtual bool deserializeConfigData(const std::wstring &file_name);

        /**
         * @brief 添加回调函数
         *
         * @param func 回调函数
         * @return bool
         */
        virtual bool appendCallback(InftCallbackFunc func) final;

        /**
         * @brief 清空回调函数
         *
         * @return bool
         */
        virtual bool clearCallback(void) final;

        /**
         * @brief 保存回调函数
         *
         * @return bool
         */
        virtual bool storeCallback(void) final;

        /**
         * @brief 恢复回调函数
         *
         * @return bool
         */
        virtual bool restoreCallback(void) final;

        bool operator==(const HDBridgeIntf &rhs) const;

    protected:
        std::list<InftCallbackFunc>             m_callback_list  = {};
        std::stack<std::list<InftCallbackFunc>> m_callback_stack = {};
        mutable std::mutex                      m_callback_mutex = {};
        std::atomic<bool>                       m_thread_running = false;
        std::thread                             m_read_thread    = {};
        mutable std::mutex                      m_param_mutex    = {};

        using _T_GateV = std::vector<std::vector<std::optional<Union::Base::Gate>>>;
        using _T_DataV = std::vector<std::shared_ptr<ScanData>>;

        // param
        int                 m_frequency     = {}; ///< 重复频率
        int                 m_voltage       = {}; ///< 电压
        uint32_t            m_channel_flag  = {}; ///< 通道标志
        int                 m_damper_flag   = {}; ///< 阻尼标志
        std::vector<double> m_velocity      = {}; ///< 声速s
        std::vector<double> m_zero_bias     = {}; ///< 零点偏移
        std::vector<double> m_pulse_width   = {}; ///< 脉冲宽度
        std::vector<double> m_delay         = {}; ///< 延时
        std::vector<double> m_sample_depth  = {}; ///< 采样深度
        std::vector<int>    m_sample_factor = {}; ///< 采样因子
        std::vector<double> m_gain          = {}; ///< 增益
        std::vector<int>    m_filter        = {}; ///< 滤波频带
        std::vector<int>    m_demodu        = {}; ///< 检波方式
        std::vector<bool>   m_phase_reverse = {}; ///< 相位翻转
        _T_GateV            m_gate_info     = {}; ///< 检波信息

        // scan data
        _T_DataV           m_scan_data       = {}; ///< 扫查数据
        mutable std::mutex m_scan_data_mutex = {}; ///< 扫查数据锁

        bool m_param_is_init = false;

        /**
         * @brief 读取一帧数据
         *
         * @return std::shared_ptr<ScanData>
         */
        virtual std::shared_ptr<ScanData> readOneFrame(void) = 0;

        /**
         * @brief 运行读取线程
         *
         */
        void runReadThread(void);

        /**
         * @brief 关闭读取线程并等待线程退出
         *
         */
        void closeReadThreadAndWaitExit(void) noexcept;

        /**
         * @brief 读取线程
         *
         */
        void readThread(void);

        /**
         * @brief 初始化参数
         *
         */
        void initParam(void);

        /**
         * @brief 锁定参数
         *
         */
        void lock_param(void);

        /**
         * @brief 解锁参数
         *
         */
        void unlock_param(void);
    };

} // namespace Union::Bridge::MultiChannelHardwareBridge
