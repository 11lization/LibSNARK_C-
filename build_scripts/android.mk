
ANDROID_NDK_PATH          :=${HOME}/Library/Android/sdk/ndk/20.0.5594570
ANDROID_SYSROOT           :=${ANDROID_NDK_PATH}/toolchains/llvm/prebuilt/darwin-x86_64/sysroot
TOOLCHAIN_PATH            :=${ANDROID_NDK_PATH}/toolchains/llvm/prebuilt/darwin-x86_64/bin
ANDROID_API_LEVEL_ARM32   :=18
ANDROID_API_LEVEL         :=23



CXX_ARGUMENTS       += -DUSING_ANDROID_JNI_WRAPPER -DUSING_JNI_WRAPPER 
CXX_ARGUMENTS       += -DNO_PROCPS -fPIC 
CXX_ARGUMENTS       += -DMULTICORE=1 -fopenmp 

configure_Snark : configure_Snark_arm32 configure_Snark_arm64 configure_Snark_x86_64 ;
make_Snark      : make_Snark_arm32      make_Snark_arm64      make_Snark_x86_64 ;

configure_Snark_arm32 :  ;
	mkdir -p ${BUILD_DIR}_arm32
	cd ${BUILD_DIR}_arm32 ; cmake  \
	-D TARGET="${target}" \
	-D ARCH=armv7a \
	-D CC="armv7a-linux-androideabi${ANDROID_API_LEVEL_ARM32}-clang" \
	-D CXX="armv7a-linux-androideabi${ANDROID_API_LEVEL_ARM32}-clang++" \
	-D AR="llvm-ar" \
	-D ANDROID_SYSROOT="${ANDROID_SYSROOT}" \
	-D CXX_FLAGS=" ${CXX_ARGUMENTS} -DCS_BINARY_FORMAT=ANDROID_ARM32 " \
	-D LIBSNARK_SRC_DIR="${LIBSNARK_SRC_DIR}" \
	-D INSTALL_DIR="${INSTALL_DIR}/lib/" \
	-D LIB_NAME="Snark_arm32" \
	-S ${PWD}/build_scripts/  

configure_Snark_arm64 :  ;
	mkdir -p ${BUILD_DIR}_arm64
	cd ${BUILD_DIR}_arm64 ; cmake  \
	-D TARGET="${target}" \
	-D ARCH=aarch64 \
	-D CC="aarch64-linux-android${ANDROID_API_LEVEL}-clang" \
	-D CXX="aarch64-linux-android${ANDROID_API_LEVEL}-clang++" \
	-D AR="aarch64-linux-android-ar" \
	-D ANDROID_SYSROOT="${ANDROID_SYSROOT}" \
	-D CXX_FLAGS=" ${CXX_ARGUMENTS} -DCS_BINARY_FORMAT=ANDROID_ARM64 " \
	-D LIBSNARK_SRC_DIR="${LIBSNARK_SRC_DIR}" \
	-D INSTALL_DIR="${INSTALL_DIR}/lib/" \
	-D LIB_NAME="Snark_arm64" \
	-S ${PWD}/build_scripts/  

configure_Snark_x86_64 :  ;
	mkdir -p ${BUILD_DIR}_x86_64
	cd ${BUILD_DIR}_x86_64 ; cmake  \
	-D TARGET="${target}" \
	-D ARCH=x86_64 \
	-D CC="x86_64-linux-android${ANDROID_API_LEVEL}-clang" \
	-D CXX="x86_64-linux-android${ANDROID_API_LEVEL}-clang++" \
	-D AR="x86_64-linux-android-ar" \
	-D ANDROID_SYSROOT="${ANDROID_SYSROOT}" \
	-D CXX_FLAGS=" ${CXX_ARGUMENTS}  -DCS_BINARY_FORMAT=ANDROID_x86_64 " \
	-D LIBSNARK_SRC_DIR="${LIBSNARK_SRC_DIR}" \
	-D INSTALL_DIR="${INSTALL_DIR}/lib/" \
	-D LIB_NAME="Snark_x86_64" \
	-S ${PWD}/build_scripts/  



make_Snark_arm32 : ;
	cd ${BUILD_DIR}_arm32 ; make ${MAKE_JOBS}
	cd ${BUILD_DIR}_arm32 ; make install

make_Snark_arm64 : ;
	cd ${BUILD_DIR}_arm64 ; make ${MAKE_JOBS}
	cd ${BUILD_DIR}_arm64 ; make install

make_Snark_x86_64 : ;
	cd ${BUILD_DIR}_x86_64 ; make ${MAKE_JOBS}
	cd ${BUILD_DIR}_x86_64 ; make install





TEST_APP_DIR 			:=${PWD}/test_android_app
JNILIBS_32_DIR          :=${TEST_APP_DIR}/app/src/main/jniLibs/armeabi-v7a
JNILIBS_64_DIR          :=${TEST_APP_DIR}/app/src/main/jniLibs/arm64-v8a
JNILIBS_x86_64_DIR      :=${TEST_APP_DIR}/app/src/main/jniLibs/x86_64

