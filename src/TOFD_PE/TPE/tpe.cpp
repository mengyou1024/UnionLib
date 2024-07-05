#include "tpe.hpp"
#include "common/union_common.hpp"
#include <QLoggingCategory>
#include <Yo/File>
#include <stdexcept>

static Q_LOGGING_CATEGORY(TAG, "TOFD/PE.tpe");

namespace Yo::File {
    template <>
    size_t __Read(std::ifstream &file, Union::TOFD_PE::TPE::TpeType &data, [[maybe_unused]] size_t file_size) {
        return data.__Read(file, file_size);
    }

} // namespace Yo::File

namespace Union::TOFD_PE::TPE {
    std::unique_ptr<TofdPeIntf> TpeType::FromFile(const std::wstring &fileName) {
        auto ret = std::make_unique<TpeType>();
        if (!Yo::File::ReadFile(fileName, (*(ret.get())))) {
            return nullptr;
        }
        return ret;
    }

    size_t TpeType::__Read(std::ifstream &file, size_t file_size) {
        size_t ret = 0;
        ret += Yo::File::__Read(file, Yo::File::MakeStructSub(&(m_data.pulseMode), &(m_data.bandMode), sizeof(int32_t)), file_size);
        m_data.data.resize((m_data.line + 1) * getAScanSize());
        ret += Yo::File::__Read(file, m_data.data, file_size);
        ret += Yo::File::__Read(file, m_data.flag, file_size);

        if (m_data.flag == 2) {
            m_data.subData = SubData();
            ret += Yo::File::__Read(file, Yo::File::MakeStructSub(&(m_data.subData->baseGain), &(m_data.subData->derect), sizeof(int32_t)), file_size);
            m_data.subData->data.resize((m_data.subData->line + 1) * getAScanSize());
            ret += Yo::File::__Read(file, m_data.subData->data, file_size);
        }
        return ret;
    }

    int TpeType::getAScanSize(void) const {
        return 500;
    }

    int TpeType::getTofdLines(void) const {
        return m_data.line + 1;
    }

    const std::vector<uint8_t> &TpeType::getTofdData(void) const {
        return m_data.data;
    }

    double TpeType::getTofdDelay(void) const {
        if (!m_data.subData.has_value()) {
            return m_data.delay / 100.0;
        }
        return m_data.delay / 100.0 * 5.92 / 2.0;
    }

    double TpeType::getTofdRange(void) const {
        return m_data.range / 10.0;
    }

    bool TpeType::hasPe(void) const {
        return m_data.subData.has_value();
    }

    int TpeType::getPeLines(void) const {
        if (!hasPe()) {
            return -1;
        }
        return m_data.subData->line + 1;
    }

    const std::vector<uint8_t> &TpeType::getPeData(void) const {
        if (!hasPe()) {
            qCritical(TAG) << "No PE data";
            static std::vector<uint8_t> empty;
            return empty;
        }
        return m_data.subData->data;
    }

    double TpeType::getPeRange(void) const {
        if (!hasPe()) {
            return 0.0;
        }
        return m_data.subData->range / 300.0;
    }

    double TpeType::getPeDelay(void) const {
        return 0.0;
    }

    double TpeType::getPeAngle(void) const {
        if (!hasPe()) {
            return 0.0;
        }
        return m_data.subData->angle / 10.0;
    }

    void TpeType::removeThroughWaveEvent(double x, double y, double w, double h) {
        const auto maxLines = std::max(getTofdLines(), getPeLines());
        const auto start_x  = static_cast<int>(x * (maxLines - 1));
        const auto end_x    = static_cast<int>((x + w) * (maxLines - 1));
        const auto start_y  = static_cast<int>(y * static_cast<double>(getAScanSize() - 1.0));
        const auto end_y    = static_cast<int>((y + h) * static_cast<double>(getAScanSize() - 1.0));

        for (auto i = start_x + 1; std::cmp_less_equal(i, end_x); i++) {
            for (auto j = start_y; std::cmp_less_equal(j, end_y); j++) {
                m_data.data[i * getAScanSize() + j] += -m_data.data[start_x * getAScanSize() + j] + 128;
            }
        }
    }

