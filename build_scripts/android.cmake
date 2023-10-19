

set ( ANDROID_NDK_HOME        ${ANDROID_SYSROOT}/../../../../../ )
set ( TOOLCHAIN_ABS_PATH      ${ANDROID_SYSROOT}/../bin/ )


set ( CMAKE_C_COMPILER        ${TOOLCHAIN_ABS_PATH}${CC} )
set ( CMAKE_CXX_COMPILER      ${TOOLCHAIN_ABS_PATH}${CXX} )
set ( CMAKE_AR                ${TOOLCHAIN_ABS_PATH}${AR} )
set ( CMAKE_LINKER            ${TOOLCHAIN_ABS_PATH}llvm-link )
set ( CMAKE_RANLIB            /usr/bin/true )
set ( CMAKE_SYSTEM_PROCESSOR  ${ARCH} )
set ( CMAKE_SYSTEM_NAME       Android )
set ( CMAKE_SYSROOT           ${ANDROID_SYSROOT} )
set ( CMAKE_CXX_FLAGS         "${CXX_FLAGS}" )

include_directories( ${LIBSNARK_SRC_DIR}/../depends/include/android/${ARCH} )