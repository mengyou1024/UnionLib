#include "Probe.hpp"

#include <QObject>
#include <array>
#include <cmath>
#include <sstream>
#include <string>

namespace Union::Base::Probe {
    static std::array probeArray = {
        QObject::tr("直探头"),
        QObject::tr("斜探头"),
        QObject::tr("双晶纵波直探头"),
        QObject::tr("穿透探头"),
        QObject::tr("双晶纵波斜探头"),
    };

    QString Index2Name_QtExtra(int index) {
        if (index < 0 || std::cmp_greater_equal(index, probeArray.size())) {
            return QObject::tr("未知");
        }
        return probeArray[index];
    }

    int Name2Index_QtExtra(QString name) {
        for (auto i = 0; std::cmp_less(i, probeArray.size()); i++) {
            if (probeArray[i] == name) {
                return i;
            }
        }
        return -1;
    }

} // namespace Union::Base::Probe

namespace Union::Base::Probe {
    std::string CreateProbeChipShape(int index, int a, int b) {
        std::stringstream ss;
        switch (index) {
            case 0:
                return (ss << "Φ" << a << "mm", ss.str());
            case 1:
                return (ss << a << "×" << b, ss.str());
            case 2:
                return (ss << a << "mm", ss.str());
            default: return (ss << a << "×" << b, ss.str());
        }
    }

    double Degree2K(double deg) {
        return std::tan(Degree2Rd(deg));
    }

    double K2Degree(double k) {
        return Rd2Degree(std::atan(k));
    }

} // namespace Union::Base::Probe
