#pragma once

#include "../HDBridge.hpp"
#include <ftd2xx.h>
#include <vector>

namespace Union::Bridge::MultiChannelHardwareBridge {
    class _8CH_USB : public HDBridgeIntf {
    public:
        explicit _8CH_USB(int gate_number = 2);
        virtual ~_8CH_USB();

        virtual bool open() override;

        virtual bool isOpen() override;

        virtual bool close() override;

        virtual bool isDeviceExist() override;

        virtual int getChannelNumber() const override;

        virtual int getGateNumber() const override;

        virtual void loadDefaultConfig() override;

        virtual bool setFrequency(int freq) override;

        virtual bool setVoltage(int volt) override;

        virtual const QVector<QString> &getVoltageTable() const override;

        virtual bool setChannelFlag(uint32_t flag) override;

        virtual bool setDamperFlag(int flag) override;

        virtual bool setSoundVelocity(int ch, double velocity) override;

        virtual bool setZeroBias(int ch, double bias_us) override;

        virtual bool setPulseWidth(int ch, double width_us) override;

        virtual bool setDelay(int ch, double delay_us) override;

        virtual bool setSampleDepth(int ch, double depth) override;

        virtual bool setSampleFactor(int ch, int factor) override;

        virtual bool setGain(int ch, double gain) override;

        virtual bool setFilter(int ch, int filter_index) override;

        virtual const QVector<QString> &getFilterTable() const override;

        virtual bool setDemodu(int ch, int demodu_index) override;

        virtual const QVector<QString> &getDemoduTable() const override;

        virtual bool setPhaseReverse(int ch, bool enable) override;

        virtual bool sync2Board(void) const override;

    private:
        int m_channel_number = 8;
        int m_gate_number    = 2;

        mutable std::mutex m_usb_mutex = {};

#pragma pack(1)
        struct SYS_CFG_INFO {
            uint8_t  Scan_Unit   = {0}; // 扫查增量寄存器 取值范围[0--4]
            uint8_t  Encoder_Rst = {0}; // 编码器清零寄存器
            uint8_t  TestWR      = {0}; // 测试寄存器 取值范围[0--3]
            uint8_t  Resv1       = {0};
            uint32_t RepeatFreq  = {1000}; // 重复频率 取值范围[100--16K]

            uint8_t HV_Sel    = {1}; // 高压选择 取值范围[0--3]
            uint8_t Chl_Cycle = {8}; // 循环通道总数 取值范围[0--7]
            uint8_t Resv2     = {0};
            uint8_t Digit_IO  = {0}; // 数字输出接口寄存器，对应Extended Interface的输出控制，具体控制功能待定。

            std::array<uint8_t, 4> Reserve = {}; // 预留
        };

        struct CHAN_CFG_INFO {
            uint8_t  Tx_Chl_No   = {};   // 发射通道 取值范围[0--7]
            uint8_t  Pulse_Width = {10}; // 发射脉冲宽度 取值范围[3--100] 单位10ns
            uint8_t  Rx_Chl_No   = {};   // 接收通道 取值范围[0--7]
            uint8_t  Fixgain     = {};   // 增益段选择控制寄存器
            uint16_t DA_Gain     = {};   // 主放DA[0--1100] 110db
            uint8_t  DemoduSel   = {1};  // 检波选择 取值[0--射频;1--全波;2--正半波;3--负半波]
            uint8_t  Echo_Reject = {0};  // 回波抑制[0--100]

            uint32_t SampLen   = {10000}; // 采样声程 取值范围[512--(100M/重复频率-延时-DELAY_CHAN)]，以10ns为单位
            uint32_t SampDelay = {0};     // 采样延时 取值范围[0--(100M/重复频率-声程-DELAY_CHAN)]，以10ns为单位

            std::array<uint32_t, 3> GatePos     = {}; // 波门一门位 范围[0 --- 声程  减门宽] 单位10ns
            std::array<uint32_t, 3> GateWidt    = {}; // 波门一宽度 范围[0 --- 声程减门位] 单位10ns
            uint8_t                 Gate_2_Type = {}; // 波门二类型 [0--独立; 1--相对,波门2的门位寄存器是相对量,绝对量需要加上波门1峰值的位置]
            uint8_t                 Gate_3_Type = {}; // 波门三类型 [0--独立; 1--相对,波门3的门位寄存器是相对量,绝对量需要加上波门1峰值的位置]
            std::array<uint8_t, 6>  Reserve     = {}; // 预留
        };

        struct UDA_CFG {
            SYS_CFG_INFO                 sysParam = {};
            std::array<CHAN_CFG_INFO, 8> chParam  = {};
            std::array<uint8_t, 112>     Reserve  = {};
        };

        struct UDA_CHAN_DATA {
            std::array<uint8_t, 480> AcanData;    // ASCAN点数
            uint8_t                  Chan_No  :3; // 通道号
            uint8_t                  res1     :1; // 通道号
            uint8_t                  Digit_In :2;
            uint8_t                  res2     :2;
            uint8_t                  FPGA_Ver;
            uint16_t                 FPGA_date;
            uint32_t                 Encoder1; // 编码器一计数值
            uint32_t                 Encoder2; // 编码器二计数值
            std::array<uint32_t, 3>  PeekPos;  // 门内峰值位置
            std::array<uint16_t, 3>  PeekAmp;  // 门内峰值幅度
        };
#pragma pack()

        using _T_DL = std::tuple<int, std::shared_ptr<FT_DEVICE_LIST_INFO_NODE>, bool, std::shared_ptr<UDA_CFG>>;

        mutable std::vector<_T_DL> m_device_list = {};

        inline static constexpr auto STRUCT_CONFIG_SIZE  = sizeof(UDA_CFG);
        inline static constexpr auto STRUCT_CH_DATA_SIZE = sizeof(UDA_CHAN_DATA);

        inline static constexpr auto ID_DEVICE_INDEX   = 0;
        inline static constexpr auto ID_DEVICE_NODE    = 1;
        inline static constexpr auto ID_DEVICE_IS_OPEN = 2;
        inline static constexpr auto ID_DEVCIE_CONFIG  = 3;

        std::vector<std::shared_ptr<ScanData>> readAllFrame(void);

        void private_init_params(void);
    };
} // namespace Union::Bridge::MultiChannelHardwareBridge
