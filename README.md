# UnionLib

#### 一、单例构造器(InstanceBuilder)

单例构造器是一个只有静态方法的类，实现于`include_private/common/FileReaderSelector.hpp`和`src/commom/FileReaderSelector.cpp`, 其主要功能是用于文件读取原则器单例的注册和查找，解决类模板单例在

#### 二、文件读取选择器

文件读取选择器，是一个类模板(实现于`include_private/FileReaderSelector.hpp`中)，其模板参数有两个：

``` c++
template <RD_INTF T_INTF, const std::string_view &I_NAME>
class FileReaderSelector : public FileReaderSelectorIntf 
```

