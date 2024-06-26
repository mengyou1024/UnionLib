#pragma once

#include "../AScanType.hpp"
#include "./_330_common.hpp"
#include <array>
#include <cstdint>
#include <memory>
#include <string_view>

namespace Union::__390 {
    class HFDATType;
}

namespace Union::__330 {
#pragma pack(1)
    struct HEADER_TIMESTAMP_DAT {
        uint8_t second;
        uint8_t minute;
        uint8_t hour;
        uint8_t notes_len;
    };

    struct SYSTEM_STATUS_DAT {
        uint32_t nElapsedTime;      // 开机时长，以毫秒为单位
        uint32_t nStartElapsedTime; // 高16位开机时间，低16位开机时长，以分为单位
        uint32_t days       :16;    // 距2000年1月1日的天数
        uint32_t denoise    :3;     // 降噪次数
        uint32_t repeat     :10;    // 重复频率
        uint32_t pulsewidth :1;     // 脉冲宽度
        uint32_t fill       :1;     // 实心波
        uint32_t sys;               // d31-d29打印机，d28-26输入法,d25测试状态,d24采样状态，d23硬件波形，d22软件波形
                                    // d21允许光标，d20有无光标，d19-d16用户数(最大16)，d15-d12通道号(最大16)
                                    // d11按键声音d10报警声音,d9d8屏幕亮度，d7-d0输入法是否可选

        uint32_t option; // d31波形记录方式(0单幅1连续),d30启动连续存,d29-d22记录间隔(单位为0.1秒,最大25.0秒)
                         // d21存储时是否记录缺陷波频谱，d20-d16当前选择的探伤工艺
                         // d15d14颜色模式
                         // D13曲线显示模式，0波幅，1dB值，此时自动调整增益
                         // D12键盘锁
                         // d11,d10门A门B内回波是否高于门高
                         // d9=1A门变色,d8=1B门变色
                         // d7=0峰值读数1前沿读数

        // U2: start
        uint32_t displaymode  :1;  // 显示方式,1:全屏,0非
        uint32_t mainmenuitem :5;  // 当前主菜单项数,每页6项,最多24项
        uint32_t samp_len     :10; // 显示回波长度
        uint32_t coor_width   :9;  // 坐标区长度
        uint32_t coor_extern  :6;  // 坐标区右方扩展点数
        uint32_t hunit        :6;  // 坐标区水平方向每格点数
        uint32_t language     :2;  // 语言
        uint32_t unit         :2;  // 单位
        uint8_t  menu2page[24];    // 次级菜单页数,最多4页,每页4项
        // U2: end
        uint32_t prepare[4]; // 预留值
    };

    struct CHANNEL_STATUS_DAT {
        uint32_t sys; // 仪器探头状态，31d30:频带宽度：2.5\5.0\10\wide，
                      // d29d28检波方式：全波\正半波\负半波\射频 d27d26探头阻尼：50Q\400Q
                      // d25d24发射电压：200V\400V ,d23-d20重复频率，
                      // d19d18降噪次数：1-4次，d17d16波形:纵波/横波
                      // d15-d12探头方式：单直/单斜/双晶/穿透
                      // d11-d8标准号
                      // d7-d0功能

        uint32_t block; // 试块，d19-d0一位对应一种试块，1表示在试块选择菜单中会出现
                        // d31-d24高8位为当前所选试块
                        // uint16_t probe;		//探头,波型D3D2横波/众波，直/斜/双晶/穿透d1d0,组合探头d6-d4
                        // d9d8核查用探头00组合探头01小于K1斜探头10单晶直探头11双晶直探头
                        // d11初查用d12核查用组合探头d13小于K1斜探头d14单晶直探头d15双晶直探头=0未测1已测,

        uint32_t option; // d31－d28当量标准:0000母线/从上到下，
                         // d27d26当量显示：%/dB值/孔径,d25d24标度：垂直\水平\距离
                         // d23-d16参量锁
                         // d15-d9参量步长,d15d14基准增益，d13d12调节增益，d11声程，d10延时，d9声速
                         // d8-d5参量类别，d8:基准增益\调节增益，d7:门A\门B，D6D5：门位\宽\高
                         // d4B门类型
                         // d2-d0曲线数目

