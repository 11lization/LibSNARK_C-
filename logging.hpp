

#pragma once

#include <stdio.h>
#include <string.h> 
#include <chrono> 


#ifdef USING_ANDROID_JNI_WRAPPER 
    #include <jni.h>
    #include <android/log.h>
    #define  LOG_TAG   "SNARK_LOG"
#endif


#ifdef SILENT_BUILD

    static inline int void_snprintf ( char * s, size_t n, const char * format, ... ){ (void)(s); (void)(n) ; (void)(format) ; return 0 ; }
    #define  LOGD(...) void_snprintf( NULL, 0 ,__VA_ARGS__);
    #define  LOGD_HOW_LONG(...) 
    
#else

    #ifdef USING_ANDROID_JNI_WRAPPER 
        static inline void write_to_env_log(char* msg){
            __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG , "%s" , msg ) ;
            return ;
        }
    #else 
        static inline void write_to_env_log(char* msg){
            fprintf(stderr, "%s", msg ); 
            fflush(stderr);
            return ;
        }
    #endif


    #define LOGD_STR_BUF_SIZE 4096

    #define LOGD(...)   {   char LOGD_STR_BUF[LOGD_STR_BUF_SIZE] ; \
                            snprintf(LOGD_STR_BUF, LOGD_STR_BUF_SIZE,__VA_ARGS__); \
                            write_to_env_log(LOGD_STR_BUF); \
                        }
    
    static inline void LOGD_HOW_LONG(const char* ofwhat, const std::chrono::high_resolution_clock::time_point & begin ){
        const auto end = std::chrono::high_resolution_clock::now() ; 
        const auto count = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() ;
        LOGD("%s done in %lld milliseconds.\n" , ofwhat , count );
        return ;
    }

    
#endif


#undef printf 
#define printf LOGD 