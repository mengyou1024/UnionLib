#include "Probe.hpp"

#include <array>
#include <cmath>
#include <sstream>
#include <string>

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
