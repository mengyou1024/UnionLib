#include "FileReaderSelector.hpp"

#if __has_include("QtCore")
namespace Union::Utils {
    std::shared_ptr<FileReaderSelectorIntf> InstanceBuilder::FindInstance(const std::string &name) {
        if (m_data.contains(name)) {
            return m_data[name];
        }
        return nullptr;
    }

    void InstanceBuilder::RegistInstance(const std::string &name, const std::shared_ptr<FileReaderSelectorIntf> &instance) {
        m_data[name] = instance;
    }

} // namespace Union::Utils
#endif
