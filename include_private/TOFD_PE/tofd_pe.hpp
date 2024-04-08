#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>
#if __has_include("QString")
    #include <QString>
#endif

namespace Union::__TOFD_PE {
    struct SubData {
        uint32_t             baseGain;  ///< 增益
        uint32_t             range;     ///< 声程范围
        uint32_t             frequency; ///< 探头频率
        int32_t              angle;     ///< 一位小数点
        uint16_t             crystal_w;
        uint16_t             crystal_l;
        uint32_t             forward; ///< 探头前沿
        uint32_t             offset;  ///< 零点
        int32_t              line;
        int32_t              derect;
        std::vector<uint8_t> data;
    };

    struct Data {
        uint32_t               pulseMode;  ///< 脉冲模式
        uint32_t               repeatRate; ///< 重复频率
        int32_t                scanMode;   ///< 扫查模式 0: 平行扫查 1: 非平行扫查
        int32_t                coderType;  ///< 编码器类型 0: 编码器 1: 事件
        float                  tofdFreq;   ///< TOFD频率
        float                  encValue;   ///<
        int32_t                thickness;  ///< 厚度
        int32_t                pcs;        ///<
        uint32_t               baseGain;   ///< 基本增益
        uint32_t               range;      ///< 声程
        uint32_t               delay;      ///< 延时
        uint32_t               frequency;  ///< 探头频率 一位小数点
        int32_t                angle;      ///< 角度
        uint16_t               crystal_w;
        uint16_t               crystal_l;
        uint32_t               forward;   ///< 前沿
        uint32_t               offset;    ///< 零点
        int32_t                line;      ///< 曲线条数
        int32_t                groove;    ///< 坡口形式
        int32_t                average;   ///< 平均次数
        int32_t                weldWidth; ///< 焊缝宽度
        int32_t                bandMode;  ///< 滤波频带
        uint32_t               flag;
        std::vector<uint8_t>   data;
        std::optional<SubData> subData;

        static std::optional<Data> FromFile(const std::wstring& fileName);
#if __has_include("QString")
        static std::optional<Data> FromFile(const QString& fileName);
#endif
    };
}; // namespace Union::__TOFD_PE
