#pragma once
#include <QByteArray>
#include <QDataStream>
#include <cstdint>
#include <span>
#include <string>
#include <vector>
namespace Union::__390N_T8::MDATType {

    constexpr uint32_t FILE_MAGIC_CODE = 0x556EE655;
    constexpr uint8_t  FRAME_HEAD      = 0x55;
    constexpr uint8_t  FRAME_TAIL      = 0x6E;

    template <uint16_t _C_T = 0xFFFF>
    struct Serializer {
        inline static constexpr uint8_t HEAD = FRAME_HEAD;
        inline static constexpr uint8_t TAIL = FRAME_TAIL;

        inline static constexpr uint16_t CLASS_TYPE                                    = _C_T;
        virtual void                     serialize_payload(QDataStream& payload) const = 0;
        virtual void                     unserialize_payload(QDataStream& payload)     = 0;

        virtual ~Serializer() = default;
        virtual void serialize(QDataStream& data) const final {
            QByteArray  arr;
            QDataStream arr_stream(&arr, QIODevice::WriteOnly);
            arr_stream.setByteOrder(QDataStream::LittleEndian);
            arr_stream.setFloatingPointPrecision(QDataStream::SinglePrecision);
            serialize_payload(arr_stream);
            data << HEAD << CLASS_TYPE << static_cast<uint32_t>(arr.size());
            data.writeRawData(arr.data(), arr.size());
            data << TAIL;
        }
    };

    struct InstrumentBaseInfo : public Serializer<0> {
    public:
        InstrumentBaseInfo() = default;
        InstrumentBaseInfo(uint8_t name, uint32_t version, const QString& time) :
        m_name(name),
        m_version(version),
        m_time(time) {
        }

        QString getNameString() const {
            switch (m_name) {
                case 0:
                    return "PXUT-390N";
                case 1:
                    return "PXUT-T8";
                default:
                    return "Unknow";
            }
        }

        uint32_t getVersionCode() const {
            return m_version;
        }

        QString getVersionString() const {
            return QString("v%1.%2.%3").arg(getMajorVersion()).arg(getMinorVersion()).arg(getPatchVersion());
        }

        QString getTime() const {
            return m_time;
        }

        virtual void serialize_payload(QDataStream& payload) const override {
            std::string time = m_time.toStdString();
            payload << m_name << m_version << static_cast<uint8_t>(m_time.length());
            payload.writeRawData(time.c_str(), std::ssize(time));
        }

        virtual void unserialize_payload(QDataStream& payload) override {
            uint8_t len;
            payload >> m_name >> m_version >> len;
            std::string time;
            time.resize(len);
            payload.readRawData(time.data(), len);
            m_time = QString::fromStdString(time);
        }

        bool operator==(InstrumentBaseInfo other) {
            return m_name == other.m_name && m_version == other.m_version && m_time == other.m_time;
        }

    private:
        int getMajorVersion() const {
            return (m_version >> 24);
        }

        int getMinorVersion() const {
            return (m_version >> 16) & 0xFF;
        }

        int getPatchVersion() const {
            return m_version & 0xFFFF;
        }

        uint8_t  m_name    = {};
        uint32_t m_version = {};
        QString  m_time    = {};
    };

    struct ChannelParam : public Serializer<1> {
        float        axisBias              = {};
        float        axisLen               = {};
        float        baseGain              = {};
        float        scanGain              = {};
        float        compensatingGain      = {};
        float        suppression           = {};
        uint8_t      distanceMode          = {};
        uint8_t      channel               = {};
        float        zeroPoint             = {};
        float        probeFrontDistance    = {};
        float        soundVelocity         = {};
        float        angle                 = {};
        float        probeChipShapeWorD    = {};
        float        probeChipShapeLorZero = {};
        uint8_t      probe                 = {};
        float        gateAPos              = {};
        float        gateAWidth            = {};
        float        gateAHeight           = {};
        uint8_t      gateAEnable           = {};
        float        gateBPos              = {};
        float        gateBWidth            = {};
        float        gateBHeight           = {};
        uint8_t      gateBEnable           = {};
        float        probeFrequency        = {};
        float        samplingDelay         = {};
        virtual void serialize_payload(QDataStream& payload) const override {
            payload
                << axisBias
                << axisLen
                << baseGain
                << scanGain
                << compensatingGain
                << suppression
                << distanceMode
                << channel
                << zeroPoint
                << probeFrontDistance
                << soundVelocity
                << angle
                << probeChipShapeWorD
                << probeChipShapeLorZero
                << probe
                << gateAPos
                << gateAWidth
                << gateAHeight
                << gateAEnable
                << gateBPos
                << gateBWidth
                << gateBHeight
                << gateBEnable
                << probeFrequency
                << samplingDelay;
        }

