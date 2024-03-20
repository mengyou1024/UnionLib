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
        uint32_t             baseGain;
        uint32_t             range;
        uint32_t             frequency;
        int32_t              angle; ///< 一位小数点
        uint32_t             crystal;
        uint32_t             forward;
        uint32_t             offset;
        int32_t              line;
        int32_t              derect;
        std::vector<uint8_t> data;
    };

    struct Data {
        uint32_t               pulseMode;
        uint32_t               repeatRate;
        int32_t                scanMode;
        int32_t                coderType;
        float                  tofdFreq;
        float                  encValue;
        int32_t                thickness;
        int32_t                pcs;
        uint32_t               baseGain;
        uint32_t               range;
        uint32_t               delay;
        uint32_t               frequency; ///< 探头频率 一位小数点
        int32_t                angle;
        uint32_t               crystal;
        uint32_t               forward;
        uint32_t               offset;
        int32_t                line;
        int32_t                groove;
        int32_t                average;
        int32_t                width;
        int32_t                bandMode;
        uint32_t               flag;
        std::vector<uint8_t>   data;
        std::optional<SubData> subData;

        static std::optional<Data> FromFile(const std::wstring& fileName);
#if __has_include("QString")
        static std::optional<Data> FromFile(const QString& fileName);
#endif
    };
}; // namespace Union::__TOFD_PE
