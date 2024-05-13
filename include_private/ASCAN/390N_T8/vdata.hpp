#pragma once

#include "ldat.hpp"

namespace Union::AScan::Special {

} // namespace Union::AScan::Special

namespace Union::__390N_T8 {

    class VDATA : public LDAT, public AScan::Special::CameraImageSpecial {
    private:
        std::vector<QImage> m_image = {};

    public:
        static std::unique_ptr<Union::AScan::AScanIntf> FromFile(const std::wstring& fileName);

        size_t         __Read(std::ifstream& file, size_t file_size) override;
        virtual QImage getCameraImage(int idx) const override;
    };
} // namespace Union::__390N_T8