        virtual void unserialize_payload(QDataStream& payload) override {
            payload >> axisBias >> axisLen >> baseGain >> scanGain >> compensatingGain >> suppression >> distanceMode >> channel >> zeroPoint >> probeFrontDistance >> soundVelocity >> angle >> probeChipShapeWorD >> probeChipShapeLorZero >> probe >> gateAPos >> gateAWidth >> gateAHeight >> gateAEnable >> gateBPos >> gateBWidth >> gateBHeight >> gateBEnable >> probeFrequency >> samplingDelay;
        }

        bool operator==(ChannelParam other) {
            return axisBias == other.axisBias && axisLen == other.axisLen && baseGain == other.baseGain && scanGain == other.scanGain && compensatingGain == other.compensatingGain && suppression == other.suppression && distanceMode == other.distanceMode && channel == other.channel && zeroPoint == other.zeroPoint && probeFrontDistance == other.probeFrontDistance && soundVelocity == other.soundVelocity &&
                   angle == other.angle && probeChipShapeWorD == other.probeChipShapeWorD && probeChipShapeLorZero == other.probeChipShapeLorZero && probe == other.probe && gateAPos == other.gateAPos && gateAWidth == other.gateAWidth && gateAHeight == other.gateAHeight && gateAEnable == other.gateAEnable && gateBPos == other.gateBPos && gateBWidth == other.gateBWidth && gateBHeight == other.gateBHeight && gateBEnable == other.gateBEnable && probeFrequency == other.probeFrequency && samplingDelay == other.samplingDelay;
        }
    };

    struct DACParam : public Serializer<2> {
        uint8_t            isReady           = {};
        float              baseGain          = {};
        float              compensatingGain  = {};
        std::vector<float> index             = {};
        std::vector<float> value             = {};
        float              equivalent        = {};
        uint8_t            criteria          = {};
        float              criteriaBiasRL    = {};
        float              criteriaBiasSL    = {};
        float              criteriaBiasEL    = {};
        uint8_t            onlyShowBaseLine  = {};
        float              samplingXAxisBias = {};
        float              samplingXAxisLen  = {};
        virtual void       serialize_payload(QDataStream& payload) const override {
            payload << isReady << baseGain << compensatingGain << static_cast<uint8_t>(index.size());
            payload.writeRawData(reinterpret_cast<const char*>(index.data()), static_cast<int>(index.size() * sizeof(float)));
            payload.writeRawData(reinterpret_cast<const char*>(value.data()), static_cast<int>(value.size() * sizeof(float)));
            payload << equivalent << criteria << criteriaBiasRL << criteriaBiasSL << criteriaBiasEL << onlyShowBaseLine << samplingXAxisBias << samplingXAxisLen;
        }
        virtual void unserialize_payload(QDataStream& payload) override {
            uint8_t len;
            payload >> isReady >> baseGain >> compensatingGain >> len;
            index.resize(len);
            value.resize(len);
            payload.readRawData(reinterpret_cast<char*>(index.data()), static_cast<int>(len * sizeof(float)));
            payload.readRawData(reinterpret_cast<char*>(value.data()), static_cast<int>(len * sizeof(float)));
            payload >> equivalent >> criteria >> criteriaBiasRL >> criteriaBiasSL >> criteriaBiasEL >> onlyShowBaseLine >> samplingXAxisBias >> samplingXAxisLen;
        }

        bool operator==(DACParam other) {
            return isReady == other.isReady && baseGain == other.baseGain && compensatingGain == other.compensatingGain && index == other.index && value == other.value && equivalent == other.equivalent && criteria == other.criteria && criteriaBiasRL == other.criteriaBiasRL && criteriaBiasSL == other.criteriaBiasSL && criteriaBiasEL == other.criteriaBiasEL && onlyShowBaseLine == other.onlyShowBaseLine && samplingXAxisBias == other.samplingXAxisBias && samplingXAxisLen == other.samplingXAxisLen;
        }
    };

    struct AVGParam : public Serializer<3> {
        uint8_t            isReady           = {};
        float              baseGain          = {};
        float              compensatingGain  = {};
        float              scanGain          = {};
        std::vector<float> index             = {};
        std::vector<float> value             = {};
        uint8_t            onlyShowBaseLine  = {};
        float              samplingXAxisBias = {};
        float              samplingXAxisLen  = {};
        float              diameter          = {};
        float              reflectorDiameter = {};
        float              reflectorMaxDepth = {};
        float              equivalent        = {};
        virtual void       serialize_payload(QDataStream& payload) const override {
            payload << isReady << baseGain << compensatingGain << scanGain << static_cast<uint8_t>(index.size());
            payload.writeRawData(reinterpret_cast<const char*>(index.data()), static_cast<int>(index.size() * sizeof(float)));
            payload.writeRawData(reinterpret_cast<const char*>(value.data()), static_cast<int>(value.size() * sizeof(float)));
            payload << onlyShowBaseLine << samplingXAxisBias << samplingXAxisLen << diameter << reflectorDiameter << reflectorMaxDepth << equivalent;
        }
        virtual void unserialize_payload(QDataStream& payload) override {
            uint8_t len;
            payload >> isReady >> baseGain >> compensatingGain >> scanGain >> len;
            index.resize(len);
            value.resize(len);
            payload.readRawData(reinterpret_cast<char*>(index.data()), static_cast<int>(len * sizeof(float)));
            payload.readRawData(reinterpret_cast<char*>(value.data()), static_cast<int>(len * sizeof(float)));
            payload >> onlyShowBaseLine >> samplingXAxisBias >> samplingXAxisLen >> diameter >> reflectorDiameter >> reflectorMaxDepth >> equivalent;
        }

