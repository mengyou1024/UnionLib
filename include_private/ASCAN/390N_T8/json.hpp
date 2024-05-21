#pragma once

#include "../AScanType.hpp"
#include <string_view>

namespace Union::__390N_T8 {
    constexpr std::string_view CHANNEL_SCAN_VALUE = "ch_scan_value";
    // 通道通用参数
    constexpr std::string_view CHANNEL_PULSE_WIDTH     = "ch_pulse_width";
    constexpr std::string_view CHANNEL_SAMPLING_DELAY  = "ch_sampling_delay";
    constexpr std::string_view CHANNEL_SAMPLING_DEPTH  = "ch_sampling_depth";
    constexpr std::string_view CHANNEL_SAMPLING_FACTOR = "ch_sampling_factor";
    constexpr std::string_view CHANNEL_GAIN_VGA        = "ch_gain_vga";
    constexpr std::string_view CHANNEL_FILTER_BAND     = "ch_filter_band";
    constexpr std::string_view CHANNEL_DEMODU_SELECT   = "ch_demodu_mode";
    constexpr std::string_view CHANNEL_PHASE_REVERSE   = "ch_phase_reverse";
    constexpr std::string_view CHANNEL_BASE_GAIN       = "ch_base_gain";
    constexpr std::string_view CHANNEL_SCAN_GAIN       = "ch_scan_gain";
    constexpr std::string_view CHANNEL_OFFSET_GAIN     = "ch_offset_gain";
    constexpr std::string_view CHANNEL_SOUNDDIST_TYPE  = "ch_sound_distance_type";
    constexpr std::string_view CHANNEL_SOUNDDIST       = "ch_sound_distance";
    // 通道A门及B门参数
    constexpr std::string_view CHANNEL_GATEA_ACTIVITY = "ch_gatea_activity";
    constexpr std::string_view CHANNEL_GATEA_ALARM    = "ch_gatea_alarm";
    constexpr std::string_view CHANNEL_GATEA_POS      = "ch_gatea_pos";
    constexpr std::string_view CHANNEL_GATEA_WIDTH    = "ch_gatea_width";
    constexpr std::string_view CHANNEL_GATEA_HEIGHT   = "ch_gatea_height";
    constexpr std::string_view CHANNEL_GATEB_TYPE     = "ch_gateb_type";
    constexpr std::string_view CHANNEL_GATEB_ACTIVITY = "ch_gateb_activity";
    constexpr std::string_view CHANNEL_GATEB_ALARM    = "ch_gateb_alarm";
    constexpr std::string_view CHANNEL_GATEB_POS      = "ch_gateb_pos";
    constexpr std::string_view CHANNEL_GATEB_WIDTH    = "ch_gateb_width";
    constexpr std::string_view CHANNEL_GATEB_HEIGHT   = "ch_gateb_height";

    // 系统通用参数
    constexpr std::string_view SYS_CHANNEL_ID     = "sys_channel_id";
    constexpr std::string_view SYS_SCAN_INCREMENT = "sys_scan_increment";
    constexpr std::string_view SYS_ENCODER_CLEAR  = "sys_encoder_clear";
    constexpr std::string_view SYS_SYSTEM_LED     = "sys_system_led";

    // 仪器设置
    constexpr std::string_view CHANNEL_WORK_MODE    = "ch_work_mode";
    constexpr std::string_view CHANNEL_IMPEDANCE    = "ch_impedance";
    constexpr std::string_view CHANNEL_SUPPRESSION  = "ch_suppression"; // 抑制
    constexpr std::string_view CHANNEL_WORK_FREQ    = "ch_work_freq";
    constexpr std::string_view CHANNEL_HIGH_VOLTAGE = "ch_high_voltage";

