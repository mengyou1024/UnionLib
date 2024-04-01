#include "Probe.hpp"

#include <array>
#include <cmath>
#include <sstream>
#include <string>

#if __has_include("QString")
#include <QObject>
namespace Union::Base::Probe {
    QString Index2Name_QtExtra(int index) {
        static std::array ret = {
            QObject::tr("直探头"),
            QObject::tr("斜探头"),
            QObject::tr("双晶纵波直探头"),
            QObject::tr("穿透探头"),
            QObject::tr("双晶纵波斜探头"),
        };
        if (index < 0 || index >= ret.size()) {
            return QObject::tr("未知");
        }
        return ret[index];
    }
} // namespace Union::Base::Probe
#endif

namespace Union::Base::Probe {

    inline static constexpr std::array ProbeNameMap = {
        "直探头",
        "斜探头",
        "双晶纵波直探头",
        "穿透探头",
        "双晶纵波斜探头",
    };

    std::string_view Index2Name(int index) {
        if (index < 0 || index >= ProbeNameMap.size()) {
            return "未知";
        }
        return ProbeNameMap[index];
    }

    int Name2Index(std::string_view name) {
        for (int i = 0; i < ProbeNameMap.size(); i++) {
            if (ProbeNameMap[i] == name) {
                return i;
            }
        }
        return -1;
    }

    std::string CreateProbeChipShape(int index, int a, int b) {
        std::stringstream ss;
        switch (index) {
            case 0:
                return (ss << "Φ" << a, ss.str());
            default:
                return (ss << a << "×" << b, ss.str());
        }
    }

    double Degree2K(double deg) {
        return std::tan(Degree2Rd(deg));
    }

    double K2Degree(double k) {
        return Rd2Degree(std::atan(k));
    }

} // namespace Union::Base::Probe
