#pragma once

#include "../AScanType.hpp"
#include <QtCore>
#include <Yo/File>
#include <array>
#include <chrono>
#include <cstdint>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

namespace Union::__390N_T8 {

    constexpr int ECHO_PACKAGE_SIZE = 520;
    constexpr int GATE_PEAK_SIZE    = 330;
    constexpr int SCREEN_WIDTH      = 520;

    using ARR_10_S = std::array<uint16_t, 10>;
    using ARR_3_S  = std::array<uint16_t, 3>;
    using ARR_2_S  = std::array<uint16_t, 2>;

    struct PARAM_DAC_DAT {
        uint8_t  ch_already_dac;
        uint16_t ch_dac_base_gain;
        uint8_t  ch_dac_gate;
        uint8_t  ch_dac_is_subline;
        ARR_10_S ch_dac_sample_index;
        uint16_t ch_dac_sample_length;
        ARR_10_S ch_dac_sample_value;
        uint16_t ch_dac_sampling_depth;
        uint16_t ch_dac_sampling_factor;
    };
    constexpr int DAC_DAT_LEN = sizeof(PARAM_DAC_DAT);

    struct PARAM_AVG_DAT {
        uint8_t  ch_already_avg;
        uint16_t ch_avg_base_gain;
        uint16_t ch_avg_dangliang;
        uint16_t ch_avg_diameter;
        uint16_t ch_avg_scan_gain;
        uint8_t  ch_avg_is_subline;
        uint16_t ch_avg_offset_gain;
        uint16_t ch_avg_reflector_diameter;
        uint16_t ch_avg_reflector_max_depth;
        ARR_10_S ch_avg_sample_index;
        uint8_t  ch_avg_sample_length;
        ARR_10_S ch_avg_sample_value;
        uint8_t  ch_avg_sampling_depth;
        uint16_t ch_avg_sampling_factor;
    };
    constexpr int AVG_DAT_LEN = sizeof(PARAM_AVG_DAT);

    struct PARAM_CHANEL_DAT {
        uint16_t ch_id;
        uint16_t ch_base_gain;
        uint16_t ch_scan_gain;
        uint16_t ch_offset_gain;
        uint16_t ch_color_select;
        uint8_t  ch_demodu_mode;
        uint8_t  ch_denoise_level;        // 平均次数
        uint8_t  ch_depth_suppression;    // 是否深度补偿
        uint8_t  ch_equivalent_standard;  // 判定标准
        uint8_t  ch_filter_band;          // 频带宽度  {"低频", "中频", "高频", "宽频"};
        uint16_t ch_flaw_actual_dist;     // 声程
        uint8_t  ch_flaw_amplitude;       // 波高
        uint8_t  ch_flaw_amplitude_b;     // b门波高
        uint16_t ch_flaw_depth;           // 垂直距离
        uint16_t ch_flaw_depth_b;         // b门垂直距离
        int16_t  ch_flaw_equivalent;      // SL DAC的db
        uint16_t ch_flaw_horizontal_dist; // 水平距离
        uint8_t  ch_gate_widen;           // 是否门内展宽
        uint16_t ch_gatea_height;         // A门高
        uint16_t ch_gatea_pos;            // A门位置
        uint16_t ch_gatea_width;          // A门宽
        uint16_t ch_gateb_height;         // B门高
        uint16_t ch_gateb_pos;            // B门位置
        uint16_t ch_gateb_width;          // B门宽
        uint8_t  ch_gateb_type;           //
        uint16_t ch_high_voltage;         // 输出电压
        uint16_t ch_impedance;            // 探头阻尼
        ARR_2_S  ch_k_value;              // 角度
        uint16_t ch_outside_radius;       // 曲面修正外径
        uint8_t  ch_peak_memory;          // 峰值记忆
        uint16_t ch_probe_freq;           // 探头频率
        uint32_t ch_probe_frontier;       // 探头前沿
        ARR_2_S  ch_probe_size;           // 晶片尺寸
        uint16_t ch_probe_type;           // 探头类型
        uint32_t ch_sampling_delay;       // 零点
        uint8_t  ch_sampling_factor;      // 因子

        uint8_t  ch_select_canliang;     // 参量颜色
        uint8_t  ch_select_ding;         // 定量曲线
        uint8_t  ch_select_even;         // 偶次
        uint8_t  ch_select_odd;          // 奇次
        uint8_t  ch_select_feng;         // 峰值包络
        uint8_t  ch_select_gatea;        // 波门A
        uint8_t  ch_select_gateb;        // 波门B
        uint8_t  ch_select_ground;       // 背景颜色
        uint8_t  ch_select_menu;         // 菜单颜色
        uint8_t  ch_select_wave;         // 波颜色
        ARR_3_S  ch_sound_distance;      // 量程
        uint8_t  ch_sound_distance_type; // l'c量程类型
        uint16_t ch_sound_speed;         // 声速
        uint8_t  ch_standard;            // 标准类型
        uint16_t ch_suppression;         // 抑制
        uint8_t  ch_surf_correction;     // 是否曲面修正

