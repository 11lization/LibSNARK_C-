
set( CMAKE_C_COMPILER           clang )
set( CMAKE_CXX_COMPILER         clang++ )

set( CMAKE_OSX_SYSROOT          ${SDK_DIR} )
set( CMAKE_SYSROOT              ${SDK_DIR} )

if(${iPhoneOS})
    set( CMAKE_SYSTEM_PROCESSOR     arm64 )
    set( CMAKE_C_COMPILER_TARGET    arm64-apple-ios )
    set( CMAKE_CXX_COMPILER_TARGET  arm64-apple-ios )
    set( CMAKE_SYSTEM_NAME          iOS )
    set( CMAKE_CXX_FLAGS            ${CXX_FLAGS} )
endif()

if(${iPhoneSimulator})
    set( CMAKE_SYSTEM_PROCESSOR     ${HostCPU} )
    set( CMAKE_C_COMPILER_TARGET    ${HostCPU}-apple-ios15.0-simulator )
    set( CMAKE_CXX_COMPILER_TARGET  ${HostCPU}-apple-ios15.0-simulator )
    set( CMAKE_SYSTEM_NAME          iOSimulator )
    set( CMAKE_CXX_FLAGS            "${CXX_FLAGS} -Xpreprocessor -triple -Xpreprocessor ${HostCPU}-apple-ios15.0-simulator" ) 
endif()

include_directories( ${LIBSNARK_SRC_DIR}/../depends/include/ios )

message(STATUS "CMAKE_SYSTEM_PROCESSOR    : ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "CMAKE_C_COMPILER_TARGET   : ${CMAKE_C_COMPILER_TARGET}")
message(STATUS "CMAKE_CXX_COMPILER_TARGET : ${CMAKE_CXX_COMPILER_TARGET}")
message(STATUS "CMAKE_SYSTEM_NAME         : ${CMAKE_SYSTEM_NAME}")
message(STATUS "CMAKE_CXX_FLAGS           : ${CMAKE_CXX_FLAGS}")
