#include "_330_draw_dac.hpp"
#include "dat330_single.h"

namespace Union::Temp::Unresovled {

    std::array<QVector<QPointF>, 3> DrawDac(const DrawDacParam &param) {
        dat330_single _temp;
        _temp.m_unit         = param.m_unit;
        _temp.m_sys          = param.m_sys;
        _temp.m_ch_status    = param.m_ch_status;
        _temp.m_ch_option    = param.m_ch_option;
        _temp.m_ch_sys       = param.m_ch_sys;
        _temp.m_ch_Range     = param.m_ch_Range;
        _temp.m_ch_Delay     = param.m_ch_Delay;
        _temp.m_ch_Speed     = param.m_ch_Speed;
        _temp.m_ch_Crystal_w = param.m_ch_Crystal_w;
        _temp.m_ch_Crystal_l = param.m_ch_Crystal_l;
        _temp.m_ch_Frequence = param.m_ch_Frequence;
        _temp.m_ch_lineGain  = param.m_ch_lineGain;
        _temp.m_ch_BaseGain  = param.m_ch_BaseGain;
        _temp.m_ch_gatedB    = param.m_ch_gatedB;
        _temp.m_dac_db       = param.m_dac_db;
        _temp.m_dac_dist     = param.m_dac_dist;
        _temp.m_dac_num      = param.m_dac_num;
        _temp.m_range_a      = param.m_range_a;
        return _temp.DrawDac();
    }

} // namespace Union::Temp::Unresovled
