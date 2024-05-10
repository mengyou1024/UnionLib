#pragma once

#include "ldat.hpp"

namespace Union::__390N_T8 {

    class VDATA : public LDAT {
    private:
        std::vector<QImage> m_image = {};
    public:
        static std::unique_ptr<Union::AScan::AScanIntf> FromFile(const std::wstring& fileName);

        size_t __Read(std::ifstream& file, size_t file_size) override;

        bool hasCameraImage(void) const override;

        QImage getCameraImage(int idx) const override;
    };
} // namespace Union::__390N_T8