    void TpeType::pullThroughWaveEvent(double x, double y, double w, double h) {
        const auto maxLines     = std::max(getTofdLines(), getPeLines());
        const auto start_x      = static_cast<int>(x * (maxLines - 1));
        const auto end_x        = static_cast<int>((x + w) * (maxLines - 1));
        const auto start_y      = static_cast<int>(y * static_cast<double>(getAScanSize() - 1.0));
        const auto end_y        = static_cast<int>((y + h) * static_cast<double>(getAScanSize() - 1.0));
        const auto CalculateMin = [=, this](int x, int start_y, int end_y) {
            int m_iStartPlot = start_y;
            int m_iEndPlot   = end_y;
            int z            = m_iStartPlot;
            if (m_iEndPlot - m_iStartPlot < 45) {
                for (int k = m_iStartPlot + 1; k <= m_iEndPlot; k++) {
                    if (m_data.data[x * getAScanSize() + k] < m_data.data[x * getAScanSize() + z]) {
                        z = k;
                    }
                }
            } else {
                for (int k = m_iStartPlot + 1; k < m_iStartPlot + 45; k++) {
                    if (m_data.data[x * getAScanSize() + k] < m_data.data[x * getAScanSize() + z]) {
                        z = k;
                    }
                }
            }
            if ((float(m_data.data[x * getAScanSize()] - m_data.data[x * getAScanSize() + z]) /
                 float(m_data.data[x * getAScanSize()])) < 0.1) {
                z = m_iStartPlot;
            }

            return z;
        };

        auto ascan_data_copy = m_data.data;

        auto startPlotIndex = CalculateMin(start_x, start_y, end_y);
        for (int i = start_x + 1; std::cmp_less_equal(i, end_x); i++) {
            auto tempPlotIndex           = CalculateMin(i, start_y, end_y);
            auto tempDataStraightenBegin = ascan_data_copy.begin() + (i * getAScanSize());
            auto tempDataStraightenEnd   = ascan_data_copy.begin() + (i * getAScanSize()) + getAScanSize();
            auto tempDataStraighten      = std::vector<uint8_t>(tempDataStraightenBegin, tempDataStraightenEnd);

            if (startPlotIndex > tempPlotIndex) {
                for (int m = 0; m < startPlotIndex - tempPlotIndex; m++) {
                    tempDataStraighten[m] = tempDataStraighten[0];
                }
                for (int m = startPlotIndex - tempPlotIndex; m < getAScanSize(); m++) {
                    ascan_data_copy[i * getAScanSize() + m] = tempDataStraighten[m - startPlotIndex + tempPlotIndex];
                }
            } else if (startPlotIndex < tempPlotIndex) {
                for (int m = 0; m < getAScanSize() + startPlotIndex - tempPlotIndex; m++) {
                    ascan_data_copy[i * getAScanSize() + m] = tempDataStraighten[m - startPlotIndex + tempPlotIndex];
                }
                for (int m = getAScanSize() + startPlotIndex - tempPlotIndex; m < getAScanSize(); m++) {
                    ascan_data_copy[i * getAScanSize() + m] = tempDataStraighten[0];
                }
            }
        }

        for (auto i = start_x; std::cmp_less_equal(i, end_x); i++) {
            for (auto j = start_y; std::cmp_less_equal(j, end_y); j++) {
                m_data.data[i * getAScanSize() + j] = ascan_data_copy[i * getAScanSize() + j];
            }
        }
    }

    void TpeType::backup(void) {
        if (!m_data_bak.has_value()) {
            m_data_bak = m_data;
        }
    }

    void TpeType::rollback(void) {
        if (m_data_bak.has_value()) {
            m_data = m_data_bak.value();
        }
    }

