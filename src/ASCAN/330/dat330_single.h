#ifndef DAT330_SINGLE_H
#define DAT330_SINGLE_H

#include "define_dat.h"
#include <QFile>
#include <QList>
#include <QObject>
#include <QTextStream>
#include <QTimer>
// #include <windows.h>
#include <cstdint>
#include <array>
#include <QPointF>
#include <QVector>
using byte = uint8_t;

// 两个类互相包含，在头文件中声明另一个类，在cpp文件中包含另一个类的头文件

#define SINGLE_C_SIZE_INT     (4)
#define SINGLE_C_SIZE_SHORT   (2)
#define SINGLE_C_SIZE_CHAR    (1)
#define SINGLE_C_TRUE         1
#define SINGLE_C_FALSE        0
#define SINGLE_C_SCREEN_WIDTH 500  // 308
#define SINGLE_C_COORHORIUNIT (48) //(28)
#define SINGLE_C_HORIUNIT     48   // 28
/*坐标水平总点数*/
#define SINGLE_C_COORWIDTH    (10 * SINGLE_C_COORHORIUNIT)

#define SINGLE_C_LEN_SAMP     ((SINGLE_C_COORWIDTH / SINGLE_C_SIZE_INT + 1) * SINGLE_C_SIZE_INT) /*波形参数*/

typedef struct
{
    u_int   Gate1PeakSampPoints;
    u_int   Gate2PeakSampPoints;
    u_int   Gate3PeakSampPoints;
    u_short Gate1PeakPos;
    u_short Gate2PeakPos;
    u_short Gate3PeakPos;
    u_short Gate1PeakAmp;
    u_short Gate2PeakAmp;
    u_short Gate3PeakAmp;
} GATE_PEAK_SINGLE;

typedef struct
{
    u_short Position;    // 位置
    u_short Width;       // 宽
    u_short Height;      // 高
    u_char  Type;        // 种类A/B
    u_char  AlarmEnable; // 警报
} GATE_PARA_SINGLE;

typedef struct
{
    u_char second;
    u_char minute;
    u_char hour;
    u_char date;
    u_char month;
    u_char day;
    u_char yearl;
    u_char yearh;
} TIME_CLOCK_SINGLE;

typedef struct
{
    unsigned int type;   // 类型0无，1单面，2双面，3T型
    unsigned int face;   // 探测面0A面，1B面，2C面
    unsigned int thick1; // 厚度1，工件厚度、翼板厚度<200.0mm;
    unsigned int thick2; // 厚度2，有效厚度、腹板厚度<200.0mm;
    unsigned int wide;   // 焊缝宽度、A面焊脚，< 100mm;
    unsigned int radius; // 根部半径、B面焊脚，< 100mm;
    unsigned int high1;  // 上端余高、A端余高，<100mm;
    unsigned int high2;  // 下端余高、B端余高，<100mm;
    unsigned int angle1; // 坡口角度、A坡口角，< 180;
    unsigned int angle2; // 坡口面角、B坡口角，< 90;
    unsigned int deep;   // 坡口深度，< 100mm;
    unsigned int blunt;  // 钝边高度，< 100mm;
    unsigned int space;  // 根部间隙，< 100mm
    unsigned int probe;

    unsigned int angle3; // 坡内角度，< 180;
    unsigned int angle4; // 坡内面角，< 180;
    unsigned int deep3;  // 坡内深度，< 100mm;
} WELD_PARA_SINGLE;

typedef struct
{
    unsigned short lpos; // 显示工件左端
    unsigned short rpos; // 显示工件右端
    unsigned short xpos; // 焊缝的中心水平
    unsigned short ypos; // 焊缝的中心垂直
    unsigned short size; // 显示工件的大小
    unsigned short wxpos;
    unsigned short wypos; //
    unsigned short dots;
} WELD_POSI_SINGLE;

