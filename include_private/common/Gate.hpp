#pragma once

#include <array>
#include <optional>
#include <type_traits>
#include <vector>

namespace Union::Base {
    // 波门
    struct Gate {
        double pos    = {}; ///< 位置
        double width  = {}; ///< 宽度
        double height = {}; ///< 高度
        bool   enable = {}; ///< 是否启用
        bool   operator==(const Gate &rhs) const;
    };

    constexpr auto GATE_MAX_POS = 0;
    constexpr auto GATE_MAX_AMP = 1;
    using GateResult            = std::optional<std::tuple<double, uint8_t>>;

    /**
     * @brief 计算波门的结果
     *
     * @param data A扫数据
     * @param gate 波门数据
     * @param find_center_if_overflow 如果溢出是否寻找中心
     * @param supression 抑制
     * @param view_max 视图最高波大小(当传入抑制时有效)
     * @return GateResult
     */
    GateResult CalculateGateResult(
        const std::vector<uint8_t>  &data,
        const Gate                  &gate,
        bool                         find_center_if_overflow = false,
        const std::optional<double> &supression              = std::nullopt,
        uint8_t                      view_max                = 200);

} // namespace Union::Base
