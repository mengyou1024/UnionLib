#pragma once
#include <QColormap>
#include <QRect>

#include "TofdDefine.h"

namespace Union::TOFD_MULTI::__9x0 {

    constexpr auto SAMPLING_RATE      = 100;  // 超声采样分辨率 10ns一个点 每us 100个点
    constexpr auto CHANNEL_NUM        = 10;   // 通道数
    constexpr auto TOFD_CHANNEL_NUM   = 6;    // TOFD通道数
    constexpr auto SCAN_INCREMENT_MIN = 0.25; // 扫查增量

    struct URange {
        float Start;
        float End;
    };

    struct UValue {
        float VX;
        float VX_Us;
        float VY;
        float VZ;
    };

    struct UDataPosition {
        int X;
        int Y;
    };

    enum PaddingType {
        Left,
        Right,
        Bottom,
        Top,
    };

    enum VeiwType {
        TOFD,
        PE,
    };

    enum DirectionType {
        Vertical,
        Horizontal,
    };

    enum CrossType {
        Cross,
        CrossWithParabola,
    };

    struct ViewRectangleInfo {
        // 是否激活
        bool IsActive;
        // 视图类型 0:TOFD  1:PE
        VeiwType VeiwType;
        // 显示方向, 0:横向声程 1：纵向声程
        DirectionType DisplayDirection;
        // 窗口区域
        QRect WindowRect;
        // Ascan 图像区域
        QRect AScanRect;
        // Bscan 图像区域
        QRect BScanRect;
        // Ascan 图像区域
        QRect AScanImageRect;
        // Bscan 图像区域
        QRect BScanImageRect;

        // Ascan 放大后图像区域
        QRect AScanScaledImageRect;
        // Bscan 放大后图像区域
        QRect BScanScaledImageRect;

        // 波幅色卡 区域
        QRect AScanYColorMapRect;
        // 波幅标尺 区域
        QRect AScanYCaliperRect;
        // 声程标尺 区域
        QRect BScanXCaliperRect;
        // 编码器标尺 区域
        QRect BScanYCaliperRect;

        // 声程缩放滚动条 区域
        QRect XScrollRect;
        // 编码器缩放滚动条 区域
        QRect YScrollRect;

        // 波幅色卡 停靠方向
        PaddingType AScanYColorMapPadding;
        // 波幅标尺 停靠方向
        PaddingType AScanYCaliperPadding;
        // 声程标尺 停靠方向
        PaddingType BScanXCaliperPadding;
        // 编码器标尺 停靠方向
        PaddingType BScanYCaliperPadding;

        // 是否显示标尺
        bool IsShowCaliper;
        // 是否显示滚动条
        bool IsShowScroll;
        // 是否显示A扫波门
        bool IsShowGate;
        // 是否显示光标
        bool IsShowCross;

        // 是否显示声程为时间
        bool IsShowTime;

        // 声程范围，单位us
        URange XRange;
        // 声程范围，单位mm
        URange XmmRange;
        // 编码器范围，单位mm
        URange YRange;
        // 幅度范围，单位%
        URange ZRange;

        // 实际显示声程范围，单位us
        URange XTakeRange;
        // 实际显示声程范围，单位us
        URange XmmTakeRange;
        // 实际显示编码器范围，单位mm
        URange YTakeRange;

        // 实际显示声程范围 占比
        URange XTakeRangeRatio;
        // 实际显示编码器范围 占比
        URange YTakeRangeRatio;

        // 声程的缩放比(每数据点占的窗口像素)
        float XScale;
        // 编码器的缩放比
        float YScale;

        // 备份的读取文件的原始数据
        std::byte* BakData;
        // 用于处理的临时数据，用于做图像处理的数据
        std::byte* TempData;
        // 处理过的用于显示的图像数据
        std::byte* SrcData;
        // 处理过的用于显示的A扫图像数据
        std::byte* SrcDataLine;

        // 声程 数据大小
        int XDataSize;
        // 编码器 数据大小
        int YDataSize;
        // 声程实际显示 数据大小
        int XTakeDataSize;
        // 编码器实际显示 数据大小
        int YTakeDataSize;