    // 探伤设置
    constexpr std::string_view CHANNEL_SOUND_SPEED       = "ch_sound_speed";
    constexpr std::string_view CHANNEL_THICKNESS         = "ch_thickness";
    constexpr std::string_view CHANNEL_GATEB_FUNC        = "ch_gateb_func";
    constexpr std::string_view CHANNEL_DENOISE_LEVEL     = "ch_denoise_level";
    constexpr std::string_view CHANNEL_SURF_COMPENSATION = "ch_surf_compensation";
    constexpr std::string_view CHANNEL_GAIN_MODE         = "ch_gain_mode";
    constexpr std::string_view CHANNEL_STANDARD_HEIGHT   = "ch_standard_height";

    // 探头设置
    constexpr std::string_view CHANNEL_PROBE_TYPE     = "ch_probe_type";
    constexpr std::string_view CHANNEL_PROBE_FREQ     = "ch_probe_freq";
    constexpr std::string_view CHANNEL_PROBE_SIZE     = "ch_probe_size";
    constexpr std::string_view CHANNEL_K_VALUE        = "ch_k_value";
    constexpr std::string_view CHANNEL_PROBE_FRONTIER = "ch_probe_frontier";

    // 标准设置
    constexpr std::string_view CHANNEL_STANDARD            = "ch_standard";
    constexpr std::string_view CHANNEL_BLOCK_TYPE          = "ch_block_type";
    constexpr std::string_view CHANNEL_EQUIVALENT_STANDARD = "ch_equivalent_standard";

    // 功能项
    constexpr std::string_view CHANNEL_GATE_ALARM        = "ch_gate_alarm";
    constexpr std::string_view CHANNEL_PEAK_MEMORY       = "ch_peak_memory";
    constexpr std::string_view CHANNEL_WAVE_ENVELOPE     = "ch_wave_envelop";
    constexpr std::string_view CHANNEL_DAC_GATE          = "ch_dac_gate";
    constexpr std::string_view CHANNEL_DEPTH_SUPPRESSION = "ch_depth_suppression";
    constexpr std::string_view CHANNEL_GATE_WIDEN        = "ch_gate_widen";
    constexpr std::string_view CHANNEL_SURF_CORRECTION   = "ch_surf_correction";
    constexpr std::string_view CHANNEL_OUTSIDE_RADIUS    = "ch_outside_radius";

    constexpr std::string_view CHANNEL_FIRST_ECHO          = "ch_first_echo";
    constexpr std::string_view CHANNEL_SECOND_ECHO         = "ch_second_echo";
    constexpr std::string_view CHANNEL_HORIZONTAL_DIST     = "ch_horizontal_dist";
    constexpr std::string_view CHANNEL_ZEROPOINT           = "ch_zero_point";
    constexpr std::string_view CHANNEL_REFLECTOR_DIAMETER  = "ch_reflector_diameter";
    constexpr std::string_view CHANNEL_REFLECTOR_DEPTH     = "ch_reflector_depth";
    constexpr std::string_view CHANNEL_REFLECTOR_LENGTH    = "ch_reflector_length";
    constexpr std::string_view CHANNEL_REFLECTOR_MAX_DEPTH = "ch_reflector_max_depth";
    // avg
    constexpr std::string_view CHANNEL_AVG_REFLECTOR_MAX_DEPTH = "ch_avg_reflector_max_depth";
    constexpr std::string_view CHANNEL_AVG_REFLECTOR_DIAMETER  = "ch_avg_reflector_diameter";
    constexpr std::string_view CHANNEL_AVG_DANGLIANG           = "ch_avg_dangliang";
    constexpr std::string_view CHANNEL_AVG_BASE_GAIN           = "ch_avg_base_gain";
    constexpr std::string_view CHANNEL_AVG_SCAN_GAIN           = "ch_avg_scan_gain";
    constexpr std::string_view CHANNEL_AVG_OFFSET_GAIN         = "ch_avg_offset_gain";
    constexpr std::string_view CHANNEL_AVG_DIAMETER            = "ch_avg_diameter";

    constexpr std::string_view CHANNEL_ANGLE_REFLECTOR_DIAMETER = "ch_angle_reflector_diameter";

