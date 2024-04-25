#pragma once

#include <QJsonArray>
#include <QJsonObject>
#include <QtCore>
#include <functional>
#include <map>
#include <memory>
#include <string_view>

namespace Union::Utils {
    class ReadIntf {
    public:
        virtual ~ReadIntf() = default;
    };

    class FileReaderSelectorIntf {
    public:
        virtual ~FileReaderSelectorIntf() {
            qDebug(QLoggingCategory("Union::Utils")) << __FUNCTION__;
        };
    };

    class InstanceBuilder {
    public:
        static std::shared_ptr<FileReaderSelectorIntf> FindInstance(const std::string &name);
        static void                                    RegistInstance(const std::string &name, const std::shared_ptr<FileReaderSelectorIntf> &instance);

    private:
        inline static std::map<std::string, std::shared_ptr<FileReaderSelectorIntf>> m_data = {};
    };

    /**
     * @brief 文件读取选择器
     *
     * @tparam T_INTF 读取接口
     * @tparam I_NAME 对应的UI名称
     */
    template <class T_INTF, const std::string_view &I_NAME>
        requires std::is_base_of_v<ReadIntf, T_INTF>
    class FileReaderSelector : public FileReaderSelectorIntf {
    public:
        using FRS_RFUNC = std::function<std::unique_ptr<T_INTF>(const std::wstring &)>;
        using FRS_DTYPE = std::tuple<FRS_RFUNC, std::string>;

        FileReaderSelector(const FileReaderSelector &)            = delete;
        FileReaderSelector(FileReaderSelector &&)                 = delete;
        FileReaderSelector &operator=(const FileReaderSelector &) = delete;
        FileReaderSelector &operator=(FileReaderSelector &&)      = delete;

        virtual ~FileReaderSelector() {
            qDebug(QLoggingCategory("Union::Utils")) << __FUNCTION__;
        }

        /**
         * @brief 注册读取函数接口
         *
         * @param file_suffix 文件后缀
         * @param describe 描述
         * @param func 接口函数
         * @return bool
         */
        bool RegistReader(std::string_view file_suffix, std::string_view describe, const FRS_RFUNC &func) {
            qDebug(QLoggingCategory("FileReaserSelector")) << "instance id:" << &data;
            auto temp = std::make_tuple(func, std::string(describe));
            return data.try_emplace(std::string(file_suffix), temp).second;
        }

        /**
         * @brief 通过文件后缀查找存储的读写接口和描述
         *
         * @param file_suffix 文件后缀
         * @return std::optional<FRS_DTYPE>
         */
        const std::optional<FRS_DTYPE> Get(const std::string &file_suffix) {
            qDebug(QLoggingCategory("FileReaserSelector")) << "instance id:" << &data;
            const auto ret = data.find(file_suffix);
            if (ret == data.end()) {
                return std::nullopt;
            } else {
                return ret->second;
            }
        }

        /**
         * @brief 获取QML中FileDialog可使用的filter
         *
         * @return QJsonArray
         */
        QJsonArray GetFileNameFilter() {
            qDebug(QLoggingCategory("FileReaserSelector")) << "instance id:" << &data;
            QJsonArray ret;
            for (auto &[key, val] : data) {
                auto &[func, describe] = val;
                ret.append(QString("%1 (%2)").arg(QString::fromStdString(describe), QString::fromStdString(key).toLower()));
            }
            return ret;
        }

        /**
         * @brief 获取QML中FolderList可使用的filter
         *
         * @return QJsonArray
         */
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

        /**
         * @brief 获取后缀到UI的映射MAP
         *
         * @return QJsonObject
         */
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

        /**
         * @brief 通过文件后缀查找文件读取接口函数(不区分大小写)
         *
         * @param file_suffix 文件后缀
         * @return std::optional<FRS_RFUNC>
         */
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

        /**
         * @brief 获取单例
         *
         * @return FileReaderSelector&
         */
        static FileReaderSelector &Instance() {
            auto ret = InstanceBuilder::FindInstance(I_NAME.data());
            if (ret == nullptr) {
                ret = std::shared_ptr<FileReaderSelector>(new FileReaderSelector);
                InstanceBuilder::RegistInstance(I_NAME.data(), ret);
            }
            return *(dynamic_cast<FileReaderSelector *>(ret.get()));
        }

    private:
        FileReaderSelector()                  = default;
        std::map<std::string, FRS_DTYPE> data = {};
    };
} // namespace Union::Utils
