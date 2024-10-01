## 一、功能说明

- **MEMCHECK_BACKTRACE_ON**

  对于 Keil + Arm Compiler 6 来说，需要在 Options -> C/C++ -> Misc Controls 中添加编译器标志位：

  ```shell
  -fno-omit-frame-pointer -funwind-tables -fno-optimize-sibling-calls
  ```

  对于 GCC 来说，目前暂未支持

### 二、测试环境说明

#### RT-Thread 5.0.2

测试环境有两种：

1. Windows + env + Scons，编译器版本是 `gcc version 10.3.1 20210824 (release) (GNU Arm Embedded Toolchain 10.3-2021.10)`，固件下载工具是 `STM32CubeProgrammer`。

   ```shell
   # 编译 (env)
   scons
   
   # 反汇编 (env)
   arm-none-eabi-addr2line.exe -Cfie rt-thread.elf <打印的 backtrace 地址>
   ```

2. Keil + Arm Compiler 6（armclang）。

   ```shell
   # 生成 Keil 工程 (此时默认是使用 Arm Compiler 5, 记得切换为 Arm Compiler 6)
   scons --target=mdk5
   
   # 在 Keil 的 Options -> User -> After Build/Rebuild 中添加上
   fromelf --text -c -o "$L@L.txt" "#L"
   
   # 生成的文件位于 build/keil/Obj/rt-thread.txt
   # 根据打印的 backtrace 地址在 rt-thread.txt 里面查找即可 (如果是 Thumb 指令, 记得将地址 - 1 后再查找)
   ```

#### Linux

测试用例编译。

```shell
rm -rf build

# cmake 配置
cmake -G Ninja -B build -D CMAKE_BUILD_TYPE=Debug

# cmake 编译
cmake --build build

# 测试
./build/example

# 反汇编 (MSYS2)
addr2line.exe -Cfie ./build/example.exe <打印的 backtrace 地址>
```
