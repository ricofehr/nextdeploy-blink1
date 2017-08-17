cmake_minimum_required(VERSION 2.8.12)
project(nextdeploy-blink1)


# pick a version #
set(nextdeploy-blink1_VERSION 0.1)

include_directories(src)

# define bin folder as destination folder
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
# SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

set(blink1_SRCS
src/ndeployblink1/blink1/ndblink1.c
src/ndeployblink1/blink1/lib/blink1-lib.c)

set(core_SRCS
src/ndeployblink1/core/ndconfig.c
src/ndeployblink1/core/ndsignal.c)

set(ndeploy_SRCS
src/ndeployblink1/ndeploy/ndmodel.c
src/ndeployblink1/ndeploy/ndapiclient.c)

set(ndeployblink1_SRCS
${blink1_SRCS}
${core_SRCS}
${ndeploy_SRCS}
src/main.c)

set(blink1_HDRS
src/ndeployblink1/blink1/ndblink1.h
src/ndeployblink1/blink1/lib/blink1-lib.h
src/ndeployblink1/blink1/lib/blink1-lib-lowlevel-hidapi.h)

set(core_HDRS
src/ndeployblink1/core/ndconfig.h
src/ndeployblink1/core/ndsignal.h)

set(ndeploy_HDRS
src/ndeployblink1/ndeploy/ndmodel.h
src/ndeployblink1/ndeploy/ndapiclient.h)

set(ndeployblink1_HDRS
${blink1_HDRS}
${core_HDRS}
${ndeploy_HDRS})

add_executable(ndeployblink1 ${ndeployblink1_SRCS} ${ndeployblink1_HDRS})
add_definitions("-DUSE_HIDAPI")
target_link_libraries(ndeployblink1 curl jansson hidapi)
