
set( CMAKE_C_COMPILER           clang )
set( CMAKE_CXX_COMPILER         clang++ )
set( CMAKE_C_COMPILER_TARGET    ${HostCPU}-apple-darwin )
set( CMAKE_CXX_COMPILER_TARGET  ${HostCPU}-apple-darwin )
set( CMAKE_SYSTEM_NAME          macosx )
set( CMAKE_SYSTEM_PROCESSOR     ${HostCPU} )
set( CMAKE_OSX_SYSROOT          ${SDK_DIR} )
set( CMAKE_SYSROOT              ${SDK_DIR} )
set( CMAKE_CXX_FLAGS            ${CXX_FLAGS} )
