```shell
rm -rf build

# cmake 配置
cmake -G Ninja -B build -D CMAKE_BUILD_TYPE=Debug

# cmake 编译
cmake --build build

# 测试
./build/example
```