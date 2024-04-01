#pragma once

#include <numbers>
#include <string_view>

#if __has_include("QString")
    #include <QString>
namespace Union::Base::Probe {
    QString Index2Name_QtExtra(int index);
} // namespace Union::Base::Probe
#endif

namespace Union::Base::Probe {

    std::string_view Index2Name(int index);
    int              Name2Index(std::string_view name);
    std::string      CreateProbeChipShape(int index, int a, int b);
    constexpr double Degree2Rd(double deg) {
        return deg / 180.0 * std::numbers::pi;
    }

    constexpr double Rd2Degree(double rd) {
        return rd / std::numbers::pi * 180.0;
    }

    constexpr double Rd2Degree(double rd);
    double           Degree2K(double deg);
    double           K2Degree(double k);
} // namespace Union::Base::Probe
