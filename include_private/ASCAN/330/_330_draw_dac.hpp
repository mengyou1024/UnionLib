#pragma once
#include <QPointF>
#include <QVector>
#include <array>

namespace Union::Temp::Unresovled {
    // 这部分功能是从原来的程序里面移植过来的, 需要优化
    struct DrawDacParam {
        uint32_t                 m_unit;
        uint32_t                 m_sys;
        uint32_t                 m_ch_status;
        uint32_t                 m_ch_option;
        uint32_t                 m_ch_sys;
        uint32_t                 m_ch_Range;
        uint32_t                 m_ch_Delay;
        uint16_t                 m_ch_Speed;
        uint16_t                 m_ch_Crystal_w;
        uint16_t                 m_ch_Crystal_l;
        uint16_t                 m_ch_Frequence;
        std::array<int16_t, 6>   m_ch_lineGain;
        uint16_t                 m_ch_BaseGain;
        int16_t                  m_ch_gatedB;
        std::array<uint16_t, 10> m_dac_db;
        std::array<uint16_t, 10> m_dac_dist;
        int16_t                  m_dac_num;
        double                   m_range_a;
    };
    std::array<QVector<QPointF>, 3> DrawDac(const DrawDacParam &param);
} // namespace Union::Temp::Unresovled