    // 仪器性能测试
    constexpr std::string_view CHANNEL_YIQI_SHUIPIN = "ch_yiqi_shuipin"; // 水平线性
    constexpr std::string_view CHANNEL_YIQI_CHUIZHI = "ch_yiqi_chuzhi";  // 垂直线性
    constexpr std::string_view CHANNEL_YIQI_FBL     = "ch_yiqi_fbl";     // 分辨力
    constexpr std::string_view CHANNEL_YIQI_DONGTAI = "ch_yiqi_dongtai"; // 动态范围
    constexpr std::string_view CHANNEL_YIQI_LMD     = "ch_yiqi_lmd";     // 灵敏度余量

    // 缺陷位置
    constexpr std::string_view CHANNEL_FLAW_ACTUAL_DIST     = "ch_flaw_actual_dist";
    constexpr std::string_view CHANNEL_FLAW_HORIZONTAL_DIST = "ch_flaw_horizontal_dist";
    constexpr std::string_view CHANNEL_FLAW_DEPTH           = "ch_flaw_depth";
    constexpr std::string_view CHANNEL_FLAW_AMPLITUDE       = "ch_flaw_amplitude";
    constexpr std::string_view CHANNEL_FLAW_EQUIVALENT      = "ch_flaw_equivalent";

    constexpr std::string_view CHANNEL_FLAW_ACTUAL_DIST_B     = "ch_flaw_actual_dist_b";
    constexpr std::string_view CHANNEL_FLAW_HORIZONTAL_DIST_B = "ch_flaw_horizontal_dist_b";
    constexpr std::string_view CHANNEL_FLAW_EQUIVALENT_B      = "ch_flaw_equivalent_b";
    constexpr std::string_view CHANNEL_FLAW_DEPTH_B           = "ch_flaw_depth_b";
    constexpr std::string_view CHANNEL_FLAW_AMPLITUDE_B       = "ch_flaw_amplitude_b";

    // DAC曲线采样点
    constexpr std::string_view CHANNEL_DAC_SAMPLE_VALUE    = "ch_dac_sample_value";
    constexpr std::string_view CHANNEL_DAC_SAMPLE_INDEX    = "ch_dac_sample_index";
    constexpr std::string_view CHANNEL_DAC_SAMPLE_LENGTH   = "ch_dac_sample_length";
    constexpr std::string_view CHANNEL_DAC_IS_SUBLINE      = "ch_dac_is_subline";
    constexpr std::string_view CHANNEL_ALREADY_DAC         = "ch_already_dac";
    constexpr std::string_view CHANNEL_DAC_SAMPLING_FACTOR = "ch_dac_sampling_factor";
    constexpr std::string_view CHANNEL_DAC_SAMPLE_DEPTH    = "ch_dac_sampling_depth";
    constexpr std::string_view CHANNEL_DAC_BASE_GAIN       = "ch_dac_base_gain";

    // AVG曲线采样点
    constexpr std::string_view CHANNEL_AVG_SAMPLE_VALUE    = "ch_avg_sample_value";
    constexpr std::string_view CHANNEL_AVG_SAMPLE_INDEX    = "ch_avg_sample_index";
    constexpr std::string_view CHANNEL_AVG_SAMPLE_LENGTH   = "ch_avg_sample_length";
    constexpr std::string_view CHANNEL_AVG_IS_SUBLINE      = "ch_avg_is_subline";
    constexpr std::string_view CHANNEL_ALREADY_AVG         = "ch_already_avg";
    constexpr std::string_view CHANNEL_AVG_SAMPLING_FACTOR = "ch_avg_sampling_factor";
    constexpr std::string_view CHANNEL_AVG_SAMPLE_DEPTH    = "ch_avg_sampling_depth";

