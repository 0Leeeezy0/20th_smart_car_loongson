# CMake版本
cmake_minimum_required(VERSION 3.5.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -pthread -Wall")  # 对于 C++ 编译器
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -Wall")      # 对于 C 编译器

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR loongson)
SET(TOOLCHAIN_DIR "/opt/loongson-gnu-toolchain-8.3-x86_64/")
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_DIR}bin/loongarch64-linux-gnu-g++)
set(CMAKE_C_COMPILER ${TOOLCHAIN_DIR}bin/loongarch64-linux-gnu-gcc)

# 工程名字
project(INCAR.RUN)

# 启动对C++14标准的支持
set(CMAKE_CXX_STANDARD 14)

# OpenCV
# 用find_package方法引入OpenCV库
# 也可以用dir的方法
# 但是find_package方法可移植性好
set(OpenCV_ROOT_DIR "/usr/")
include_directories( ${OpenCV_ROOT_DIR}include )

# PTHREAD
# 加上这个寻找pthread库比较保险，否则得另外链接
link_libraries(pthread)

# 引入头文件
include_directories(./include)
include_directories(./include/control)

# 添加源文件
aux_source_directory(./src SRC_FILES)
aux_source_directory(./lib LIB_FILES)
aux_source_directory(./lib/control LIB_CONTROL_FILES)

#编译时所需源文件
add_executable(${PROJECT_NAME}  ${SRC_FILES} ${LIB_FILES} ${LIB_CONTROL_FILES})

# 获取OpenCV库文件列表
file(GLOB OPENCV_LIBS ${OpenCV_ROOT_DIR}lib64/libopencv_*.so)

# 链接OpenCV的库文件
target_link_libraries(${PROJECT_NAME} ${OPENCV_LIBS})