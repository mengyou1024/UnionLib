#pragma once

namespace Union::TOFD_MULTI::__9x0 {

#pragma pack(push, _un_tofd_)
#pragma pack(1)

    typedef struct {
        int GatePos;    // 门位置  us
        int GateWidth;  // 门宽度  us
        int GateHeight; // 门高度  取值范围:0-255
        int GateLine;   // 门显示  0:不显示 1:显示
        int FFTEnable;  // 门内FFT 0:不显示FFT波形  1:显示FFT波形
    } GATE_SET_INFO;

    typedef struct
    {
        int ChanIndex; // 通道序号

        double Gain;       // 增益
        double Compensate; // 补偿增益
        double EchoRange;  // 声程
        double Delay;      // 延时
        int    Reject;     // 抑制

        GATE_SET_INFO GateInfo[3]; // 闸门配置参数

        int Filter;      // 滤波频带
        int ProbeSelect; // 选择发射和接收通道	低四位发，高四位收
        int EchoMode;    // 检波方式 0:射频 1:全波 2:正半波 3:负半波
        int PulseWidth;  // 脉冲宽度
        int Reserve0;    //

        double ProberZero;      // 探头零点
        double Velocity;        // 声速[m/s]
        int    Unit;            // 单位 0:us 1:mm
        int    ProberPhase;     // 探头相位
        int    ProberType;      // 探头类型  0--自发自收； 1--一发一收；2--穿透
        double ProberFreq;      // 探头频率
        double ProberAngle;     // 探头角度
        double ProberDimension; // 探头尺寸
        double ProberFront;     // 探头前沿

        double ProberCenterDistance; // PCS
        int    AdjustLine;           // 校准线
        double AdjustDepth;          // 调校深度
        double AdjustPosition;       // 调校位置
        double StartOrigin;          // 扫查起点(__未使用)

        int Reserve[13]; // 保留

    } CHANNEL_CONFIG_PARAS; // 通道配置参数

    typedef struct {
        int ChannelNumber; // 有效通道数目
        int Reserve0;

        /*仪器参数*/
        int Voltage;    // 发射电压	0:50V 1:100V  2:200V 3:300V
        int RepeatFreq; // 重复频率	[100,6000]

        /*对象*/
        double Reserve1;
        int    ScanLength;         // 扫查长度
        double WorkpieceThickness; // 工件厚度
        double WorkpieceWeldWidth; // 工件焊缝宽度
        int    ScanDirection;      // 扫查方向 0:正向 1:反向

        /*工件*/
        int    Reserve2;
        int    Reserve3;
        double Reserve4;
        double AdjustEchoRange; // TOFD校准时的声程

        /*扫查架 */
        int  Reserve5;
        char Reserve6[20];

        /*编码器*/
        int ScanMode;           // 编码器扫查方式 0--平行；1--垂直
        int Encoder2_Increment; // 编码器2 扫查增量类型 0:2mm 1:1mm 2:0.5mm 3:0.25mm
        int Encoder1_Increment; // 编码器1 扫查增量类型 0:2mm 1:1mm 2:0.5mm 3:0.25mm
        int Time_Increment;     // 时间编码 扫查增量类型 0:2mm 1:1mm 2:0.5mm 3:0.25mm

        int Reserve7;
        int Reserve8;
        /*选项*/
        int DispalyMode; // 显示布局
        int Reserve9;
        int Reserve10;
        int Reserve11;

        int Reserve12;
        int Reserve13;
        int Reserve14;
        int Reserve15;

        int Reserve16;
        int Reserve17;

        int Reserve18;
        int TofdState;    // TOFD扫查 0:开始 1:停止
        int AverageTimes; // 信号平均次数

        /*其它*/
        int Reserve19[2];
        int TriggerMode; // 0:编码器1触发 1:编码器2触发 2:时间触发
        int Reserve20;
        int Reserve21;
        int Reserve22;
        int Reserve23;

        int Reserve24;

        int Reserve25;
        int Reserve26;
        /*辅助计算*/
        int    ProbeFrontState; // 前沿测试 0:开始 1:停止
        int    Reserve27;
        int    Reserve28;
        int    Reserve29;
        double Reserve30;
        double Reserve31;
        double Encoder1_Resolution; // 编码器分辨率
        int    Reserve32;
        double Reserve33;

        double Encoder2_Resolution; // 编码器分辨率
        int    Reserve34;

        int CurrentChannel; // 当前选择通道号

        int ChannelFlag; // 通道标志位
        int Reserve35;
        int Reserve36;
        int WorkpieceBevelType; // 工件坡口类型
        int WorkpieceRadius;    // 工件曲率半径

        int Reserve[18]; // 保留

    } SYSTEM_CONFIG_PARAS; // 系统配置参数

    typedef struct {
        int GateAmp;      // 门一内最高回波幅度
        int GateDistance; // 门一内最高回波声程[ns]
    } GATE_GET_INFO;      // 获取的门内数据

#pragma pack(pop, _un_tofd_)

} // namespace Union::TOFD_MULTI::__9x0