Sample_App_Snark :  ${TEST_APP_DIR} \
					${JNILIBS_32_DIR}/libc++_shared.so \
					${JNILIBS_64_DIR}/libc++_shared.so \
					${JNILIBS_x86_64_DIR}/libc++_shared.so \
					${BUILD_DIR}_arm32/libunwind.a.ofiles \
					build_static_lib \
					${JNILIBS_32_DIR}/libSnark.so \
					${JNILIBS_64_DIR}/libSnark.so \
					${JNILIBS_x86_64_DIR}/libSnark.so ;

${TEST_APP_DIR} : ;
	git clone -b android https://github.com/snp-labs/libsnark-optimization-test-Apps.git  ${TEST_APP_DIR}
	@echo 

${JNILIBS_32_DIR}/libc++_shared.so ${JNILIBS_64_DIR}/libc++_shared.so ${JNILIBS_x86_64_DIR}/libc++_shared.so :  ;
	@echo 
	cp ${ANDROID_NDK_PATH}/sources/cxx-stl/llvm-libc++/libs/armeabi-v7a/libc++_shared.so ${JNILIBS_32_DIR}/
	cp ${ANDROID_NDK_PATH}/sources/cxx-stl/llvm-libc++/libs/arm64-v8a/libc++_shared.so   ${JNILIBS_64_DIR}/
	cp ${ANDROID_NDK_PATH}/sources/cxx-stl/llvm-libc++/libs/x86_64/libc++_shared.so      ${JNILIBS_x86_64_DIR}/
	@echo 

${BUILD_DIR}_arm32/libunwind.a.ofiles : ;
	@echo 
	mkdir -p ${BUILD_DIR}_arm32/libunwind.a.ofiles
	cd ${BUILD_DIR}_arm32/libunwind.a.ofiles ; ${TOOLCHAIN_PATH}/llvm-ar -x ${ANDROID_NDK_PATH}/sources/cxx-stl/llvm-libc++/libs/armeabi-v7a/libunwind.a
	@echo 

build_static_lib :
	@echo 
	@make build_target_lib ${BUILD_TYPE} target=android
	@echo 

${JNILIBS_32_DIR}/libSnark.so : ${INSTALL_DIR}/lib/libSnark_arm32.a ;
	@echo ; echo 
	@rm -fr    ${BUILD_DIR}_arm32/libSnark.a.ofiles 
	@mkdir -p ${BUILD_DIR}_arm32/libSnark.a.ofiles 
	cd ${BUILD_DIR}_arm32/libSnark.a.ofiles ; ${TOOLCHAIN_PATH}/llvm-ar -x ${INSTALL_DIR}/lib/libSnark_arm32.a
	${TOOLCHAIN_PATH}/armv7a-linux-androideabi${ANDROID_API_LEVEL_ARM32}-clang++ \
		-shared -Wall -Wextra -Wfatal-errors  -std=c++11 -Wno-deprecated -flto -fuse-ld=gold -fPIC \
		${BUILD_DIR}_arm32/libSnark.a.ofiles/*.o  \
		${BUILD_DIR}_arm32/libunwind.a.ofiles/*.o \
		-L${JNILIBS_32_DIR} -lgmp -lcrypto_sha256 -lc++ \
		-lomp  -llog -ldl -lc -lm \
		-o ${JNILIBS_32_DIR}/libSnark.so

${JNILIBS_64_DIR}/libSnark.so : ${INSTALL_DIR}/lib/libSnark_arm64.a ;
	@echo ; echo 
	@rm -fr   ${BUILD_DIR}_arm64/libSnark.a.ofiles 
	@mkdir -p ${BUILD_DIR}_arm64/libSnark.a.ofiles 
	cd ${BUILD_DIR}_arm64/libSnark.a.ofiles ; ${TOOLCHAIN_PATH}/llvm-ar -x ${INSTALL_DIR}/lib/libSnark_arm64.a
	${TOOLCHAIN_PATH}/aarch64-linux-android${ANDROID_API_LEVEL}-clang++ \
		-shared -Wall -Wextra -Wfatal-errors  -std=c++11 -Wno-deprecated -flto -fuse-ld=gold -fPIC \
		${BUILD_DIR}_arm64/libSnark.a.ofiles/*.o  \
		-L${JNILIBS_64_DIR} -lgmp -lcrypto -lc++ \
		-lomp  -llog -ldl -lc -lm \
		-o ${JNILIBS_64_DIR}/libSnark.so

${JNILIBS_x86_64_DIR}/libSnark.so : ${INSTALL_DIR}/lib/libSnark_x86_64.a ;
	@echo ; echo 
	@rm -fr   ${BUILD_DIR}_x86_64/libSnark.a.ofiles 
	@mkdir -p ${BUILD_DIR}_x86_64/libSnark.a.ofiles 
	cd ${BUILD_DIR}_x86_64/libSnark.a.ofiles ; ${TOOLCHAIN_PATH}/llvm-ar -x ${INSTALL_DIR}/lib/libSnark_x86_64.a
	${TOOLCHAIN_PATH}/x86_64-linux-android${ANDROID_API_LEVEL}-clang++ \
		-shared -Wall -Wextra -Wfatal-errors  -std=c++11 -Wno-deprecated -flto -fuse-ld=gold -fPIC \
		${BUILD_DIR}_x86_64/libSnark.a.ofiles/*.o  \
		-L${JNILIBS_x86_64_DIR} -lgmp -lcrypto -lc++ \
		-lomp  -llog -ldl -lc -lm \
		-o ${JNILIBS_x86_64_DIR}/libSnark.so
