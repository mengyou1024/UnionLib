#pragma once

#include "../AScanType.hpp"
#include "./_330_common.hpp"
#include <QPointF>
#include <QVector>
#include <cstdint>
#include <memory>

namespace Union::__330 {

#pragma pack(1)
    struct SystemStatus {
        uint32_t elapsedTime;      ///< 开机时长(ms)
        uint16_t startTime;        ///< 开机时间
        uint16_t startelapsedTime; ///< 开机时长(min)
        uint32_t dasy;             ///< 距2000.1.1天数
        uint32_t denoise;          ///< 降噪次数
        uint32_t repeatFreq;       ///< 重复频率
        uint32_t pulseWidth;       ///< 脉冲宽度
        uint32_t solidWave;        ///< 实心波

        /**
         * d31-d29打印机，d28-26输入法,d25测试状态,d24采样状态，d23硬件波形，d22软件波形
         * d21允许光标，d20有无光标，d19-d16用户数(最大16)，d15-d12通道号(最大16)
         * d11按键声音d10报警声音,d9d8屏幕亮度，d7-d0输入法是否可选
         * */
        uint32_t sys;

        /**
         *  d31波形记录方式(0单幅1连续),d30启动连续存,d29-d22记录间隔(单位为0.1秒,最大25.0秒)
         * d21存储时是否记录缺陷波频谱，d20-d16当前选择的探伤工艺
         * d15d14颜色模式
         * D13曲线显示模式，0波幅，1dB值，此时自动调整增益
         * D12键盘锁
         * d11,d10门A门B内回波是否高于门高
         * d9=1A门变色,d8=1B门变色
         * d7=0峰值读数1前沿读数
         */
        uint32_t option;

        uint32_t                displayMode;       ///< 显示方式 1 全屏 0 非全屏
        uint32_t                mainMenuItem;      ///< 当前主菜单项数，每页6项， 最多24页
        uint32_t                enableEcohWaveLen; ///< 显示回波长度
        uint32_t                coorWidth;         ///< 坐标宽度
        uint32_t                coorExtern;        ///< 坐标区右方扩展点数
        uint32_t                hunit;             ///< 坐标区水平方向每格点数
        uint32_t                language;          ///< 语言
        uint32_t                unit;              ///< 单位
        std::array<uint8_t, 24> menu2Paga;         ///< 次级菜单页数，最多4也，每页4项
        std::array<uint32_t, 4> reserve;           ///< 预留值
    };

    struct ChannelStatus {
        uint32_t sys;

        uint32_t block;  ///< 试块
        uint32_t option; ///< 参量

        uint32_t status;

        uint32_t                externStatus;
        std::array<uint32_t, 4> reserved;
    };

    struct ChannelParam {
        uint32_t                range;
        uint32_t                oldRange; ///< 展宽前的声程
        uint32_t                oldDelay; ///< 展宽前的延时
        uint32_t                delay;
        std::array<uint32_t, 4> wavepara;     ///< 波形参数(距离、水平、垂直、高度/位置)
        uint16_t                thick;        ///< 厚度
        uint16_t                diameter;     ///< 直径
        uint16_t                offset;       ///< 偏移
        uint16_t                speed;        ///< 声速(m/s)
        uint16_t                angle;        ///< 角度
        uint16_t                forward;      ///< 前沿
        uint16_t                freqence;     ///< 频率
        uint16_t                reject;       ///< 抑制
        uint16_t                surGain;      ///< 表面补偿增益
        uint16_t                baseGain;     ///< 基础增益
        uint16_t                depthGain;    ///< 深度补偿增益
        int16_t                 compGain;     ///< 扫查增益
        std::array<int16_t, 6>  lineGain;     ///< 6条线偏移增益
        int16_t                 gateDB;       ///< 门dB
        int16_t                 depth;        ///< 缺陷高度
        uint16_t                crystal_l;    ///< 晶片长度
        uint16_t                crystal_w;    ///< 晶片宽度
        uint16_t                poreLength;   ///< 横孔长度
        uint16_t                poreDiameter; ///< 横孔直径
        std::array<uint16_t, 2> poreDepth;    ///< 2横孔深度
    };

    struct GateParam {
        uint16_t pos;
        uint16_t width;
        uint16_t height;
        uint8_t  type;
        uint8_t  alarmEnable;
    };

    struct DACParam {
        std::array<uint16_t, 10> dB;
        std::array<uint16_t, 10> dist;
        int16_t                  num;
        uint16_t                 diameter;
        uint16_t                 length;
    };

