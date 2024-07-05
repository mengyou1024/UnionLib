
#include "union_common.hpp"
#include <Yo/Types>
#include <stdexcept>

double Union::CalculatedGain(double input, double output) {
    return 20.0 * std::log10(output / input);
}
double Union::CalculateGainOutput(double input, double gain) {
    return input * std::pow(10.0, gain / 20.0);
}

double Union::CalculateGainInput(double output, double gain) {
    return output / std::pow(10.0, gain / 20.0);
}

double Union::ValueMap(double val, const std::array<double, 2>& dist, const std::array<double, 2>& raw) {
    return (val - raw[0]) / (raw[1] - raw[0]) * (dist[1] - dist[0]) + dist[0];
}

double Union::EchoDbDiffOfPlan(double x1, double x2) {
    return 20.0 * std::log10(x2 / x1);
}

double Union::EchoDbDiffOfHole(double x1, double d1, double x2, double d2) {
    return 40.0 * std::log10((d1 * x2) / (d2 * x1));
}

double Union::EchoDbDiffOfPlanAndHole(double lambda, double x, double d) {
    return 20.0 * std::log10((2.0 * lambda * x) / (std::numbers::pi * std::pow(d, 2)));
}

QString Union::LocationCodeToUTF8(const std::vector<uint16_t>& code) {
    auto        copy = code;
    std::string str;
    for (const auto& it : copy) {
        if (it > 128) {
            uint8_t loc = (uint8_t)(it / 100);
            uint8_t bit = (uint8_t)(it % 100);
            str.push_back(loc + 0xA0);
            str.push_back(bit + 0xA0);
        } else {
            str.push_back(it & 0xFF);
        }
    }
    return QString::fromStdString(Yo::Types::GB2312ToUtf8(str));
}

double Union::CalculateNearField(double l_or_d, double w_or_zero, double probe_freq, double speed) {
    double temp = 0.0;
    if (l_or_d != 0 && w_or_zero != 0) {
        temp = l_or_d * w_or_zero;
    } else if (l_or_d == 0 && w_or_zero == 0) {
        temp = 25.0;
    } else {
        temp = std::pow(l_or_d + w_or_zero, 2);
    }
    return (1000.0 * temp * probe_freq) / (4 * speed);
}

namespace Union::Base {
    QList<QPointF> CreateLineSeriesData(
        const std::vector<uint8_t>& data,
        std::pair<double, double>   axis_range,
        uint8_t                     view_max,
        double                      soft_gain,
        int                         supression) {
        // empty line
        QList<QPointF> ret;
        const auto     step = (axis_range.second - axis_range.first) / (data.size() - 1);
        for (auto i = 0; std::cmp_less(i, data.size()); i++) {
            auto ampValue = data[i] / (view_max / 100.0);
            if (ampValue < supression) {
                ampValue = 0.0;
            }
            ret.append(QPointF(axis_range.first + i * step, CalculateGainOutput(ampValue, soft_gain)));
        }
        return ret;
    }
} // namespace Union::Base
