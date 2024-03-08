#include "AScanType.hpp"
#include <map>
#include <tuple>

namespace Union::AScan {
#if __has_include("QtCore")
    QJsonObject AScanType::GetBoardParamJSON(const AScanType& val) {
        QJsonObject ret;
        ret.insert("probe", QString::fromStdWString(val.probe));
        ret.insert("angle", val.angle);
        ret.insert("soundVelocity", val.soundVelocity);
        QJsonObject performance;
        performance.insert("horizontalLinearity", val.performance.horizontalLinearity);
        performance.insert("verticalLinearity", val.performance.verticalLinearity);
        performance.insert("resolution", val.performance.resolution);
        performance.insert("dynamicRange", val.performance.dynamicRange);
        performance.insert("sensitivity", val.performance.sensitivity);
        ret.insert("performance", performance);
        return ret;
    }

    QList<QPointF> AScanType::GetAScanSeriesData(const AScanData& data, double softGain) {
        QList<QPointF> ret;
        const auto     AScanStep = data.axisLen / static_cast<double>(data.ascan.size());
        for (auto i = 0; std::cmp_less(i, data.ascan.size()); i++) {
            auto ampValue = data.ascan[i] / 2.0;
            if (ampValue <= data.suppression) {
                ampValue = 0;
            }
            ret.append({data.axisBias + i * AScanStep, CalculateGainOutput(ampValue, softGain)});
        }
        return ret;
    }

    bool AScanFileSelector::RegistReader(std::string_view file_suffix, std::string_view describe, const AScanFileSelector::AFS_RFUNC& func) {
        auto temp = std::make_tuple(func, std::string(describe));
        return data.try_emplace(std::string(file_suffix), temp).second;
    }

    const std::optional<AScanFileSelector::AFS_DTYPE> AScanFileSelector::Get(const std::string& file_suffix) {
        const auto ret = data.find(file_suffix);
        if (ret == data.end()) {
            return std::nullopt;
        } else {
            return ret->second;
        }
    }

    QJsonArray AScanFileSelector::GetFileNameFilter() {
        QJsonArray ret;
        QString    str("所有文件 (");
        for (auto& [key, val] : data) {
            str += QString::fromStdString(key).toLower() + " ";
        }
        str.replace(str.size(), 1, ')');
        ret.append(str);
        for (auto& [key, val] : data) {
            auto& [func, describe] = val;
            ret.append(QString("%1 (%2)").arg(QString::fromStdString(describe), QString::fromStdString(key).toLower()));
        }
        return ret;
    }

    QJsonArray AScanFileSelector::GetFileListModelNameFilter() {
        QJsonArray obj;
        for (auto& [key, val] : data) {
            static QRegularExpression reg(R"((\*\.\w+))");
            auto                      ret = reg.globalMatch(QString::fromStdString(key), 0);
            while (ret.hasNext()) {
                QRegularExpressionMatch match = ret.next();
                obj.append(match.captured(1).toLower());
            }
        }
        return obj;
    }

    QJsonObject AScanFileSelector::GetUINameMap() {
        QJsonObject obj;
        for (auto& [key, val] : data) {
            static QRegularExpression reg(R"((\.\w+))");
            auto                      ret = reg.globalMatch(QString::fromStdString(key), 0);
            while (ret.hasNext()) {
                QRegularExpressionMatch match = ret.next();
                obj.insert(match.captured(1).toLower(), "AScan");
            }
        }
        return obj;
    }

    const std::optional<AScanFileSelector::AFS_RFUNC> AScanFileSelector::GetReadFunction(const std::wstring& fileName) {
        QFileInfo file(QString::fromStdWString(fileName));
        for (auto& [key, val] : data) {
            if (QString::fromStdString(key).toLower().contains(file.suffix().toLower())) {
                const auto& [func, describe] = val;
                return func;
            }
        }
        return std::nullopt;
    }

    QJsonObject AScanType::GetBoardParamJSON(void) const {
        QJsonObject ret;
        ret.insert("probe", QString::fromStdWString(probe));
        ret.insert("angle", angle);
        ret.insert("soundVelocity", soundVelocity);
        QJsonObject _performance;
        _performance.insert("horizontalLinearity", this->performance.horizontalLinearity);
        _performance.insert("verticalLinearity", this->performance.verticalLinearity);
        _performance.insert("resolution", this->performance.resolution);
        _performance.insert("dynamicRange", this->performance.dynamicRange);
        _performance.insert("sensitivity", this->performance.sensitivity);
        ret.insert("performance", _performance);
        return ret;
    }

    QList<QPointF> AScanType::GetAScanSeriesData(int index, double softGain) const {
        if (std::cmp_greater(index, this->data.size())) {
            throw std::runtime_error("AScanType::GetAScanSeriesData: index out of range");
        }
        const auto&    aScanData = this->data[index];
        QList<QPointF> ret;
        const auto     AScanStep = aScanData.axisLen / static_cast<double>(aScanData.ascan.size());
        for (auto i = 0; std::cmp_less(i, aScanData.ascan.size()); i++) {
            ret.append({aScanData.axisBias + i * AScanStep, CalculateGainOutput(aScanData.ascan[i] / 2.0, softGain)});
        }
        return ret;
    }
#endif

} // namespace Union::AScan
