
BUILD_DIR       :=${PWD}/../build_workspace/test

# select target
OS              :=$(shell uname -s | tr A-Z a-z)
CPU             :=$(shell uname -m)
TARGET          :=${OS}

ifeq ($(strip $(OS)),linux)
CXX            	:=clang++-12
else
CXX             :=clang++
endif


CXX_FLAGS 		+= -Wall -Wextra -Wfatal-errors -stdlib=libc++ -std=c++11  -O2 -g 
LD_FLAGS       	+= -flto -fPIC 
LD_LIBS 		:= -lgmp -lgmpxx -lssl -lcrypto  -lc++ -lomp -ldl -lpthread
LIBSNARK 		:= ../lib/${TARGET}_${BUILD_TYPE}/lib/libSnark.a 

TEST_INCLUDE    =   -I../
TEST_INCLUDE   	+=  -I../depends/CircuitBuilder 
TEST_INCLUDE    +=  -I../depends/json_tree

MISC_INCLUDE    =   -I../
MISC_INCLUDE    +=  -I../depends/gmp_bigInt 
MISC_INCLUDE    +=  -I../depends/ec 
MISC_INCLUDE    +=  -I../depends/hashes 
MISC_INCLUDE    +=  -I../depends/json_tree
MISC_INCLUDE    +=  -I../depends/CircuitBuilder/util 

TEST_EXEC 		:=${BUILD_DIR}/test.${BUILD_TYPE}
MISC_TEST_EXEC	:=${BUILD_DIR}/misc_function_test.${BUILD_TYPE}

LIB_INFO :=${BUILD_DIR}/../darwin_path.info



debug :   
	make BUILD_TYPE=debug all 

release : 
	make BUILD_TYPE=release all 

all : ${BUILD_DIR} ${OS}_info_banner compile_${OS}_test run_all

# compile_${OS}_misc_function_test 


${BUILD_DIR} :
	@mkdir -p ${BUILD_DIR}

linux_info_banner :
	@echo
	@echo Target : ${TARGET}
	@echo


darwin_info_banner : ${BUILD_DIR} ${LIB_INFO} ;
	@echo
	@echo Target : ${TARGET}
	@echo Installation directories :
	@source ${LIB_INFO} ; echo "\tOpenssl  : $${OpenSSL}"
	@source ${LIB_INFO} ; echo "\tGMP      : $${GMP}"
	@source ${LIB_INFO} ; echo "\tOMP      : $${OMP}"
	@source ${LIB_INFO} ; echo "\tOPENJDK  : $${OpenJDK}"
	@echo 

${LIB_INFO} :
	@mkdir -p build_workspace
	@echo
	@echo " --> Searching for [ OpenSSL , GMP , OMP , OpenJDK ] installation path on MacOS"
	@echo
	python ../build_scripts/make_helper.py --action=darwin_path --export_script_file=$@
	@echo

 
compile_linux_test  :  test.cpp ;
	@echo 
	${CXX} ${CXX_FLAGS} \
	test.cpp  \
	${TEST_INCLUDE} \
	${LD_FLAGS} \
	-fuse-ld=gold ${LIBSNARK} \
	${LD_LIBS} \
	-o ${TEST_EXEC}
	
compile_linux_misc_function_test  : misc_function_test.cpp ;
	@echo 
	${CXX} ${CXX_FLAGS} \
	misc_function_test.cpp  \
	${MISC_INCLUDE} \
	-fuse-ld=gold ${LD_FLAGS} \
	${LIBSNARK} \
	${LD_LIBS} \
	-o ${MISC_TEST_EXEC}
 

compile_darwin_test  :  test.cpp ;
	@echo ;
	source ${BUILD_DIR}/../darwin_path.info ; \
	${CXX} ${CXX_FLAGS} \
	test.cpp  \
	${TEST_INCLUDE} -I$${GMP}/include -I$${OpenSSL}/include -I$${OMP}/include -I$${OpenJDK}/include \
	${LD_FLAGS} \
	${LIBSNARK} \
	-L$${OpenSSL}/lib -L$${GMP}/lib -L$${OMP}/lib \
	${LD_LIBS} \
	-o ${TEST_EXEC}
	
compile_darwin_misc_function_test  : misc_function_test.cpp ;
	@echo 
	source ${BUILD_DIR}/../darwin_path.info ; \
	${CXX} ${CXX_FLAGS} \
	misc_function_test.cpp  \
	${MISC_INCLUDE} -I${GMP}/include -I${OpenSSL}/include -I${OMP}/include -I${OpenJDK}/include \
	${LD_FLAGS} \
	${LIBSNARK} \
	-L$${OpenSSL}/lib -L$${GMP}/lib -L$${OMP}/lib \
	${LD_LIBS} \
	-o ${MISC_TEST_EXEC}


run_all :
	@echo
	${TEST_EXEC} RealEstate 32
	@#${TEST_EXEC} DID
	@#${TEST_EXEC} Register  
	@#${TEST_EXEC} Tally   
	@#${TEST_EXEC} Vote 
	@#${TEST_EXEC} ZKlay 8
	@#${TEST_EXEC} ZKlay 32
	@#${TEST_EXEC} zkzkRollup
	@#${MISC_TEST_EXEC}/misc_function_test.debug


clean :
	rm -fr ${BUILD_DIR} 
	rm -f *crs_pk.json 
	rm -f *crs_pk.dat 
	rm -f *crs_vk.json 
	rm -f *crs_vk.dat 
	rm -f *proof.json 
	rm -f *_constraint_system.dat
