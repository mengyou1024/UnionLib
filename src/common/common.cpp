
#include "common.hpp"
#include <Yo/Types>

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