    struct WeldParam {
        uint32_t type;   // 类型0无，1单面，2双面，3T型
        uint32_t face;   // 探测面0A面，1B面，2C面
        uint32_t thick1; // 厚度1，工件厚度、翼板厚度<200.0mm;
        uint32_t thick2; // 厚度2，有效厚度、腹板厚度<200.0mm;
        uint32_t wide;   // 焊缝宽度、A面焊脚，< 100mm;
        uint32_t radius; // 根部半径、B面焊脚，< 100mm;
        uint32_t high1;  // 上端余高、A端余高，<100mm;
        uint32_t high2;  // 下端余高、B端余高，<100mm;
        uint32_t angle1; // 坡口角度、A坡口角，< 180;
        uint32_t angle2; // 坡口面角、B坡口角，< 90;
        uint32_t deep;   // 坡口深度，< 100mm;
        uint32_t blunt;  // 钝边高度，< 100mm;
        uint32_t space;  // 根部间隙，< 100mm
        uint32_t probe;
        uint32_t angle3; // 坡内角度，< 180;
        uint32_t angle4; // 坡内面角，< 180;
        uint32_t deep3;  // 坡内深度，< 100mm;
    };

    struct GatePeakSingleParam {
        uint32_t Gate1PeakSampPoints;
        uint32_t Gate2PeakSampPoints;
        uint32_t Gate3PeakSampPoints;
        uint16_t Gate1PeakPos;
        uint16_t Gate2PeakPos;
        uint16_t Gate3PeakPos;
        uint16_t Gate1PeakAmp;
        uint16_t Gate2PeakAmp;
        uint16_t Gate3PeakAmp;
    };

    class DASType : public Union::AScan::AScanIntf, public Union::__330::_330_DAC_C {
    public:
        SystemStatus             systemStatus;
        ChannelStatus            channelStatus;
        ChannelParam             channelParam;
        std::array<GateParam, 2> gateParam;
        DACParam                 dacParam;
        std::array<WeldParam, 2> weldParam;
        GatePeakSingleParam      gatePeakSingleParam;
        uint32_t                 channelTemp;
        uint16_t :16;

        std::vector<uint8_t> data;
        std::vector<uint8_t> name;

        std::vector<std::wstring> m_fileNameList = {};

        static std::unique_ptr<Union::AScan::AScanIntf> FromFile(const std::wstring& fileName);

        virtual size_t __Read(std::ifstream& file, size_t file_size) override;
        virtual int    getDataSize(void) const override;

        virtual std::vector<std::wstring> getFileNameList(void) const override;
        virtual void                      setFileNameIndex(int index) override;

        // IMPL
        Base::Performance getPerformance(int idx) const override;
        std::string       getDate(int idx) const override;
        std::wstring      getProbe(int idx) const override;
        double            getProbeFrequence(int idx) const override;
        std::string       getProbeChipShape(int idx) const override;
        double            getAngle(int idx) const override;
        double            getSoundVelocity(int idx) const override;
        double            getFrontDistance(int idx) const override;
        double            getZeroPointBias(int idx) const override;
        double            getSamplingDelay(int idx) const override;
        int               getChannel(int idx) const override;
        std::string       getInstrumentName(void) const override;

        std::array<Base::Gate, 2>   getGate(int idx = 0) const override;
        const std::vector<uint8_t>& getScanData(int idx = 0) const override;
        double                      getAxisBias(int idx = 0) const override;
        double                      getAxisLen(int idx = 0) const override;
        double                      getBaseGain(int idx = 0) const override;
        double                      getScanGain(int idx = 0) const override;
        double                      getSurfaceCompentationGain(int idx = 0) const override;
        int                         getSupression(int idx = 0) const override;
        Union::AScan::DistanceMode  getDistanceMode(int idx = 0) const override;
        std::optional<Base::AVG>    getAVG(int idx = 0) const override;
        std::optional<Base::DAC>    getDAC(int idx = 0) const override;
        Union::AScan::DAC_Standard  getDACStandard(int idx = 0) const override;

        virtual const std::array<QVector<QPointF>, 3>& unResolvedGetDacLines(int idx) const override;

        virtual void setUnResolvedGetDacLines(const std::array<QVector<QPointF>, 3>& dat, int idx) override;

        virtual QJsonArray createGateValue(int idx, double soft_gain) const override;

        virtual std::pair<double, double> getProbeSize(int idx) const override final;

    private:
        int                             getOption(void) const noexcept;
        double                          getUnit(void) const noexcept;
        uint8_t                         convertDB2GateAMP(int db) const;
        std::array<QVector<QPointF>, 3> m_dacLines = {};
    };
#pragma pack()
} // namespace Union::__330