typedef struct
{
    u_int nElapsedTime;      // 开机时长，以毫秒为单位
    u_int nStartElapsedTime; // 高16位开机时间，低16位开机时长，以分为单位
    u_int days;              // 距2000年1月1日的天数
    u_int denoise;           // 降噪次数
    u_int repeat;            // 重复频率
    u_int pulsewidth;        // 脉冲宽度
    u_int fill;              // 实心波
    u_int sys;               // d31-d29打印机，d28-26输入法,d25测试状态,d24采样状态，d23硬件波形，d22软件波形
    // d21允许光标，d20有无光标，d19-d16用户数(最大16)，d15-d12通道号(最大16)
    // d11按键声音d10报警声音,d9d8屏幕亮度，d7-d0输入法是否可选

    u_int option; // d31波形记录方式(0单幅1连续),d30启动连续存,d29-d22记录间隔(单位为0.1秒,最大25.0秒)
    // d21存储时是否记录缺陷波频谱，d20-d16当前选择的探伤工艺
    // d15d14颜色模式
    // D13曲线显示模式，0波幅，1dB值，此时自动调整增益
    // D12键盘锁
    // d11,d10门A门B内回波是否高于门高
    // d9=1A门变色,d8=1B门变色
    // d7=0峰值读数1前沿读数

    // U2: start
    u_int  displaymode;   // 显示方式,1:全屏,0非
    u_int  mainmenuitem;  // 当前主菜单项数,每页6项,最多24项
    u_int  samp_len;      // 显示回波长度
    u_int  coor_width;    // 坐标区长度
    u_int  coor_extern;   // 坐标区右方扩展点数
    u_int  hunit;         // 坐标区水平方向每格点数
    u_int  language;      // 语言
    u_int  unit;          // 单位
    u_char menu2page[24]; // 次级菜单页数,最多4页,每页4项
    // U2: end
    u_int prepare[4]; // 预留值

} SYSTEM_STATUS_SINGLE; // 整个系统的状态 68 BYTES

typedef struct
{
    u_int sys; // 仪器探头状态，31d30:频带宽度：2.5\5.0\10\wide，
    // d29d28检波方式：全波\正半波\负半波\射频 d27d26探头阻尼：50Q\400Q
    // d25d24发射电压：200V\400V ,d23-d20重复频率，
    // d19d18降噪次数：1-4次，d17d16波形:纵波/横波
    // d15-d12探头方式：单直/单斜/双晶纵波直/穿透/双晶纵波斜
    // d11-d8标准号
    // d7-d0功能

    u_int block; // 试块，d19-d0一位对应一种试块，1表示在试块选择菜单中会出现
    // d31-d24高8位为当前所选试块
    // u_short probe;		//探头,波型D3D2横波/众波，直/斜/双晶/穿透d1d0,组合探头d6-d4
    // d9d8核查用探头00组合探头01小于K1斜探头10单晶直探头11双晶直探头
    // d11初查用d12核查用组合探头d13小于K1斜探头d14单晶直探头d15双晶直探头=0未测1已测,

    u_int option; // d31－d28当量标准:0000母线/从上到下，
    // d27d26当量显示：%/dB值/孔径,d25d24标度：垂直\水平\距离
    // d23-d16参量锁
    // d15-d9参量步长,d15d14基准增益，d13d12调节增益，d11声程，d10延时，d9声速
    // d8-d5参量类别，d8:基准增益\调节增益，d7:门A\门B，D6D5：门位\宽\高
    // d4B门类型
    // d2-d0曲线数目

    u_int status; // d31-d24仪器性能测试天数，D23-d18探头测试天数(零点、K值、DAC)，
    // d17－d14灵敏度测试天数，d13-d7基准波高(0-128)
    // d6-d0:d0:零点声速，D1：K值\折射角，D2：DAC,D3:AVG,
    // 3300+: start
    u_int externstatus; // d0 探测面：内侧/外侧
    // d1 自动增益大小
    // d2 报告格式 d3 模拟或数字探伤(0数字，1模拟),d4-d10基准高度(0-128),d11,12通用、标准,其他。
    // d13,d14 大平底，圆柱面，空心轴
    // 3300+: start
    u_int prepare[4]; // 预留值

} CHANNEL_STATUS_SINGLE; // 每个通道的状态 36 BYTES

