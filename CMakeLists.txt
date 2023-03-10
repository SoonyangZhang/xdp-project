PROJECT(project)
cmake_minimum_required(VERSION 2.6)
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall  -O2")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -O2")
SET(CMAKE_CXX_FLAGS "-fPIC")


add_definitions(-D RTE_CACHE_LINE_SIZE=64)
add_definitions(-D CPU_SETSIZE)

#for libbpf
set(LIBBPF_DIR /build/root/usr)
include_directories(${LIBBPF_DIR}/include)
LINK_DIRECTORIES(${LIBBPF_DIR}/lib64)

include_directories(${CMAKE_SOURCE_DIR}/)
include_directories(${CMAKE_SOURCE_DIR}/lib/buf/)
include_directories(${CMAKE_SOURCE_DIR}/lib/log/)
include_directories(${CMAKE_SOURCE_DIR}/lib/util/)
include_directories(${CMAKE_SOURCE_DIR}/lib/thirdparty/ncx_mempool/)
include_directories(${CMAKE_SOURCE_DIR}/lib/thirdparty/dpdk/eal/include)
include_directories(${CMAKE_SOURCE_DIR}/lib/thirdparty/dpdk/eal/include)
include_directories(${CMAKE_SOURCE_DIR}/lib/thirdparty/dpdk/eal/linux/include)
include_directories(${CMAKE_SOURCE_DIR}/lib/thirdparty/dpdk/eal/x86/include)
include_directories(${CMAKE_SOURCE_DIR}/lib/thirdparty/dpdk/eal/common)
include_directories(${CMAKE_SOURCE_DIR}/lib/thirdparty/dpdk/timer/)
include_directories(${CMAKE_SOURCE_DIR}/lib/thirdparty/dpdk/config)

set(util_LIB
${CMAKE_SOURCE_DIR}/lib/util/packet.c
${CMAKE_SOURCE_DIR}/lib/log/my_log.c
${CMAKE_SOURCE_DIR}/lib/thirdparty/dpdk/eal/common/eal_common_config.c
${CMAKE_SOURCE_DIR}/lib/thirdparty/dpdk/eal/common/eal_common_errno.c
${CMAKE_SOURCE_DIR}/lib/thirdparty/dpdk/eal/common/eal_common_lcore.c
${CMAKE_SOURCE_DIR}/lib/thirdparty/dpdk/eal/common/eal_common_timer.c
${CMAKE_SOURCE_DIR}/lib/thirdparty/dpdk/eal/common/eal_common_thread.c
${CMAKE_SOURCE_DIR}/lib/thirdparty/dpdk/eal/common/rte_random.c
${CMAKE_SOURCE_DIR}/lib/thirdparty/dpdk/eal/linux/eal.c
${CMAKE_SOURCE_DIR}/lib/thirdparty/dpdk/eal/linux/eal_thread.c
${CMAKE_SOURCE_DIR}/lib/thirdparty/dpdk/eal/linux/eal_timer.c
${CMAKE_SOURCE_DIR}/lib/thirdparty/dpdk/eal/linux/eal_unix_timer.c
${CMAKE_SOURCE_DIR}/lib/thirdparty/dpdk/eal/x86/rte_cycles.c
${CMAKE_SOURCE_DIR}/lib/thirdparty/dpdk/eal/x86/rte_cpuflags.c
${CMAKE_SOURCE_DIR}/lib/thirdparty/dpdk/eal/x86/rte_spinlock.c
${CMAKE_SOURCE_DIR}/lib/thirdparty/dpdk/timer/rte_timer.c
)
set(mem_LIB
${CMAKE_SOURCE_DIR}/lib/buf/buf.c
${CMAKE_SOURCE_DIR}/lib/thirdparty/ncx_mempool/ncx_slab.c
)

add_library(util STATIC ${util_LIB})
add_library(mem STATIC ${mem_LIB})


set(EXECUTABLE_NAME "time")
add_executable(${EXECUTABLE_NAME} 
${CMAKE_SOURCE_DIR}/sample/time_test.c
)
target_link_libraries(${EXECUTABLE_NAME} util)

set(EXECUTABLE_NAME "xdpsock")
add_executable(${EXECUTABLE_NAME} 
${CMAKE_SOURCE_DIR}/sample/xdpsock_user.c
)
target_link_libraries(${EXECUTABLE_NAME} util bpf pthread)

set(EXECUTABLE_NAME "memtest")
add_executable(${EXECUTABLE_NAME} 
${CMAKE_SOURCE_DIR}/sample/mempool_test.c
)
target_link_libraries(${EXECUTABLE_NAME} mem util)

set(EXECUTABLE_NAME "ncxslab")
add_executable(${EXECUTABLE_NAME} 
${CMAKE_SOURCE_DIR}/sample/ncxslab_test.c
)
target_link_libraries(${EXECUTABLE_NAME} mem util)
#cmake .. -DCMAKE_C_COMPILER=/usr/bin/clang -DCMAKE_CXX_COMPILER=/usr/bin/clang++