        uint32_t status; // d31-d24仪器性能测试天数，D23-d18探头测试天数(零点、K值、DAC)，
                         // d17－d14灵敏度测试天数，d13-d7基准波高(0-128)
                         // d6-d0:d0:零点声速，D1：K值\折射角，D2：DAC,D3:AVG,
        // 3300+: start
        uint32_t externstatus; // d0 探测面：内侧/外侧
                               // d1 自动增益大小
                               // d2 报告格式 d3 模拟或数字探伤(0数字，1模拟),d4-d10基准高度(0-128),d11,12通用、标准,其他。
                               // d13,d14 大平底，圆柱面，空心轴
        // 3300+: start
        uint32_t prepare[4]; // 预留值

    }; // 每个通道的状态

    struct CHANNEL_PARAMETER_DAT {
        uint32_t Range;
        uint32_t OldRange; // 展宽前的声程
        uint32_t OldDelay; // 展宽前的延时
        uint32_t Delay;

        uint32_t wavepara[4]; // 波形的四个参数(距离,水平,垂直,高度/位置,高度,位置,高度)

        uint16_t Thick;
        uint16_t Diameter; // 直径

        uint16_t Offset;
        uint16_t Speed;     /*试件声速：	m/s	*/
        uint16_t Angle;     /*折射角：60o	*/
        uint16_t Forward;   /*探头前沿长度	*/
        uint16_t Frequence; /*探头频率：2.5Mhz	*/

        uint16_t Reject;

        uint16_t SurfGain; /*表面补偿增益	*/
        uint16_t BaseGain; /*基本增益	*/

        uint16_t DepthGain; // 深度补偿值
        int16_t  CompGain;  /*补偿增益	*/

        std::array<int16_t, 6> lineGain; // 六条线偏移

        int16_t gatedB;
        int16_t depth; // 缺陷高度

        uint16_t Crystal_l; /*晶片尺寸	*/
        uint16_t Crystal_w; /*晶片尺寸	*/

        // 3300+: start
        uint16_t PoreLong;     // 横孔长度
        uint16_t PoreDiameter; // 横孔直径
        uint16_t PoreDepth[2]; // 横孔孔深
        // 3300+: end
        uint32_t prepare[4]; // 预留值

    }; /*15*2BYTES*/

    struct GATE_PARA_DAT {
        uint16_t Position;
        uint16_t Width;
        uint16_t Height;
        uint8_t  Type;
        uint8_t  AlarmEnable;
    };

    typedef struct
    {
        std::array<uint16_t, 10> db;
        std::array<uint16_t, 10> dist;
        int16_t                  num;
        uint16_t                 diameter; // 反射体直径、长度
        uint16_t                 length;
    } DAC_DAT;

    struct WELD_PARA_DAT {
        uint32_t type   :3;  // 类型0无，1单面，2双面，3T型
        uint32_t face   :3;  // 探测面0A面，1B面，2C面
        uint32_t thick1 :14; // 厚度1，工件厚度、翼板厚度<200.0mm;
        uint32_t thick2 :14; // 厚度2，有效厚度、腹板厚度<200.0mm;
        uint32_t wide   :10; // 焊缝宽度、A面焊脚，< 100mm;
        uint32_t radius :10; // 根部半径、B面焊脚，< 100mm;
        uint32_t high1  :7;  // 上端余高、A端余高，<100mm;
        uint32_t high2  :7;  // 下端余高、B端余高，<100mm;
        uint32_t angle1 :11; // 坡口角度、A坡口角，< 180;
        uint32_t angle2 :10; // 坡口面角、B坡口角，< 90;
        uint32_t deep   :10; // 坡口深度，< 100mm;
        uint32_t blunt  :10; // 钝边高度，< 100mm;
        uint32_t space  :10; // 根部间隙，< 100mm
        uint32_t probe  :14;

        uint32_t angle3 :10; // 坡内角度，< 180;
        uint32_t angle4 :10; // 坡内面角，< 180;
        uint32_t deep3  :10; // 坡内深度，< 100mm;
    };

    struct __DATHead;

    struct __DATType {
        std::vector<uint8_t>       ascan_data;
        std::array<uint32_t, 4>    wave_para;
        std::shared_ptr<__DATHead> head;
    };
#pragma pack()

