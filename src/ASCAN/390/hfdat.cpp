#include "hfdat.hpp"
#include "../330/_330_draw_dac.hpp"

std::unique_ptr<Union::AScan::AScanIntf> Union::__390::HFDATType::FromFile(const std::wstring &fileName) {
    auto dat = Union::__330::DATType::FromFile(fileName);
    return std::unique_ptr<Union::AScan::AScanIntf>(static_cast<Union::__390::HFDATType *>(dat.release()));
}

QJsonArray Union::__390::HFDATType::createGateValue(int idx, double soft_gain) const {
    QJsonArray ret = Union::__330::DATType::createGateValue(idx, soft_gain);

    QString strMRange;
    int     mode;
    auto    channelStatus = getHead(idx).channel_status;
    auto    channelParam  = getHead(idx).channel_param;
    auto    systemStatus  = getHead(idx).system_status;
    auto    dacParam      = getHead(idx).dac;
    auto    wavepara      = m_data.at(m_fileName_index).at(idx).wave_para;
    if (((channelStatus.status >> 2) & 0x01) == 1) {
        mode = (((channelStatus.option >> 28) & 0x0f) % 6) + 1;
    } else if (((channelStatus.status >> 3) & 0x01) == 1) {
        int Ax = channelStatus.option; /*标度：垂直\水平\距离2位	d3d2*/
        Ax     = (Ax >> 26);
        Ax     = (Ax & 0x03);
        mode   = Ax + 10;
    } else
        mode = -1;

    if (mode == 1) {
        if (systemStatus.unit % 4) {
            strMRange = "Φ" + QString::number(dacParam.diameter / 10.0 / 25.4, 'f', 3) + " x " + QString::number(dacParam.length / 10.0 / 25.4, 'f', 3);

        } else {
            strMRange = "Φ" + QString::number(dacParam.diameter / 10) + " x " + QString::number(dacParam.length / 10);
        }
        float fOffset = int(wavepara[3]) / 10.0;
        if (fOffset > 0) {
            strMRange += " +" + QString::number(int(wavepara[3]) / 10.0, 'f', 1);
        } else {
            strMRange += " " + QString::number(int(wavepara[3]) / 10.0, 'f', 1);
        }
        strMRange += "dB";
    } else if (mode > 1) {
        if (systemStatus.unit % 4) {
            strMRange = "Φ" + QString::number(dacParam.diameter / 10.0 / 25.4, 'f', 3);
        } else {
            strMRange = "Φ" + QString::number(dacParam.diameter / 10);
        }

        if ((mode < 10) || (mode > 11)) {
            auto Mid        = int(wavepara[3]);
            auto strMRange1 = QString::number(wavepara[2] / (10.0), 'f', 1);
            if (Mid > 0)
                strMRange1 = " +" + QString::number(abs(Mid / 10.0), 'f', 1);
            else
                strMRange1 = " -" + QString::number(abs(Mid / (10.0)), 'f', 1);
            strMRange = strMRange + strMRange1 + "dB";
        }

    } else if (mode < 1) {
        QString s0 = QString::asprintf("∧%0.1f %", (float)(int(wavepara[3]) / 10.0));
        strMRange  = "";
        (void)s0;
    }
    qDebug(QLoggingCategory("HFD")) << "strMRange" << strMRange;
    qDebug(QLoggingCategory("HFD")) << ret;
    auto obj1 = ret[0].toObject();
    auto obj2 = ret[1].toObject();
    if (strMRange.isEmpty()) {
        strMRange = "-";
    }
    obj1["equi"] = strMRange;
    // obj1["dist_a"] = QString::number(wavepara[0] / 10.0, 'f', 1);
    // obj1["dist_b"] = QString::number(wavepara[1] / 10.0, 'f', 1);
    // obj1["dist_c"] = QString::number(wavepara[2] / 10.0, 'f', 1);
    obj2["equi"] = "-";
    ret.replace(0, obj1);
    ret.replace(1, obj2);
    return ret;
}

std::string Union::__390::HFDATType::getInstrumentName(void) const {
    return "PXUT-390";
}