    // 选项菜单
    constexpr std::string_view CHANNEL_XUAN_BIAODU   = "ch_xuan_biaodu";   // 声程标度
    constexpr std::string_view CHANNEL_XUAN_WAIJING  = "ch_xuan_waijing";  // 工件外径
    constexpr std::string_view CHANNEL_XUAN_DANGLIAN = "ch_xuan_danglian"; // 当量标准
    constexpr std::string_view CHANNEL_XUAN_RL       = "ch_xuan_rl";       // 当量rl
    constexpr std::string_view CHANNEL_XUAN_SL       = "ch_xuan_sl";       // 当量sl
    constexpr std::string_view CHANNEL_XUAN_EL       = "ch_xuan_el";       // 当量el
    constexpr std::string_view CHANNEL_XUAN_GENGSHU  = "ch_xuan_gengshu";  // 根数

    constexpr std::string_view CHANNEL_XUAN_FANSHI     = "ch_xuan_fanshi";     // 记录方式
    constexpr std::string_view CHANNEL_XUAN_WEIZHI     = "ch_xuan_weizhi";     // 记录位置
    constexpr std::string_view CHANNEL_XUAN_WEIZHIXIAN = "ch_xuan_weizhixian"; // 位置显示
    constexpr std::string_view CHANNEL_XUAN_LANG       = "ch_xuan_lang";       // 语言
    constexpr std::string_view CHANNEL_XUAN_BUSE       = "ch_xuan_buse";       // B门用途
    constexpr std::string_view CHANNEL_XUAN_SHENG      = "ch_xuan_sheng";      // 按键声音
    constexpr std::string_view CHANNEL_XUAN_LDU        = "ch_xuan_ldu";        // 屏幕亮度
    constexpr std::string_view CHANNEL_XUAN_BOGAO      = "ch_xuan_bgao";       // 标准波高
    constexpr std::string_view CHANNEL_XUAN_HANFENG    = "ch_xuan_hanfeng";    // 焊缝图示
    constexpr std::string_view CHANNEL_XUAN_CHUNCHU    = "ch_xuan_chunchu";    // 存储频谱
    constexpr std::string_view CHANNEL_XUAN_AVG1       = "ch_xuan_avg1";       // 当量rl
    constexpr std::string_view CHANNEL_XUAN_AVG2       = "ch_xuan_avg2";       // 当量sl
    constexpr std::string_view CHANNEL_XUAN_AVG3       = "ch_xuan_avg3";       // 当量el

    constexpr std::string_view CHANNEL_BIAOZUN = "ch_biaozun"; // 标准类型
    constexpr std::string_view CHANNEL_YANGSHI = "ch_yangshi"; // 延时

    constexpr std::string_view CHANNEL_COLOR_SELECT    = "ch_color_select";    // 颜色
    constexpr std::string_view CHANNEL_HANFENG_MIAN    = "ch_hanfeng_mian";    // 探测面
    constexpr std::string_view CHANNEL_HANFENG_YXHOUDU = "ch_hanfeng_yxhoudu"; // 有效厚度
    constexpr std::string_view CHANNEL_HANFENG_WIDTH   = "ch_hanfeng_width";   // 焊缝宽度
    constexpr std::string_view CHANNEL_HANFENG_GENGBU  = "ch_hanfeng_gengbu";  // 根部半径
    constexpr std::string_view CHANNEL_HANFENG_SYUGAO  = "ch_hanfeng_syugao";  // 上端余高
    constexpr std::string_view CHANNEL_HANFENG_XYUGAO  = "ch_hanfeng_xyugao";  // 下端余高
    constexpr std::string_view CHANNEL_HANFENG_POKOU   = "ch_hanfeng_pokou";   // 坡口角度
    constexpr std::string_view CHANNEL_HANFENG_PKMJ    = "ch_hanfeng_pkmj";    // 坡口面角
    constexpr std::string_view CHANNEL_HANFENG_PKSD    = "ch_hanfeng_pksd";    // 坡口深度
    constexpr std::string_view CHANNEL_HANFENG_DBGD    = "ch_hanfeng_dbgd";    // 钝边高度
    constexpr std::string_view CHANNEL_HANFENG_GBJX    = "ch_hanfeng_gbjx";    // 根部间隙