        // 是否启用A扫波门
        bool IsGateOpen[3];
        // A扫波门长度位置信息
        float GatePosition[3];
        float GateWidth[3];
        // A扫波门的高度信息
        float GateHeight[3];

        // 色谱类型
        int ColorType;
        // 色谱
        QColormap* Colormap;

        // 当前鼠标位置
        QPoint MousePoint;
        // 图像中心距离窗口中心的偏移像素(拉伸后)
        QPoint CenterOffset;

        // A红色光标的 坐标
        QPoint AScanRedCrossPoint;
        // A蓝色光标的 坐标
        QPoint AScanBlueCrossPoint;

        // B红色光标的 坐标
        QPoint BScanRedCrossPoint;
        // B蓝色光标的 坐标
        QPoint BScanBlueCrossPoint;

        // 红色光标的测量值
        UValue RedCrossValue;
        // 蓝色光标的测量值
        UValue BlueCrossValue;

        // A扫光标的类型： 0：十字光标
        CrossType AScanCrossType;
        // B扫光标的类型： 0：十字光标  1：抛物线光标
        CrossType BScanCrossType;

        // 零点时间，单位us
        float ZeroTime;
        // 声速，单位 mm/us
        float SoundSpeed;
        // PCS，单位mm
        float PCS;
        // 编码器速度
        float EncoderVel;
        // 曲面时弦上的盲区高度
        float DeathHeader;

        // 当前补偿增益
        int CompensateGain;
    };

    class TofdBridge {
    public:
        TofdBridge();
        ~TofdBridge();

        bool OpenFile(QString filepath);

        // 获取参数信息
        QString GetParametersFormat();

        void ResetParameter();

        // 计算校准零点
        float CalculateAdjustZero(int ChannelID);
        // 计算曲面盲区高度
        float CalculateCurveLen(int ChannelID);
        // 计算X偏移
        bool CalculateXOffset(int ChannelID);

        void ParseChannelIndex();

        void LoadImageData();
        void LoadLayout();

        void Update();

        void HitCurChannel(QPoint point);
        void UpdateCrossInfo();

    public:
        SYSTEM_CONFIG_PARAS  m_SystemPara;
        CHANNEL_CONFIG_PARAS m_ChannelPara[CHANNEL_NUM];
        // 扫查长度
        int m_iScanLength;

        // 文件情况  0:910 920 1：930
        int m_nFileVersion;
        // 当前文件路径
        QString m_FilePath;

        // MDI视口区域
        QRect m_WindowRect;

        ViewRectangleInfo m_ViewRectangleInfo[CHANNEL_NUM];

        // 当前通道是否激活
        bool m_isChannelActive[CHANNEL_NUM];
        // 当前通道是否有效
        bool m_isChannelValid[CHANNEL_NUM];

        // 实际通道数量
        int m_nChannelNum;
        // 当前通道号
        int m_nCurChannel;
        // 当前通道号索引
        int m_nCurChannelIndex;

        // 显示布局方向 0:横向声程 1：纵向声程
        DirectionType m_nDisplayLayoutDirection;

        // 是否显示标尺
        bool m_isDisplayCaliper;
        // 是否显示缩放滚动条
        bool m_isDisplayScroll;
        // 是否显示波门
        bool m_isDisplayGate;
        // 是否显示测量线光标
        bool m_isDisplayCross;

        // 是否显示声程为时间
        bool m_isDisplayTime;
        // 是否修正曲率长度
        bool m_isAdjustRadius;

        // 当前红色光标的测量值
        UValue m_RedCrossValue;
        // 当前蓝色光标的测量值
        UValue m_BlueCrossValue;

        // 补偿增益(-50~50db)
        int m_nCompensateGain[CHANNEL_NUM];
        // X偏移
        double m_fXOffset[CHANNEL_NUM];

        // 校准深度
        double m_fAdjustDepth[CHANNEL_NUM];
        // 校准位置
        double m_fAdjustPosition[CHANNEL_NUM];
        // 校准pcs
        double m_fAdjustPCS[CHANNEL_NUM];
        // 校准pcs
        double m_fAdjustVelocity[CHANNEL_NUM];

        double m_fOffsetmaxlen;
        double m_fOffset[CHANNEL_NUM];
    };

} // namespace Union::TOFD_MULTI::__9x0
