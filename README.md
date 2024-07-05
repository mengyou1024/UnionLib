# UnionLib

#### 一、单例构造器(InstanceBuilder)

单例构造器是一个只有静态方法的类，实现于`include_private/common/FileReaderSelector.hpp`和`src/commom/FileReaderSelector.cpp`, 其主要功能是用于文件读取原则器单例的注册和查找，

主要目的是解决类模板单例(局部静态对象的单例方式)在`qmlRegisterType`注册类后由QML动态库调用重复初始化的问题。

该类仅用于文件读取选择器中。

#### 二、文件读取选择器(FileReaserSelector)

文件读取选择器，是一个类模板(实现于`include_private/FileReaderSelector.hpp`中)，其模板参数有两个：

``` c++
template <RD_INTF T_INTF, const std::string_view &I_NAME>
class FileReaderSelector;

T_INTF: 读取接口的抽象类(纯虚类)
I_NAME: 动态加载的UI界面名称，
    主界面:
         qrc:/qml/src/<I_NAME>/MainUI.qml
    控制界面:
         qrc:/qml/src/<I_NAME>/Control.qml
```

当前文件选择器有两个模板类: 

``` c++
inline static constexpr std::string_view ASCAN_I_NAME = "AScan";
using AScanFileSelector = Union::Utils::FileReaderSelector<AScanIntf, ASCAN_I_NAME>;

inline static constexpr std::string_view TOFD_PE_I_NAME = "TOFD_PE";
using TofdPeFileSelector = Union::Utils::FileReaderSelector<TofdPeIntf, TOFD_PE_I_NAME>;
```

分别用于读取A扫类型和TOFD/PE类型的文件

具体的文件后缀名打开方式，通过文件选择器模板类的`RegistReader`方法注册：

``` c++
using FRS_RFUNC = std::function<std::unique_ptr<T_INTF>(const std::wstring &)>;
bool RegistReader(std::string_view file_suffix, std::string_view describe, const FRS_RFUNC &func);
file_suffix: 文件后缀, 如*.dat *.json 等
describe: 文件类型描述
func: 从文件名读取至抽象接口的可调用对象
```