    constexpr std::string_view CHANNEL_COLOR_WAVE     = "ch_select_wave";     // 波形
    constexpr std::string_view CHANNEL_COLOR_ODD      = "ch_select_odd";      // 寄次回波
    constexpr std::string_view CHANNEL_COLOR_EVEN     = "ch_select_even";     // 偶次回波
    constexpr std::string_view CHANNEL_COLOR_MENU     = "ch_select_menu";     // 提示菜单
    constexpr std::string_view CHANNEL_COLOR_DING     = "ch_select_ding";     // 定量曲线
    constexpr std::string_view CHANNEL_COLOR_CANLIANG = "ch_select_canliang"; // 坐标参量
    constexpr std::string_view CHANNEL_COLOR_GATEA    = "ch_select_gatea";    // 波门A
    constexpr std::string_view CHANNEL_COLOR_GATEB    = "ch_select_gateb";    // 波门B
    constexpr std::string_view CHANNEL_COLOR_FENG     = "ch_select_feng";     // 峰值包络

    // tofd
    constexpr std::string_view TOFD_PROBE_FREQ        = "tofd_probe_freq";
    constexpr std::string_view TOFD_PROBE_SIZE        = "tofd_probe_size";
    constexpr std::string_view TOFD_PROBE_ANGLE       = "tofd_probe_angle";
    constexpr std::string_view TOFD_ZEROPOINT         = "tofd_zero_point";
    constexpr std::string_view TOFD_PROBE_FRONTIER    = "tofd_probe_frontier";
    constexpr std::string_view TOFD_FILTER_BAND       = "tofd_filter_band"; // 滤波频带
    constexpr std::string_view TOFD_PCS               = "tofd_PCS";
    constexpr std::string_view TOFD_THICKNESS         = "tofd_thickness";
    constexpr std::string_view TOFD_SPEED             = "tofd_speed";
    constexpr std::string_view TOFD_HANFENG_WIDTH     = "tofd_hanfeng_width";  // 焊缝宽度
    constexpr std::string_view TOFD_POKOU_TYPE        = "tofd_pokou_type";     // 坡口形式
    constexpr std::string_view TOFD_HIGH_VOLTAGE      = "tofd_high_voltage";   // 发射电压
    constexpr std::string_view TOFD_WORK_FREQ         = "tofd_work_freq";      // 重复频率
    constexpr std::string_view TOFD_DEMODU_SELECT     = "tofd_demodu_mode";    // 检波方式
    constexpr std::string_view TOFD_WORK_MODE         = "tofd_work_mode";      // 工作方式  CHANNEL_WORK_MODE
    constexpr std::string_view TOFD_CUFA_MODE         = "tofd_cufa_mode";      // 触发方式
    constexpr std::string_view TOFD_FANGXIAN          = "tofd_fangxian";       // 扫查方向
    constexpr std::string_view TOFD_DENOISE_LEVEL     = "tofd_denoise_level";  // 平均次数
    constexpr std::string_view TOFD_SCAN_INCREMENT    = "tofd_scan_increment"; // 扫查增量
    constexpr std::string_view TOFD_SCAN_MODE         = "tofd_scan_mode";      // 扫查方式
    constexpr int              JSON_390N_T8_ASCAN_LEN = 520;

    class T8_390N_JSON : public Union::AScan::AScanIntf {
    public:
        explicit T8_390N_JSON(const std::wstring& fileName);

        virtual size_t __Read(std::ifstream& file, size_t file_size) override final;
        virtual int    getDataSize(void) const override final;

        virtual std::vector<std::wstring> getFileNameList(void) const override final;
        virtual void                      setFileNameIndex(int index) override final;