    const QJsonObject TpeType::getTofdParam(void) const {
        QJsonObject ret;
        ret.insert(QObject::tr("系统参数"), getTofdSystemParam());
        ret.insert(QObject::tr("工件情况"), getTofdWorkPieceParam());
        ret.insert(QObject::tr("通道参数 (类型TOFD 发射1 接收1)") + QString("[PCS:%1mm]").arg(QString::number(m_data.pcs / 10.0, 'f', 1)), getTofdChannelParam());
        return ret;
    }

    const QJsonObject TpeType::getPeParam(void) const {
        QJsonObject ret;
        ret.insert(QObject::tr("工件情况"), getPeWorkPieceParam());
        return ret;
    }

    const QJsonObject TpeType::getTofdSystemParam(void) const {
        QJsonObject ret;
        ret.insert(QObject::tr("发射电压"), getEmitVoltage());
        ret.insert(QObject::tr("重复频率"), QString("%1Hz").arg(getRepeatFreq()));
        ret.insert(QObject::tr("扫查方式"), getScanMode());
        ret.insert(QObject::tr("触发方式"), getTriggerMode());
        ret.insert(QObject::tr("扫查增量"), QString::number(getScanIncrement(), 'f', 2) + "mm");
        return ret;
    }

    const QJsonObject TpeType::getTofdWorkPieceParam(void) const {
        QJsonObject ret;
        ret.insert(QObject::tr("工件厚度"), QString::number(getThicknessOfWorkPiece(), 'f', 1) + "mm");
        ret.insert(QObject::tr("焊缝宽度"), QString::number(getTofdWeldWidth(), 'f', 1) + "mm");
        ret.insert(QObject::tr("坡口类型"), getGrooveType());
        return ret;
    }

    const QJsonObject TpeType::getTofdChannelParam(void) const {
        QJsonObject ret;
        ret.insert(QObject::tr("增益"), QString::number(getTofdGain(), 'f', 1) + "db");
        ret.insert(QObject::tr("声程"), QString::number(Union::KeepDecimals<1>(mm2us(getTofdRange())), 'f', 1) + "μs");
        ret.insert(QObject::tr("延时"), QString::number(Union::KeepDecimals<1>(mm2us(getTofdDelay())), 'f', 1) + "μs");
        ret.insert(QObject::tr("滤波频带"), getFilterBand());
        ret.insert(QObject::tr("检波方式"), getDetectionMode());
        ret.insert(QObject::tr("探头类型"), getTofdProbeType());
        ret.insert(QObject::tr("探头尺寸"), getTofdProbeSize());
        ret.insert(QObject::tr("探头频率"), QString::number(getTofdProbeFreq(), 'f', 1) + "MHz");
        ret.insert(QObject::tr("探头角度"), QString::number(getTofdAngle(), 'f', 1) + QObject::tr("度"));
        ret.insert(QObject::tr("探头前沿"), QString::number(getTofdProbeFront(), 'f', 1) + "mm");
        ret.insert(QObject::tr("零点"), QString::number(getTofdZeroPoint(), 'f', 2) + "μs");
        return ret;
    }

    const QJsonObject TpeType::getPeWorkPieceParam(void) const {
        QJsonObject ret;
        ret.insert(QObject::tr("增益"), QString::number(getPeGain(), 'f', 1) + "db");
        ret.insert(QObject::tr("声程"), QString::number(getPeRange(), 'f', 1) + "mm");
        ret.insert(QObject::tr("前沿"), QString::number(getPeProbeFront(), 'f', 1) + "mm");
        ret.insert(QObject::tr("零点"), QString::number(getPeZeroPoint(), 'f', 2) + "μs");
        ret.insert(QObject::tr("频率"), QString::number(getPeProbeFreq(), 'f', 1) + "MHz");
        ret.insert(QObject::tr("角度"), QString::number(getPeAngle(), 'f', 1) + QObject::tr("度"));
        ret.insert(QObject::tr("晶片"), getPeCrystalPlate());
        return ret;
    }

