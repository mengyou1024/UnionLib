
#include "common.hpp"
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