        virtual Base::Performance getPerformance(int idx) const override final;
        virtual std::string       getDate(int idx) const override final;
        virtual std::wstring      getProbe(int idx) const override final;
        virtual double            getProbeFrequence(int idx) const override final;
        virtual std::string       getProbeChipShape(int idx) const override final;
        virtual double            getAngle(int idx) const override final;
        virtual double            getSoundVelocity(int idx) const override final;
        virtual double            getFrontDistance(int idx) const override final;
        virtual double            getZeroPointBias(int idx) const override final;
        virtual double            getSamplingDelay(int idx) const override final;
        virtual int               getChannel(int idx) const override final;
        virtual std::string       getInstrumentName(void) const override final;

        virtual std::array<Base::Gate, 2>   getGate(int idx) const override final;
        virtual const std::vector<uint8_t>& getScanData(int idx) const override final;
        virtual double                      getAxisBias(int idx) const override final;
        virtual double                      getAxisLen(int idx) const override final;
        virtual double                      getBaseGain(int idx) const override final;
        virtual double                      getScanGain(int idx) const override final;
        virtual double                      getSurfaceCompentationGain(int idx) const override final;
        virtual int                         getSupression(int idx) const override final;
        virtual Union::AScan::DistanceMode  getDistanceMode(int idx) const override final;
        virtual std::optional<Base::AVG>    getAVG(int idx) const override final;
        virtual std::optional<Base::DAC>    getDAC(int idx) const override final;
        virtual Union::AScan::DAC_Standard  getDACStandard(int idx) const override final;

        static std::unique_ptr<Union::AScan::AScanIntf> FromFile(const std::wstring& file_name);

        virtual QJsonArray createGateValue(int idx, double soft_gain) const override final;

        virtual std::pair<double, double> getProbeSize(int idx) const override final;

    private:
        std::vector<std::wstring> m_fileNameList = {};
        struct AScanData {
            std::array<Base::Gate, 2>  gate                    = {}; ///< 波门
            std::vector<uint8_t>       ascan                   = {}; ///< A扫数据
            double                     axisBias                = {}; ///< 坐标轴偏移
            double                     axisLen                 = {}; ///< 坐标轴长度
            double                     baseGain                = {}; ///< 基本增益
            double                     scanGain                = {}; ///< 扫查增益
            double                     surfaceCompentationGain = {}; ///< 表面补偿增益
            int                        suppression             = {}; ///< 抑制
            Union::AScan::DistanceMode distanceMode            = {}; ///< 声程模式
            std::optional<Base::AVG>   avg                     = {}; ///< AVG曲线
            std::optional<Base::DAC>   dac                     = {}; ///< DAC曲线
            Union::AScan::DAC_Standard std                     = {}; ///< DAC标准
        };

        struct AScanType {
            std::vector<AScanData> data           = {};
            Base::Performance      performance    = {}; ///< 性能
            std::string            time           = {}; ///< 时间
            std::wstring           probe          = {}; ///< 探头
            double                 probeFrequence = {}; ///< 探头频率
            std::string            probeChipShape = {}; ///< 探头晶片尺寸
            double                 angle          = {}; ///< 角度
            double                 soundVelocity  = {}; ///< 声速
            double                 frontDistance  = {}; ///< 前沿
            double                 zeroPointBias  = {}; ///< 零点偏移
            double                 samplingDelay  = {}; ///< 采样延迟
            int                    channel        = {}; ///< 通道
            std::string            instrumentName = {}; ///< 仪器型号
        };

        std::array<QString, 2> m_equi = {"-", "-"};
        std::array<QString, 2> m_a    = {"-", "-"};
        std::array<QString, 2> m_b    = {"-", "-"};
        std::array<QString, 2> m_c    = {"-", "-"};
        std::pair<double, double> m_probeSize = {};

        std::optional<AScanType> __390N_T8_JSON_READ(const std::wstring& fileName);
        std::optional<AScanType> m_ascan = std::nullopt;
        ;
    };

} // namespace Union::__390N_T8
