## 一、配置说明

相关配置都定义在 `inc/memcheck/memcheck_config.h` 头文件中。

### 1、Debug

输出打印控制。

### 2、Function

memcheck 所支持的功能，默认是支持 memcheck 功能，即内存泄漏检测。

- `MEMCHECK_BACKTRACE_ON`

  该功能是用来回溯发生内存泄漏时，malloc 时的函数调用过程，比如：xxx -> xxx -> xxx -> malloc。

  对于 Keil + Arm Compiler 6 来说，需要在 Options -> C/C++ -> Misc Controls 中添加编译器标志位：

  ```shell
  -fno-omit-frame-pointer
  ```

  对于 GCC 来说，目前暂未支持。

### 3、Function Config

- `MEMCHECK_TRACE_MAX`

  内存泄漏检测的最大个数，其大小取决内存大小和频繁 malloc 的次数，数值越大，所使用内存就越大。

- `MEMCHECK_BACKTRACE_MAX`

  仅在 `MEMCHECK_BACKTRACE_ON` 启用时生效，表示最大可以追溯的内存调用次数。

### 4、Arch

当前支持的架构，具体参考 memcheck_config.h 文件。

### 二、测试环境说明

#### RT-Thread 5.0.2

架构：`MEMCHECK_ARCH_CORTEX_M4`。

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

架构：`MEMCHECK_ARCH_SIMULATOR`。

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