        uint16_t ch_thickness;    // 厚度
        uint8_t  ch_wave_envelop; // 全包包络
        uint16_t ch_work_freq;    // 重复频率
        uint8_t  ch_work_mode;    // 工作方式,一发一收或常规超声
        int16_t  ch_xuan_avg1;
        int16_t  ch_xuan_avg2;
        int16_t  ch_xuan_avg3;
        int16_t  ch_xuan_el;
        int16_t  ch_xuan_rl;
        int16_t  ch_xuan_sl;
        uint8_t  ch_xuan_buse;    // B门用途
        uint8_t  ch_xuan_hanfeng; //{"无", "单面","双面","T型"}; //焊缝图示
        uint32_t ch_xuan_waijing; // 工件外径
        uint32_t ch_yangshi;      // 延时
        uint32_t ch_zero_point;   // 零点

        // 五大性能
        uint16_t ch_yiqi_chuzhi;  // 垂直线性
        uint16_t ch_yiqi_dongtai; // 动态范围
        uint16_t ch_yiqi_fbl;     // 分辨力
        uint16_t ch_yiqi_lmd;     // 灵敏度余量
        uint16_t ch_yiqi_shuipin; // 水平线性

        uint16_t ch_biaozun_num;     // 标准编号
        uint16_t ch_biaozun_gengshu; // 标准根数
        uint16_t ch_biaozun_shikuai; // 标准试块
        uint16_t ch_biaozun_houdu;   // 标准厚度
        int16_t  ch_biaozun_rl;      // 标准rl
        int16_t  ch_biaozun_sl;      // 标准sl
        int16_t  ch_biaozun_el;      // 标准el
        int16_t  ch_biaozun_xian4;   // 标准xian4
        int16_t  ch_biaozun_xian5;   // 标准xian5
        int16_t  ch_biaozun_xian6;   // 标准xian6
    };
    constexpr int CHANEL_DAT_LEN = sizeof(PARAM_CHANEL_DAT);

    constexpr int InfoLen    = DAC_DAT_LEN + AVG_DAT_LEN + CHANEL_DAT_LEN;
    constexpr int EncoderLen = ECHO_PACKAGE_SIZE + GATE_PEAK_SIZE;

    struct _ldat {
        std::vector<uint8_t> AScan       = {};
        PARAM_DAC_DAT        dac_data    = {};
        PARAM_AVG_DAT        avg_data    = {};
        PARAM_CHANEL_DAT     chanel_data = {};
    };

    constexpr int HEAD_LEN = 22;
    constexpr int BODY_LEN = EncoderLen + 8;

    class LDAT : public Union::AScan::AScanIntf {
        friend class VDATA;

    private:
        std::string time = []() -> std::string {
            auto const        now = std::chrono::system_clock::now();
            std::time_t       t   = std::chrono::system_clock::to_time_t(now);
            std::stringstream ss;
            tm                t_m = {};
            localtime_s(&t_m, &t);
            ss << std::put_time(&t_m, "%Y-%m-%d %H:%M:%S");
            return ss.str();
        }();

        std::vector<_ldat> ldat = {};

        std::vector<std::wstring> m_fileNameList = {};

    protected:
        std::string convertTime(std::array<char, 14> arr) const;

    public:
        static std::unique_ptr<Union::AScan::AScanIntf> FromFile(const std::wstring& fileName);

        // READ IMPL
        size_t __Read(std::ifstream& file, size_t file_size) override;
        int    getDataSize(void) const override;

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

        std::array<Base::Gate, 2>   getGate(int idx) const override;
        const std::vector<uint8_t>& getScanData(int idx) const override;
        double                      getAxisBias(int idx) const override;
        double                      getAxisLen(int idx) const override;
        double                      getBaseGain(int idx) const override;
        double                      getScanGain(int idx) const override;
        double                      getSurfaceCompentationGain(int idx) const override;
        int                         getSupression(int idx) const override;
        Union::AScan::DistanceMode  getDistanceMode(int idx) const override;
        std::optional<Base::AVG>    getAVG(int idx) const override;
        std::optional<Base::DAC>    getDAC(int idx) const override;
        Union::AScan::DAC_Standard  getDACStandard(int idx) const override;

        std::function<double(double)> getAVGLineExpr(int idx) const override;
        std::function<double(double)> getDACLineExpr(int idx) const override;

        void pushFileNameList(const std::wstring& fileName);

        virtual QJsonArray createGateValue(int idx, double soft_gain) const override final;
    };

} // namespace Union::__390N_T8
