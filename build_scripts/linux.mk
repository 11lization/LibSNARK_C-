
 

#
# 	Configure and Build shared libsnark
#
CXX_ARGUMENTS     += -fPIC -DCS_BINARY_FORMAT=LINUX_X86_64 
CXX_ARGUMENTS     += -I/usr/lib/llvm-12/include/c++/v1
CXX_ARGUMENTS     += -DMULTICORE=1 -fopenmp
CXX_ARGUMENTS     += -DUSING_JNI_WRAPPER
CXX_ARGUMENTS     += -I/usr/lib/jvm/java-11-openjdk-amd64/include/linux 
CXX_ARGUMENTS     += -I/usr/lib/jvm/java-11-openjdk-amd64/include/

configure_Snark : 
	cd ${BUILD_DIR} ; cmake  \
	-D CXX_FLAGS=" ${CXX_ARGUMENTS} " \
	-D LIBSNARK_SRC_DIR="${LIBSNARK_SRC_DIR}" \
	-D INSTALL_DIR="${INSTALL_DIR}/lib/" \
	-D TARGET="${target}" \
	-D LIB_NAME="Snark" \
	-S ${PWD}/build_scripts/  

make_Snark : 
	cd ${BUILD_DIR} ; make -j 8 
	cd ${BUILD_DIR} ; make install
	llvm-ranlib-12 ${INSTALL_DIR}/lib/libSnark.a
	@echo ; echo 
	@rm -fr   ${BUILD_DIR}/libSnark.a.ofiles 
	@mkdir -p ${BUILD_DIR}/libSnark.a.ofiles 
	cd ${BUILD_DIR}/libSnark.a.ofiles ; ar -x ${INSTALL_DIR}/lib/libSnark.a
	@echo
	clang++-12 -shared -Wall -Wextra -Wfatal-errors  -std=c++11 -Wno-deprecated ${USING_FLTO} -fPIC \
		${BUILD_DIR}/libSnark.a.ofiles/*.o  \
		-lgmp -lcrypto -lomp -z noexecstack \
		-o ${INSTALL_DIR}/lib/libSnark.so
