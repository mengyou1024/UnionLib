#pragma once

#include <QString>
#include <numbers>
#include <string_view>

namespace Union::Base::Probe {
    QString Index2Name_QtExtra(int index);
    int Name2Index_QtExtra(QString name);
    bool IsStraightBeamProbe(QString name);
    bool IsStraightBeamProbe(int idx);

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