    struct __DATHead {
#pragma pack(1)
        HEADER_TIMESTAMP_DAT         header_timestamp;
        SYSTEM_STATUS_DAT            system_status;
        CHANNEL_STATUS_DAT           channel_status;
        CHANNEL_PARAMETER_DAT        channel_param;
        std::array<GATE_PARA_DAT, 2> gate_param;
        DAC_DAT                      dac;
        WELD_PARA_DAT                weld_param;
#pragma pack()
        std::vector<uint16_t> info_buf;
    };

    inline static constexpr auto ASCAN_DATA_SIZE = 500;
    inline static constexpr auto DAT_HEAD_SIZE   = sizeof(HEADER_TIMESTAMP_DAT) + sizeof(SYSTEM_STATUS_DAT) + sizeof(CHANNEL_STATUS_DAT) +
                                                 sizeof(CHANNEL_PARAMETER_DAT) + sizeof(std::array<GATE_PARA_DAT, 2>) + sizeof(DAC_DAT) + sizeof(WELD_PARA_DAT);
    inline static constexpr auto ASCAN_FRAME_SIZE = 800;

    std::unique_ptr<Union::AScan::AScanIntf> _FromFile(const std::wstring& filename);

    class DATType : public Union::AScan::AScanIntf, public Union::__330::_330_DAC_C {
    private:
        using _My_T = std::vector<std::vector<__DATType>>;

        _My_T                                   m_data           = {};
        int                                     m_fileName_index = {};
        std::string                             m_date           = {};
        QString                                 m_fileName       = {};
        mutable std::array<QVector<QPointF>, 3> m_dacLines       = {};

        friend Union::__390::HFDATType;

    public:
        void setDate(const std::string& date);

        friend std::unique_ptr<Union::AScan::AScanIntf> _FromFile(const std::wstring& fileName);
        static std::unique_ptr<Union::AScan::AScanIntf> FromFile(const std::wstring& fileName);

        virtual size_t __Read(std::ifstream& file, size_t file_size) final;
        virtual int    getDataSize(void) const final;

        virtual std::vector<std::wstring> getFileNameList(void) const override final;
        virtual void                      setFileNameIndex(int index) override final;

        // IMPL
        Base::Performance getPerformance(int idx) const override final;
        std::string       getDate(int idx) const override final;
        std::wstring      getProbe(int idx) const override final;
        double            getProbeFrequence(int idx) const override final;
        std::string       getProbeChipShape(int idx) const override final;
        double            getAngle(int idx) const override final;
        double            getSoundVelocity(int idx) const override final;
        double            getFrontDistance(int idx) const override final;
        double            getZeroPointBias(int idx) const override final;
        double            getSamplingDelay(int idx) const override final;
        int               getChannel(int idx) const override final;
        std::string       getInstrumentName(void) const override;

        std::array<Base::Gate, 2>   getGate(int idx = 0) const override final;
        const std::vector<uint8_t>& getScanData(int idx = 0) const override final;
        double                      getAxisBias(int idx = 0) const override final;
        double                      getAxisLen(int idx = 0) const override final;
        double                      getBaseGain(int idx = 0) const override final;
        double                      getScanGain(int idx = 0) const override final;
        double                      getSurfaceCompentationGain(int idx = 0) const override final;
        double                      getSupression(int idx = 0) const override final;
        Union::AScan::DistanceMode  getDistanceMode(int idx = 0) const override final;
        std::optional<Base::AVG>    getAVG(int idx = 0) const override final;
        std::optional<Base::DAC>    getDAC(int idx = 0) const override final;
        Union::AScan::DAC_Standard  getDACStandard(int idx = 0) const override final;

        virtual QJsonArray createGateValue(int idx, double soft_gain) const override;

        virtual const std::array<QVector<QPointF>, 3>& unResolvedGetDacLines(int idx) const override;

        virtual void setUnResolvedGetDacLines(const std::array<QVector<QPointF>, 3>& dat, int idx) override;

        virtual std::pair<double, double> getProbeSize(int idx) const override;

        virtual int getReplayTimerInterval() const override final;

    private:
        int              getOption(int idx) const noexcept;
        double           getUnit(int idx) const noexcept;
        const __DATHead& getHead(int idx) const;
        uint8_t          convertDB2GateAMP(int idx, int db) const;
        void             setFileName(const QString& fileName);
        const QString&   getFileName();
    };
} // namespace Union::__330
