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

    size_t DAAType::__Read(std::ifstream &file, size_t file_size) {
        if (file_size != 1242) {
            return 0;
        }
        size_t ret = 0;
        ret += Yo::File::__Read(file, Yo::File::MakeStructSub(&(systemStatus), &(channelTemp), sizeof(uint32_t)), file_size);
        data.resize(480);
        ret += Yo::File::__Read(file, data, file_size);
        name.resize(file_size - ret - SIZE_390_EXTRA);
        ret += Yo::File::__Read(file, name, file_size);
        ret += Yo::File::__Read(file, m_390Extra, file_size);
        return ret;
    }

    std::wstring DAAType::getProbe(int idx) const {
        (void)idx;
        static const std::array _probe = {
            L"直探头",
            L"斜探头",
            L"双晶探头",
            L"双探头",
        };
        return _probe.at(channelStatus.sys >> 12 & 0x07);
    }

    QVariantMap DAAType::createTechnologicalParameter(int idx) const {
        auto        ret   = AScanIntf::createTechnologicalParameter(idx);
        QVariantMap page1 = {
            {"焊头编号", m_390Extra.num11},
            {"里程(km)",   m_390Extra.num12},
            {"轨号",       m_390Extra.num13},
            {"线名",       getNum14()      },
            {"站名",       getStationName()},
            {"股别",       getStockType()  },
            {"轨型",       getRailType()   },
            {"线别",       getLineType()   },
            {"行别",       getFineType()   },
        };
        QVariantMap page2 = {
            {"焊型",       getWeldType()},
            {"道岔号",    getNum22()   },
            {"股道编号", getNum23()   },
            {"操作人员", getNum24()   },
            {"日期时间", getNum25()   },
        };
        QVariantMap page3 = {
            {"损伤位置", getNum31()},
            {"损伤大小", getNum32()},
            {"损伤类型", getNum33()},
            {"损伤图示", getNum34()},
            {"X坐标",      getNum35()},
            {"Y坐标",      getNum36()},
            {"Z坐标",      getNum37()},
        };
        ret.insert("钢轨焊缝页1", page1);
        ret.insert("钢轨焊缝页2", page2);
        ret.insert("钢轨焊缝页3", page3);
        auto baseInfo = ret["基本信息"].toMap();
        if (getChannel(idx) == 1) {
            baseInfo.insert("位置", getPosiFlag());
        } else {
            baseInfo.insert("位置", getTailPosiFlag());
        }
        baseInfo.insert("通道", getChannelName());
        ret.insert("基本信息", baseInfo);
        return ret;
    }

    std::string DAAType::getInstrumentName(void) const {
        return "390 Single";
    }

    QVariantMap DAAType::createReportValueMap(int idx, double soft_gain) const {
        auto    ret  = Union::AScan::AScanIntf::createReportValueMap(idx, soft_gain);
        auto    date = QString("%1年%2月%3日").arg(m_390Extra.yearTemp).arg(m_390Extra.monthTemp).arg(m_390Extra.dateTemp);
        QString _k_range;
        switch (getChannel(idx)) {
            case 1:
            case 2:
                _k_range = "K2.5~K2.5";
                break;
            case 3:
                _k_range = "K1~K1";
                break;
            case 4:
                _k_range = "K0~K0";
                break;
            case 5:
                _k_range = "K0.8~K1";
                break;
            default:
                _k_range = "error";
                break;
        }

        QVariantMap _390_special = {
            {"PXUT-390日期", date         },
            {"K范围",        _k_range     },
            {"线名",         getNum14()   },
            {"行别",         getFineType()},
        };
        ret.insert(_390_special);
        return ret;
    }

    std::string DAAType::getDate(int idx) const {
        (void)idx;
        return QString("%1-%2-%3").arg(m_390Extra.yearTemp).arg(m_390Extra.monthTemp).arg(m_390Extra.dateTemp).toStdString();
    }

    double DAAType::getDotX() const {
        return m_390Extra.num35;
    }

    double DAAType::getDotY() const {
        return m_390Extra.num36;
    }

    double DAAType::getDotZ() const {
        return m_390Extra.num37;
    }

    QString DAAType::getFineType() const {
        switch (m_390Extra.linetype) {
            case 1:
                return "上行";
            case 2:
                return "下行";
            case 3:
                return "单线";
            case 4:
                return "三线";
            case 5:
                return "四线";
            default:
                return "错误";
        }
    }

    QString DAAType::getWeldType() const {
        switch (m_390Extra.finetype) {
            case 1:
                return "现场闪光焊";
            case 2:
                return "工厂闪光焊";
            case 3:
                return "铝热焊";
            case 4:
                return "气压焊";
            case 5:
                return "电弧焊";
            case 6:
                return "尖轨";
            case 7:
                return "翼轨";
            case 8:
                return "长心轨";
            default:
                return "其他";
        }
    }

    QString DAAType::getNum14() const {
        std::vector<uint16_t> _data(m_390Extra.num14.begin() + 1, m_390Extra.num14.begin() + 1 + m_390Extra.num14[0]);
        return Union::LocationCodeToUTF8(_data);
    }

    QString DAAType::getNum22() const {
        return QString::number(m_390Extra.num22);
    }

    QString DAAType::getNum23() const {
        return QString::number(m_390Extra.num23);
    }

    QString DAAType::getNum24() const {
        std::vector<uint16_t> _data(m_390Extra.pnNumber24.begin() + 1, m_390Extra.pnNumber24.begin() + 1 + m_390Extra.pnNumber24[0]);
        return Union::LocationCodeToUTF8(_data);
    }

    QString DAAType::getNum25() const {
        return QString("%1-%2-%3 %4:%5").arg(m_390Extra.yearTemp).arg(m_390Extra.monthTemp).arg(m_390Extra.dateTemp).arg(m_390Extra.hourTemp).arg(m_390Extra.minuteTemp);
    }

    QString DAAType::getNum31() const {
        // 轨头 轨腰 轨底 轨底脚
        switch (m_390Extra.position) {
            case 1:
                return "轨头";
            case 2:
                return "轨腰";
            case 3:
                return "轨底";

            default:
                return "error";
        }
    }

    QString DAAType::getNum32() const {
        return QString::number(m_390Extra.num32);
    }

    QString DAAType::getNum33() const {
        // 无伤   重伤 轻伤
        switch (m_390Extra.size) {
            case 1:
                return "无伤";
            case 2:
                return "重伤";
            case 3:
                return "轻伤";
            default:
                return "error";
        }
    }

    QString DAAType::getNum34() const {
        switch (m_390Extra.type) {
            case 1:
                return "截面";
            case 2:
                return "侧面";
            default:
                return "error";
        }
    }

    QString DAAType::getNum35() const {
        if (m_390Extra.num35 > 0) {
            return "+" + QString::number(m_390Extra.num35);
        } else {
            return QString::number(m_390Extra.num35);
        }
    }

    QString DAAType::getNum36() const {
        if (m_390Extra.num36 > 0) {
            return "+" + QString::number(m_390Extra.num36);
        } else {
            return QString::number(m_390Extra.num36);
        }
    }

    QString DAAType::getNum37() const {
        if (m_390Extra.num37 > 0) {
            return "+" + QString::number(m_390Extra.num37);
        } else {
            return QString::number(m_390Extra.num37);
        }
    }

    QString DAAType::getPosiFlag() const {
        switch (m_390Extra.posiFlage) {
            case 0:
                return "正向";
            case 1:
                return "反向";
            default:
                return "error";
        }
    }

    QString DAAType::getTailPosiFlag() const {
        switch (m_390Extra.tailPosiFlage) {
            case 0:
                return "正向外径";
            case 1:
                return "反向外径";
            case 2:
                return "正向内径";
            case 3:
                return "反向内径";
            default:
                return "error";
        }
    }

    QString DAAType::getChannelName() const {
        switch (getChannel(0)) {
            case 1:
                return "K2.5轨头 " + getPosiFlag();
            case 2:
                return "K2.5轨底 " + getTailPosiFlag();
            case 3:
                return "K1.0";
            case 4:
                return "K0.0";
            case 5:
                return "K1.5";
            default:
                return "error";
        }
    }

    QString DAAType::getStationName() const {
        std::vector<uint16_t> _data(m_390Extra.pnNumber15.begin() + 1, m_390Extra.pnNumber15.begin() + 1 + m_390Extra.pnNumber15[0]);
        return Union::LocationCodeToUTF8(_data);
    }

    QString DAAType::getStockType() const {
        if (m_390Extra.stocktype == 1) {
            return "左股";
        } else {
            return "右股";
        }
    }

    QString DAAType::getRailType() const {
        switch (m_390Extra.railtype) {
            case 1:
                return "43";
            case 2:
                return "50";
            case 3:
                return "65";
            case 4:
                return "75";
            default:
                return "error";
        }
    }

    QString DAAType::getLineType() const {
        switch (m_390Extra.linetype) {
            case 1:
                return "正线";
            case 2:
                return "站线";
            case 3:
                return "到发线";
            default:
                return "error";
        }
    }

} // namespace Union::__390