        bool operator==(AVGParam other) {
            return isReady == other.isReady && baseGain == other.baseGain && compensatingGain == other.compensatingGain && scanGain == other.scanGain && index == other.index && value == other.value && onlyShowBaseLine == other.onlyShowBaseLine && samplingXAxisBias == other.samplingXAxisBias && samplingXAxisLen == other.samplingXAxisLen && diameter == other.diameter && reflectorDiameter == other.reflectorDiameter && reflectorMaxDepth == other.reflectorMaxDepth && equivalent == other.equivalent;
        }
    };

    struct Performance : public Serializer<4> {
        float        horizontalLinearity = {};
        float        verticalLinearity   = {};
        float        resolution          = {};
        float        dynamicRange        = {};
        float        surplusSensitivity  = {};
        virtual void serialize_payload(QDataStream& payload) const override {
            payload << horizontalLinearity << verticalLinearity << resolution << dynamicRange << surplusSensitivity;
        }
        virtual void unserialize_payload(QDataStream& payload) override {
            payload >> horizontalLinearity >> verticalLinearity >> resolution >> dynamicRange >> surplusSensitivity;
        }

        bool operator==(Performance other) {
            return horizontalLinearity == other.horizontalLinearity && verticalLinearity == other.verticalLinearity && resolution == other.resolution && dynamicRange == other.dynamicRange && surplusSensitivity == other.surplusSensitivity;
        }
    };

    struct CameraData : public Serializer<5> {
    public:
        uint16_t             m_width     = {};
        uint16_t             m_height    = {};
        uint8_t              m_imgFormat = {};
        std::vector<uint8_t> m_data      = {};

        virtual void serialize_payload(QDataStream& payload) const override {
            payload << m_width << m_height << m_imgFormat << static_cast<uint32_t>(m_data.size());
            payload.writeRawData(reinterpret_cast<const char*>(m_data.data()), std::ssize(m_data));
        }
        virtual void unserialize_payload(QDataStream& payload) override {
            uint32_t data_size;
            payload >> m_width >> m_height >> m_imgFormat >> data_size;
            m_data.resize(data_size);
            payload.readRawData(reinterpret_cast<char*>(m_data.data()), std::ssize(m_data));
        }

        bool operator==(CameraData other) {
            return m_width == other.m_width && m_height == other.m_height && m_imgFormat == other.m_imgFormat && m_data == other.m_data;
        }
    };

    struct AScanData : public Serializer<6> {
    public:
        std::vector<uint8_t> m_data = {};

        virtual void serialize_payload(QDataStream& payload) const override {
            payload.writeRawData(reinterpret_cast<const char*>(m_data.data()), std::ssize(m_data));
        }

        virtual void unserialize_payload(QDataStream& payload) override {
            (void)payload;
            throw std::runtime_error("this function will not be used, use void unserialize_payload(QDataStream& payload, uint32_t len)");
        }

        void unserialize_payload(QDataStream& payload, uint32_t len) {
            m_data.resize(len);
            payload.readRawData(reinterpret_cast<char*>(m_data.data()), std::ssize(m_data));
        }

        bool operator==(AScanData other) {
            return m_data == other.m_data;
        }
    };

    struct CMP000 : public Serializer<0x8000> {
        uint8_t gateBType          = {};
        float   criteriaBiasLine4  = {};
        float   criteriaBiasLine5  = {};
        float   criteriaBiasLine6  = {};
        uint8_t LineNumber         = {};
        float   workPieceThickness = {};

        virtual void serialize_payload(QDataStream& payload) const override {
            payload << gateBType << criteriaBiasLine4 << criteriaBiasLine5 << criteriaBiasLine6 << LineNumber << workPieceThickness;
        }

        virtual void unserialize_payload(QDataStream& payload) override {
            payload >> gateBType >> criteriaBiasLine4 >> criteriaBiasLine5 >> criteriaBiasLine6 >> LineNumber >> workPieceThickness;
        }

        bool operator==(CMP000 other) {
            return gateBType == other.gateBType && criteriaBiasLine4 == other.criteriaBiasLine4 && criteriaBiasLine5 == other.criteriaBiasLine5 && criteriaBiasLine6 == other.criteriaBiasLine6 && LineNumber == other.LineNumber && workPieceThickness == other.workPieceThickness;
        }
    };
} // namespace Union::__390N_T8::MDATType
