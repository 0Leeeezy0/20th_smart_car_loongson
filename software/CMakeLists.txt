# 设置CMake版本
cmake_minimum_required(VERSION 3.5)
# 设置编译参数
set(CMAKE_CXX_STANDARD 17)  # 设置C++标准
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -pthread -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall")

# 设置交叉编译器路径
set(CROSS_COMPILER_ROOT_DIR /opt/loongson-gnu-toolchain-8.3-x86_64-loongarch64-linux-gnu-rc1.4/bin/)
set(CMAKE_C_COMPILER ${CROSS_COMPILER_ROOT_DIR}loongarch64-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER ${CROSS_COMPILER_ROOT_DIR}loongarch64-linux-gnu-g++)

# 工程名
project(LOONGSON)

# 设置OpenCV根路径
set(OPENCV_ROOT_DIR /usr/)
# 添加OpenCV头文件路径
include_directories(${OPENCV_ROOT_DIR}include/opencv4)

# 链接多线程库
link_libraries(pthread)

# 添加用户和库头文件
include_directories(./include)

# 查找源文件
aux_source_directory(./src SRC_LISTS)
aux_source_directory(./lib LIB_LISTS)

# 添加源文件进行编译
add_executable(${PROJECT_NAME} ${SRC_LISTS} ${LIB_LISTS})

# 寻找OpenCV库文件
file(GLOB OPENCV_LIBS ${OPENCV_ROOT_DIR}lib64/libopencv_*.so)

# 链接OpenCV库
target_link_libraries(${PROJECT_NAME} ${OPENCV_LIBS})
