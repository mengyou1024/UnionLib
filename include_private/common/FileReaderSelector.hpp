#pragma once

#include <functional>
#include <map>
#include <memory>
#include <string_view>

#if __has_include("QtCore")
    #include <QJsonArray>
    #include <QJsonObject>
    #include <QtCore>
#endif

namespace Union::Utils {
#if __has_include("QtCore")

    class FileReaderSelectorIntf {
    public:
        virtual ~FileReaderSelectorIntf() {
            qDebug(QLoggingCategory("Union::Utils")) <<  __FUNCTION__;
        };
    };

    class InstanceBuilder {
    public:
        static std::shared_ptr<FileReaderSelectorIntf> FindInstance(const std::string &name);
        static void                                    RegistInstance(const std::string &name, const std::shared_ptr<FileReaderSelectorIntf> &instance);

    private:
        inline static std::map<std::string, std::shared_ptr<FileReaderSelectorIntf>> m_data = {};
    };

    template <class T_INTF, const std::string_view &I_NAME>
    class FileReaderSelector:public FileReaderSelectorIntf {
    public:
        using FRS_RFUNC = std::function<std::unique_ptr<T_INTF>(const std::wstring &)>;
        using FRS_DTYPE = std::tuple<FRS_RFUNC, std::string>;

        FileReaderSelector(const FileReaderSelector &)            = delete;
        FileReaderSelector(FileReaderSelector &&)                 = delete;
        FileReaderSelector &operator=(const FileReaderSelector &) = delete;
        FileReaderSelector &operator=(FileReaderSelector &&)      = delete;

        virtual ~FileReaderSelector()  {
            qDebug(QLoggingCategory("Union::Utils")) <<  __FUNCTION__;
        }

        bool RegistReader(std::string_view file_suffix, std::string_view describe, const FRS_RFUNC &func) {
            qDebug(QLoggingCategory("FileReaserSelector")) << "instance id:" << &data;
            auto temp = std::make_tuple(func, std::string(describe));
            return data.try_emplace(std::string(file_suffix), temp).second;
        }

        const std::optional<FRS_DTYPE> Get(const std::string &file_suffix) {
            qDebug(QLoggingCategory("FileReaserSelector")) << "instance id:" << &data;
            const auto ret = data.find(file_suffix);
            if (ret == data.end()) {
                return std::nullopt;
            } else {
                return ret->second;
            }
        }

        QJsonArray GetFileNameFilter() {
            qDebug(QLoggingCategory("FileReaserSelector")) << "instance id:" << &data;
            QJsonArray ret;
            for (auto &[key, val] : data) {
                auto &[func, describe] = val;
                ret.append(QString("%1 (%2)").arg(QString::fromStdString(describe), QString::fromStdString(key).toLower()));
            }
            return ret;
        }

        QJsonArray GetFileListModelNameFilter() {
            qDebug(QLoggingCategory("FileReaserSelector")) << "instance id:" << &data;
            QJsonArray obj;
            for (auto &[key, val] : data) {
                static QRegularExpression reg(R"((\*\.\w+))");
                auto                      ret = reg.globalMatch(QString::fromStdString(key), 0);
                while (ret.hasNext()) {
                    QRegularExpressionMatch match = ret.next();
                    obj.append(match.captured(1).toLower());
                }
            }
            return obj;
        }

        QJsonObject GetUINameMap() {
            qDebug(QLoggingCategory("FileReaserSelector")) << "instance id:" << &data;
            QJsonObject obj;
            for (auto &[key, val] : data) {
                static QRegularExpression reg(R"((\.\w+))");
                auto                      ret = reg.globalMatch(QString::fromStdString(key), 0);
                while (ret.hasNext()) {
                    QRegularExpressionMatch match = ret.next();
                    obj.insert(match.captured(1).toLower(), I_NAME.data());
                }
            }
            return obj;
        }

        const std::optional<FRS_RFUNC> GetReadFunction(const std::wstring &fileName) {
            qDebug(QLoggingCategory("FileReaserSelector")) << "instance id:" << &data;
            QFileInfo file(QString::fromStdWString(fileName));
            for (auto &[key, val] : data) {
                if (QString::fromStdString(key).toLower().contains(file.suffix().toLower())) {
                    const auto &[func, describe] = val;
                    return func;
                }
            }
            return std::nullopt;
        }

        static FileReaderSelector &Instance() {
            auto ret = InstanceBuilder::FindInstance(I_NAME.data());
            if (ret == nullptr) {
                ret = std::shared_ptr<FileReaderSelector>(new FileReaderSelector);
                InstanceBuilder::RegistInstance(I_NAME.data(), ret);
            }
            return *(dynamic_cast<FileReaderSelector *>(ret.get()));
        }

    private:
        FileReaderSelector() = default;
        std::map<std::string, FRS_DTYPE> data = {};
    };
#endif
} // namespace Union::Utils
