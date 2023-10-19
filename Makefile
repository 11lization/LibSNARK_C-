

LIBSNARK_SRC_DIR    :=${PWD}/libsnark/

LOG_FILE            :=build_workspace/${target}/log_${task}_${project}

MAKE_JOBS           := -j 8


# Select Target
OS                  :=$(shell uname -s | tr A-Z a-z)
CPU                 :=$(shell uname -m)
AUTO_SELECT_TARGET  :=${OS}

CXX_ARGUMENTS       = -Wall -Wextra -Wfatal-errors -stdlib=libc++  -std=c++11 -Wno-deprecated

ifeq ($(strip $(release)),no)

BUILD_TYPE          := release=no
BUILD_DIR           :=build_workspace/${target}/${project}_debug
INSTALL_DIR         :=${PWD}/lib/${target}_debug
CXX_ARGUMENTS		+= -g -O2 -DDEBUG -DDEBUG_BUILD

else

BUILD_TYPE          = release=yes
CXX_ARGUMENTS       += -flto -Ofast  -DRELEASE_BUILD
USING_FLTO          := -flto 
BUILD_DIR           :=build_workspace/${target}/${project}_release
INSTALL_DIR         :=${PWD}/lib/${target}_release

ifeq ($(strip $(silent)),yes)
BUILD_TYPE          += silent=yes
CXX_ARGUMENTS       += -DSILENT_BUILD 
else
BUILD_TYPE          += silent=no
endif

endif

	



all : 
	@make ${AUTO_SELECT_TARGET} target=${AUTO_SELECT_TARGET} ${BUILD_TYPE}

help:
	@echo 
	@echo "Help :"
	@echo 
	@echo "\tSpecify Build Target : Examples :"
	@echo "\t  $$ make android"
	@echo "\t  $$ make ios"
	@echo "\t  $$ make darwin           # Apple Silicon or Intel MacOS"
	@echo "\t  $$ make linux"
	@echo ; echo 
	@echo "\tDefault Build Type is \"Debug\" for darwin and linux targets"
	@echo "\t change with : release=yes "
	@echo "\t Eg :"
	@echo "\t  $$ make linux_x86_64 release=yes"
	@echo ; echo 

android : ;
	@make build_target_lib ${BUILD_TYPE} target=android

android_sample_app : ;
	@make run_task_verbose ${BUILD_TYPE} target=android  task=Sample_App project=Snark

ios :  ;
	@make build_target_lib ${BUILD_TYPE} target=ios

ios_sample_app :  ;
	@make run_task_verbose ${BUILD_TYPE} target=ios  task=Sample_App project=Snark

darwin : banner build_workspace/darwin_path.info   ;
	@make build_target_lib ${BUILD_TYPE} target=darwin

linux : banner ;
	@make build_target_lib ${BUILD_TYPE} target=linux

banner :
	@echo ; echo
	@echo "---------------------------------------------------"
	@echo "  Target : ${target} , Build Type : ${BUILD_TYPE}"
	@echo "---------------------------------------------------"
	@echo 

build_workspace/darwin_path.info :
	@mkdir -p build_workspace
	@echo
	@echo " --> Searching for [ OpenSSL , GMP , OMP , OpenJDK ] installation path on MacOS"
	@echo
	python3 build_scripts/make_helper.py --action=darwin_path --export_script_file=$@
	@echo

doc:
	@mkdir -p build_workspace/doxygen
	@mkdir -p docs
	@cd  build_workspace/doxygen ; doxygen ../../build_scripts/Doxygen


clean : 
	rm -rf build_workspace/${AUTO_SELECT_TARGET}
	rm -f lib/${AUTO_SELECT_TARGET}_*/lib/libSnark.* 

clean_android :
	rm -rf build_workspace/android
	rm -f lib/android_*/lib/libSnark.* 

clean_ios :
	rm -rf build_workspace/ios
	rm -f lib/ios_*/lib/libSnark*.a 


${BUILD_DIR} :
	@mkdir -p ${BUILD_DIR}


${INSTALL_DIR} :
	@mkdir -p ${INSTALL_DIR} 


build_target_lib : 
	@make run_task_verbose target=${target}  ${BUILD_TYPE} task=configure project=Snark
	@make run_task_verbose target=${target}  ${BUILD_TYPE} task=make      project=Snark


run_task : ${BUILD_DIR} ${INSTALL_DIR} ;
	@echo "\n"
	@echo "  Task     = ${task}_${project}"
	@echo "  Target   = ${target} "
	@echo "  Log file = ${LOG_FILE}"
	@echo "" >${LOG_FILE}
	@echo 
	@make target=${target} ${task}_${project} ${BUILD_TYPE}  >>${LOG_FILE} 2>>${LOG_FILE}


run_task_verbose : ${BUILD_DIR} ${INSTALL_DIR} ;
	@echo "\n"
	@echo "  Task     = ${task}_${project}"
	@echo "  Target   = ${target} " 
	@echo 
	@make target=${target} ${task}_${project} ${BUILD_TYPE}


ifneq ($(strip $(target)),)
include build_scripts/${target}.mk  
endif
