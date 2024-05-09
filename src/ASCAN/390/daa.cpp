#include "daa.hpp"
#include "../330/_330_draw_dac.hpp"
#include <QString>
#include <Yo/File>
#include <Yo/Types>
#include <cstdio>
#include <numbers>

namespace Yo::File {
    template <>
    size_t __Read(std::ifstream &file, Union::__390::DAAType &data, [[maybe_unused]] size_t file_size) {
        return data.__Read(file, file_size);
    }

} // namespace Yo::File

namespace Union::__390 {
    std::unique_ptr<Union::AScan::AScanIntf> DAAType::FromFile(const std::wstring &fileName) {
        auto ret = std::make_unique<DAAType>();
        auto res = Yo::File::ReadFile(fileName, *(ret.get()));
        if (res) {
            auto                                  _raw = dynamic_cast<DAAType *>(ret.get());
            Union::Temp::Unresovled::DrawDacParam _temp;
            _temp.m_unit         = _raw->systemStatus.unit;
            _temp.m_sys          = _raw->systemStatus.sys;
            _temp.m_ch_status    = _raw->channelStatus.status;
            _temp.m_ch_option    = _raw->channelStatus.option;
            _temp.m_ch_sys       = _raw->channelStatus.status;
            _temp.m_ch_Range     = _raw->channelParam.range;
            _temp.m_ch_Delay     = _raw->channelParam.delay;
            _temp.m_ch_Speed     = _raw->channelParam.speed;
            _temp.m_ch_Crystal_w = _raw->channelParam.crystal_w;
            _temp.m_ch_Crystal_l = _raw->channelParam.crystal_l;
            _temp.m_ch_Frequence = _raw->channelParam.freqence;
            _temp.m_ch_lineGain  = _raw->channelParam.lineGain;
            _temp.m_ch_BaseGain  = _raw->channelParam.baseGain;
            _temp.m_ch_gatedB    = _raw->channelParam.gateDB;
            _temp.m_dac_db       = _raw->dacParam.dB;
            _temp.m_dac_dist     = _raw->dacParam.dist;
            _temp.m_dac_num      = _raw->dacParam.num;
            _temp.m_range_a      = _raw->getAxisLen(0);
            _raw->setUnResolvedGetDacLines(DrawDac(_temp), 0);
            QFileInfo info(QString::fromStdWString(fileName));
            _raw->m_fileNameList.push_back(info.baseName().toStdWString());
            return ret;
        }
        return nullptr;
    }

    std::string DAAType::getInstrumentName(void) const {
        return "390 Single";
    }

    QVariantMap DAAType::createReportValueMap(int idx, double soft_gain) const {
        auto        ret          = Union::AScan::AScanIntf::createReportValueMap(idx, soft_gain);
        auto        date         = QDateTime::fromString(QString::fromStdString(getDate(idx)), "yyyy-MM-dd").toString("yyyy年MM月dd日");
        QVariantMap _390_special = {
            {"PXUT-390日期", date},
        };
        ret.insert(_390_special);
        return ret;
    }

} // namespace Union::__390