typedef struct
{
    u_int Range;
    u_int OldRange; // 展宽前的声程
    u_int OldDelay; // 展宽前的延时
    u_int Delay;

    u_int wavepara[4]; // 波形的四个参数(距离,水平,垂直,高度/位置,高度,位置,高度)

    u_short Thick;
    u_short Diameter; // 直径

    u_short Offset;
    u_short Speed;     /*试件声速：	m/s	*/
    u_short Angle;     /*折射角：60o	*/
    u_short Forward;   /*探头前沿长度	*/
    u_short Frequence; /*探头频率：2.5Mhz	*/

    u_short Reject;

    u_short SurfGain; /*表面补偿增益	*/
    u_short BaseGain; /*基本增益	*/

    u_short DepthGain; // 深度补偿值
    short   CompGain;  /*扫查增益	*/

    short lineGain[6]; // 六条线偏移

    short gatedB;
    short depth; // 缺陷高度

    u_short Crystal_l; /*晶片尺寸	*/
    u_short Crystal_w; /*晶片尺寸	*/

    // 3300+: start
    u_short PoreLong;     // 横孔长度
    u_short PoreDiameter; // 横孔直径
    u_short PoreDepth[2]; // 2横孔孔深
    // 3300+: end
    u_int prepare[4]; // 预留值

} CHANNEL_PARAMETER_SINGLE; // 100 BYTES

typedef struct
{
    u_int  wavepara[4];           // 波形的四个参数(距离,水平,垂直,高度/位置,高度,位置,高度)
    u_char sampbuff[C_COORWIDTH]; // 包络线
} WAVEPARA_SINGLE;

typedef struct
{
    u_short dac_db[C_COORWIDTH];   // 母线
    u_short dac_high[C_COORWIDTH]; // 曲线高度
} DAC_LINE_SINGLE;

typedef struct
{
    u_short db[10];
    u_short dist[10];
    short   num;
    u_short diameter; // 反射体直径
    u_short length;   // 反射体长度
} DAC_SINGLE;

class dat330_single {
public:

    int C_TEST_DAC    = 2;
    int C_TEST_AVG    = 3;
    int C_MAKE_DACAVG = 4;

    short dac_line[500] = {};
    short dac_gate[600] = {};

    int Last_Test_Dist = 0;

    std::array<QVector<QPointF>, 3> DrawDac();


    uint32_t m_unit = 0;
    uint32_t m_sys = {};
    uint32_t m_ch_status = {};
    uint32_t m_ch_option = {};
    uint32_t m_ch_sys = {};
    uint32_t m_ch_Range = {};
    uint32_t m_ch_Delay = {};
    uint16_t m_ch_Speed = {};
    uint16_t m_ch_Crystal_w = {};
    uint16_t m_ch_Crystal_l = {};
    uint16_t m_ch_Frequence = {};
    std::array<int16_t, 6> m_ch_lineGain = {};
    uint16_t m_ch_BaseGain = {};
    int16_t m_ch_gatedB = {};
    std::array<uint16_t, 10> m_dac_db = {};
    std::array<uint16_t, 10> m_dac_dist = {};
    int16_t m_dac_num = {};
    double m_range_a = {};
private:
    int   CalcuDac();
    int   GetMiddleValue(int dist1, int db1, int dist2, int db2, int dist);
    int   GetLine_TwoDot(short line[], int dist1, int dist2);
    void  GetDacLine();
    float Mexpf(float ex, u_int len); // 指数函数，得到e^ex,位数不大于len×8
    std::array<std::array<int, 500>, 10> dac_points = {};
};

#endif // DAT330_SINGLE_H
