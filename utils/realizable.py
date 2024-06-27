import re
import os

current_dir_path = os.path.dirname(os.path.abspath(__file__))

realizable_type = {
    "ASCAN": os.path.join(current_dir_path, "../include_private/ASCAN/AScanType.hpp"),
    "TOFD_PE": os.path.join(current_dir_path, "../include_private/TOFD_PE/TofdPeType.hpp")
}


class Realizable:

    def __init__(self, type, dir: str, name):
        self.type = type
        if re.match(r"\d.+", dir):
            self.dir = "__" + dir
        else:
            self.dir = dir
        self.name = name
        self.class_name = ""
        self.pure_virtual_func = []

    def getClassBlock(self, buf: str):
        # 匹配类名
        self.class_name = re.search(r"class (.+?):", buf).group(1)
        # 匹配类区块
        class_bloc = re.search(
            r"class.+?public Union::Utils::ReadIntf.+?};", buf, flags=re.S)
        # 匹配需要实现的纯虚函数
        self.pure_virtual_func = re.findall(
            r"virtual[^;]+?= 0;", class_bloc.group(), flags=re.S)

    def create_file_buffer(self):
        virtual_func_buf = ""
        for f in self.pure_virtual_func:
            f = f.replace(' &', '& ')
            virtual_func_buf += f"        {f.replace('= 0;', 'override;')}" + "\n"
        header_buf = \
            f"""
#pragma once


#include "../{os.path.basename(realizable_type[self.type])}"

namespace Union::{self.dir} {{
    class   {self.name}: Union::Utils::{self.class_name} {{
    public:
{virtual_func_buf}
    }};
}}

"""
        virtual_func_buf = ""
        for f in self.pure_virtual_func:
            f = f.replace(' &', '& ')
            func_str = f.replace('virtual ', '').replace('= 0;', '{}')
            func_str = re.sub(r" ([^ ]+?\(.+?\))",
                              fr" {self.name}::\1", func_str)
            virtual_func_buf += f"    {func_str}" + "\n"

        cpp_buf = \
            f"""
#include "{self.name}.hpp"

namespace Union::{self.dir} {{
{virtual_func_buf}
}}

"""
        return header_buf, cpp_buf

    def realize(self):
        if self.type not in realizable_type.keys():
            raise Exception("type not found")

        # 查找对应的
        with open(realizable_type[self.type], "r", encoding="utf-8") as f:
            self.getClassBlock(f.read())

        header_file, cpp_file = self.create_file_buffer()
        header_path = f"{os.path.join(current_dir_path, '../include_private/', self.type, self.dir, self.name + '.hpp')}"
        cpp_path = f"{os.path.join(current_dir_path, '../src/', self.type, self.dir, self.name + '.cpp')}"
        for path, buf in [(header_path, header_file), (cpp_path, cpp_file)]:
            if not os.path.exists(os.path.abspath(path)):
                os.makedirs(os.path.dirname(os.path.abspath(path)), exist_ok=True)
            with open(path, 'w', encoding="utf-8") as f:
                f.write(buf)