    QString TpeType::getEmitVoltage(void) const {
        switch (m_data.pulseMode) {
            case 0:
                return QObject::tr("低压");
            case 1:
                return QObject::tr("中压");
            case 2:
                return QObject::tr("标准压");
            case 3:
                return QObject::tr("高压");
            default:
                return QObject::tr("未知");
        }
    }

    int TpeType::getRepeatFreq(void) const {
        return m_data.repeatRate;
    }

    QString TpeType::getScanMode(void) const {
        switch (m_data.scanMode) {
            case 0:
                return QObject::tr("水平扫查");
            case 1:
                return QObject::tr("非平行扫查");
            default:
                return QObject::tr("未知");
        }
    }

    QString TpeType::getTriggerMode(void) const {
        switch (m_data.coderType) {
            case 0:
                return QObject::tr("编码器");
            case 1:
                return QObject::tr("时间");
            default:
                return QObject::tr("未知");
        }
    }

    double TpeType::getScanIncrement(void) const {
        return m_data.encValue;
    }

    double TpeType::getThicknessOfWorkPiece(void) const {
        return m_data.thickness / 10.0;
    }

    double TpeType::getTofdWeldWidth(void) const {
        return m_data.weldWidth / 10.0;
    }

    QString TpeType::getGrooveType(void) const {
        switch (m_data.groove) {
            case 1:
                return QObject::tr("X型");
            case 2:
                return QObject::tr("U型");
            case 3:
                return QObject::tr("V型");
            default:
                return QObject::tr("未知");
        }
    }

    double TpeType::getTofdGain(void) const {
        return m_data.baseGain / 10.0;
    }

    QString TpeType::getFilterBand(void) const {
        switch (m_data.bandMode) {
            case 0:
                return QObject::tr("2~3.2 MHz");
            case 1:
                return QObject::tr("2.5~9 MHz");
            case 2:
                return QObject::tr("3.5~13.5 MHz");
            case 3:
                return QObject::tr("0.25~17 MHz");
            default:
                return QObject::tr("未知");
        }
    }

    QString TpeType::getDetectionMode(void) const {
        return QObject::tr("射频");
    }

    QString TpeType::getTofdProbeType(void) const {
        return QObject::tr("一收一发");
    }

    QString TpeType::getTofdProbeSize(void) const {
        if (m_data.crystal_l == 0) {
            return QString::number(m_data.crystal_w / 1000.0, 'f', 1) + "mm";
        } else {
            return QString("%1 x %2 mm").arg(m_data.crystal_l / 1000).arg(m_data.crystal_w / 1000);
        }
    }

    double TpeType::getTofdProbeFreq(void) const {
        return m_data.frequency / 100.0;
    }

    double TpeType::getTofdAngle(void) const {
        return m_data.angle;
    }

    double TpeType::getTofdProbeFront(void) const {
        return m_data.forward / 10.0;
    }

    double TpeType::getTofdZeroPoint(void) const {
        return m_data.offset / 160.0;
    }

    double TpeType::getPeGain(void) const {
        if (m_data.subData.has_value()) {
            return m_data.subData->baseGain / 10.0;
        }
        return 0.0;
    }

    double TpeType::getPeProbeFront(void) const {
        if (m_data.subData.has_value()) {
            return m_data.subData->forward / 10.0;
        }
        return 0.0;
    }

    double TpeType::getPeZeroPoint(void) const {
        if (m_data.subData.has_value()) {
            return m_data.subData->offset / 160.0;
        }
        return 0.0;
    }

    double TpeType::getPeProbeFreq(void) const {
        if (m_data.subData.has_value()) {
            return m_data.subData->frequency / 100.0;
        }
        return 0.0;
    }

    QString TpeType::getPeCrystalPlate(void) const {
        if (m_data.subData.has_value()) {
            return QString("%1 x %2 mm").arg(m_data.subData->crystal_w / 1000).arg(m_data.subData->crystal_w / 1000);
        }
        return QString();
    }

    double TpeType::getPCS(void) const {
        return m_data.pcs / 10.0;
    }

} // namespace Union::TOFD_PE::TPE
