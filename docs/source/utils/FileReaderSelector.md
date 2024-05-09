# 选择器

## 读取接口概念

```{doxygenconcept} Union::Utils::RD_INTF
:project: UnionLib
```

## 文件读取选择器

### 派生类

目前文件选择器只有两个派生类: `AScanFileSelector`和`TofdPeFileSelector`

### 使用示例

``` c++
// 注册文件读写接口
Union::AScan::AScanFileSelector::Instance().RegistReader("*.ldat", "390N、T8连续图像", Union::__390N_T8::LDAT::FromFile);
Union::AScan::AScanFileSelector::Instance().RegistReader("*.json", "390N、T8单幅图像", Union::__390N_T8::T8_390N_JSON::FromFile);
Union::AScan::AScanFileSelector::Instance().RegistReader("*.das", "330单幅图像", Union::__330::DASType::FromFile);
Union::AScan::AScanFileSelector::Instance().RegistReader("*.DAT", "330连续图像", Union::__330::DATType::FromFile);
Union::AScan::AScanFileSelector::Instance().RegistReader("*.cod", "330串口数据", Union::__330::Serial_330::FromFile);

QVariantList folderListModel_nameFilter;
QVariantList filedialog_nameFilter;
QVariantMap  mainUi_map;
// 获取QML中需要使用的变量
filedialog_nameFilter.push_back("所有文件 (*.*)");
mainUi_map.insert(Union::AScan::AScanFileSelector::Instance().GetUINameMap().toVariantMap());
folderListModel_nameFilter += Union::AScan::AScanFileSelector::Instance().GetFileListModelNameFilter().toVariantList();
filedialog_nameFilter += Union::AScan::AScanFileSelector::Instance().GetFileNameFilter().toVariantList();
// 注册变量到QML的全局环境
context->setContextProperty("FOLDERLISTMODEL_NAMEFILTER", folderListModel_nameFilter);
context->setContextProperty("FILEDIALOG_NAMEFILTER", filedialog_nameFilter);
context->setContextProperty("MAINUI_MAP", mainUi_map);
```

### 类定义
```{doxygenclass} Union::Utils::FileReaderSelector
:project: UnionLib
:members:
```

