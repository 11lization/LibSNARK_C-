

 
#include <api.hpp>

#ifndef UNUSEDPARAM
#define UNUSEDPARAM(x) (void)(x);
#endif


#ifdef USING_JNI_WRAPPER 

    #include <jni.h>
    
    #ifdef __cplusplus
    extern "C" {
    #endif

    #define CAT2(a,b,c) a ## _ ## b ## _ ## c
    #define CAT1(a,b,c) CAT2(a,b,c)
    #define JNIFunction(ftn) JNIEXPORT jint JNICALL CAT1(JavaPackageName,JavaClassName,ftn)
    #define JNIFunctionString(ftn) JNIEXPORT jstring JNICALL CAT1(JavaPackageName,JavaClassName,ftn)
    
    
    JNIFunction(createCircuitContext)(
            JNIEnv* env, jobject jobj,
            jstring circuit_name ,
            jint proof_system ,
            jint ec_selection ,
            jstring arith_text_path,
            jstring inputs_text_path,
            jstring cs_file_path )
    {   
        UNUSEDPARAM(jobj) 
        
        const char *circuit_name_char = (env)->GetStringUTFChars(circuit_name, NULL);

        const char *arith_text_path_char  = (env)->GetStringUTFChars(arith_text_path, NULL);
        
        const char *inputs_text_path_char = (env)->GetStringUTFChars(inputs_text_path, NULL);

        const char *cs_path_char  = (env)->GetStringUTFChars(cs_file_path, NULL);
        
        int context_id = createCircuitContext(circuit_name_char, 
                                              proof_system , 
                                              ec_selection , 
                                              arith_text_path_char , 
                                              inputs_text_path_char , 
                                              cs_path_char );

        return (jint)context_id ;
    }
    

    JNIFunction(assignCircuitArgument)(
            JNIEnv* env, jobject jobj,
            jint context_id ,
            jstring arg_key ,
            jstring arg_value )
    {   
        UNUSEDPARAM(jobj) 
        const char* key = (env)->GetStringUTFChars(arg_key, NULL);
        const char* value = (env)->GetStringUTFChars(arg_value, NULL);
        
        int rtn = assignCircuitArgument(context_id, key , value );

        return (jint)rtn ;
    }


    JNIFunction(buildCircuit)(
            JNIEnv* env, jobject jobj,
            jint context_id )
    {   
        UNUSEDPARAM(jobj) 
        UNUSEDPARAM(env)
        
        int rtn = buildCircuit( context_id);

        return (jint)rtn ;
    }


    JNIFunction(updatePrimaryInput)(
            JNIEnv* env, jobject jobj,
            jint context_id ,
            jstring input_name,
            jint value )
    {
        UNUSEDPARAM(jobj) 
        const char *input_name_char = (env)->GetStringUTFChars(input_name, NULL);
        int rtn = updatePrimaryInput (context_id ,input_name_char , value ) ;
        return (jint)rtn ;
    }


    JNIFunction(updatePrimaryInputStr)(
            JNIEnv* env, jobject jobj,
            jint context_id ,
            jstring input_name,
            jstring value_str )
    {
        UNUSEDPARAM(jobj) 
        const char *input_name_char = (env)->GetStringUTFChars(input_name, NULL);
        const char *value_str_char = (env)->GetStringUTFChars(value_str, NULL);
        int rtn = updatePrimaryInputStr (context_id ,input_name_char , value_str_char ) ;
        return (jint)rtn ;
    }


    JNIFunction(updatePrimaryInputArray)(
            JNIEnv* env, jobject jobj,
            jint context_id ,
            jstring input_name,
            jint array_index ,
            jint value )
    {
        UNUSEDPARAM(jobj) 
        const char *input_name_char = (env)->GetStringUTFChars(input_name, NULL);
        int rtn = updatePrimaryInputArray (context_id ,input_name_char , array_index , value ) ;
        return (jint)rtn ;
    }

    
    JNIFunction(updatePrimaryInputArrayStr)(
            JNIEnv* env, jobject jobj,
            jint context_id ,
            jstring input_name,
            jint array_index ,
            jstring value_str )
    {
        UNUSEDPARAM(jobj) 
        const char *input_name_char = (env)->GetStringUTFChars(input_name, NULL);
        const char *value_str_char = (env)->GetStringUTFChars(value_str, NULL);
        int rtn = updatePrimaryInputArrayStr (context_id ,input_name_char , array_index , value_str_char ) ;
        return (jint)rtn ;
    }

    JNIFunction(resetPrimaryInputArray)(
            JNIEnv* env, jobject jobj,
            jint context_id ,
            jstring input_name,
            jint value )
    {
        UNUSEDPARAM(jobj) 
        const char *input_name_char = (env)->GetStringUTFChars(input_name, NULL);
        int rtn = resetPrimaryInputArray (context_id ,input_name_char , value ) ;
        return (jint)rtn ;
    }

    
    JNIFunction(resetPrimaryInputArrayStr)(
            JNIEnv* env, jobject jobj,
            jint context_id ,
            jstring input_name,
            jstring value_str )
    {
        UNUSEDPARAM(jobj) 
        const char *input_name_char = (env)->GetStringUTFChars(input_name, NULL);
        const char *value_str_char = (env)->GetStringUTFChars(value_str, NULL);
        int rtn = resetPrimaryInputArrayStr (context_id ,input_name_char , value_str_char ) ;
        return (jint)rtn ;
    }

    JNIFunction(updatePrimaryInputFromJson)(
            JNIEnv* env, jobject jobj,
            jint context_id ,
            jstring json_str)
    {
        UNUSEDPARAM(jobj) 
        const char *input_name_char = (env)->GetStringUTFChars(json_str, NULL);
        int rtn = updatePrimaryInputFromJson (context_id , input_name_char ) ;
        return (jint)rtn ;
    }
    
    JNIFunction(writeConstraintSystem)(
            JNIEnv* env, jobject jobj,
            jint context_id ,
            jstring document_dir,
            jint use_compression,
            jstring checksum_prefix)
    {
        UNUSEDPARAM(jobj) 
        const char *document_dir_char = (env)->GetStringUTFChars(document_dir, NULL);
        const char *checksum_prefix_char = (env)->GetStringUTFChars(checksum_prefix, NULL);
        int rtn = writeConstraintSystem (context_id ,document_dir_char, use_compression,  checksum_prefix_char ) ;
        return (jint)rtn ;
    }

    JNIFunction(verifyConstraintSystemFileChecksum)(
            JNIEnv* env, jobject jobj,
            jint context_id ,
            jstring document_dir ,
            jstring checksum_prefix)
    {
        UNUSEDPARAM(jobj) 
        const char *document_dir_char = (env)->GetStringUTFChars(document_dir, NULL);
        const char *checksum_prefix_char = (env)->GetStringUTFChars(checksum_prefix, NULL);
        int rtn = verifyConstraintSystemFileChecksum (context_id ,document_dir_char, checksum_prefix_char ) ;
        return (jint)rtn ;
    }


    JNIFunction(writeCircuitToFile)(
            JNIEnv* env, jobject jobj,
            jint context_id ,
            jstring document_dir)
    {
        UNUSEDPARAM(jobj) 
        const char *document_dir_char = (env)->GetStringUTFChars(document_dir, NULL);
        int rtn = writeCircuitToFile (context_id ,document_dir_char) ;
        return (jint)rtn ;
    }


    JNIFunction(writeInputsToFile)(
            JNIEnv* env, jobject jobj,
            jint context_id ,
            jstring document_dir)
    {
        UNUSEDPARAM(jobj) 
        const char *document_dir_char = (env)->GetStringUTFChars(document_dir, NULL);
        int rtn = writeInputsToFile (context_id ,document_dir_char) ;
        return (jint)rtn ;
    }


    JNIFunction(writeVK)(
            JNIEnv* env, jobject jobj,
            jint context_id ,
            jstring document_dir)
    {
        UNUSEDPARAM(jobj) 
        const char *document_dir_char = (env)->GetStringUTFChars(document_dir, NULL);
        int rtn = writeVK (context_id ,document_dir_char) ;
        return (jint)rtn ;
    }


    JNIFunction(readVK)(
            JNIEnv* env, jobject jobj,
            jint context_id ,
            jstring document_dir)
    {
        UNUSEDPARAM(jobj) 
        const char *document_dir_char = (env)->GetStringUTFChars(document_dir, NULL);
        int rtn = readVK (context_id ,document_dir_char) ;
        return (jint)rtn ;
    }


    JNIFunction(writePK)(
            JNIEnv* env, jobject jobj,
            jint context_id ,
            jstring document_dir)
    {
        UNUSEDPARAM(jobj) 
        const char *document_dir_char = (env)->GetStringUTFChars(document_dir, NULL);
        int rtn = writePK (context_id ,document_dir_char) ;
        return (jint)rtn ;
    }


    JNIFunction(readPK)(
            JNIEnv* env, jobject jobj,
            jint context_id ,
            jstring document_dir)
    {
        UNUSEDPARAM(jobj) 
        const char *document_dir_char = (env)->GetStringUTFChars(document_dir, NULL);
        int rtn = readPK (context_id ,document_dir_char) ;
        return (jint)rtn ;
    }


    JNIFunction(writeProof)(
            JNIEnv* env, jobject jobj,
            jint context_id ,
            jstring document_dir)
    {
        UNUSEDPARAM(jobj) 
        const char *document_dir_char = (env)->GetStringUTFChars(document_dir, NULL);
        int rtn = writeProof (context_id ,document_dir_char) ;
        return (jint)rtn ;
    }


    JNIFunction(readProof)(
            JNIEnv* env, jobject jobj,
            jint context_id ,
            jstring document_dir)
    {
        UNUSEDPARAM(jobj) 
        const char *document_dir_char = (env)->GetStringUTFChars(document_dir, NULL);
        int rtn = readProof (context_id ,document_dir_char) ;
        return (jint)rtn ;
    }



    JNIFunctionString(serializeProofKey)(
            JNIEnv* env, jobject jobj,
            jint context_id)
    {
        UNUSEDPARAM(jobj) 
        const char* ret_val = serializeProofKey(context_id);
        return env->NewStringUTF( ret_val );
    }

    JNIFunction(deSerializeProofKey)(
            JNIEnv* env, jobject jobj,
            jint context_id ,
            jstring json_string)
    {
        UNUSEDPARAM(jobj) 
        const char *json_string_char = (env)->GetStringUTFChars(json_string, NULL);
        int rtn = deSerializeProofKey (context_id ,json_string_char) ;
        return (jint)rtn ;
    }


    JNIFunctionString(serializeVerifyKey)(
            JNIEnv* env, jobject jobj,
            jint context_id)
    {
        UNUSEDPARAM(jobj) 
        const char* ret_val = serializeVerifyKey(context_id);
        return env->NewStringUTF( ret_val );
    }

    JNIFunction(deSerializeVerifyKey)(
            JNIEnv* env, jobject jobj,
            jint context_id ,
            jstring json_string)
    {
        UNUSEDPARAM(jobj) 
        const char *json_string_char = (env)->GetStringUTFChars(json_string, NULL);
        int rtn = deSerializeVerifyKey (context_id ,json_string_char) ;
        return (jint)rtn ;
    }


    JNIFunctionString(serializeProof)(
            JNIEnv* env, jobject jobj,
            jint context_id)
    {
        UNUSEDPARAM(jobj) 
        const char* ret_val = serializeProof(context_id);
        return env->NewStringUTF( ret_val );
    }

    JNIFunction(deSerializeProof)(
            JNIEnv* env, jobject jobj,
            jint context_id ,
            jstring hex_string)
    {
        UNUSEDPARAM(jobj) 
        const char *hex_string_char = (env)->GetStringUTFChars(hex_string, NULL);
        int rtn = deSerializeProof (context_id ,hex_string_char) ;
        return (jint)rtn ;
    }

    JNIFunction(serializeFormat)(
            JNIEnv* env, jobject jobj,
            jint context_id ,
            jint format )
    {
        UNUSEDPARAM(env)
        UNUSEDPARAM(jobj) 
        int rtn = serializeFormat (context_id , format ) ;
        return (jint)rtn ;
    }

    
    JNIFunction(runSetup)(
            JNIEnv* env, jobject jobj,
            jint context_id )
    {
        UNUSEDPARAM(env)
        UNUSEDPARAM(jobj) 
        int rtn = runSetup (context_id ) ;
        return (jint)rtn ;
    }


    JNIFunction(runProof)(
            JNIEnv* env, jobject jobj,
            jint context_id )
    {
        UNUSEDPARAM(env)
        UNUSEDPARAM(jobj) 
        int rtn = runProof (context_id ) ;
        return (jint)rtn ;
    }



    JNIFunction(runVerify)(
            JNIEnv* env, jobject jobj,
            jint context_id )
    {
        UNUSEDPARAM(env)
        UNUSEDPARAM(jobj) 
        int rtn = runVerify (context_id ) ;
        return (jint)rtn ;
    }


    JNIFunction(finalizeCircuit)(
            JNIEnv* env, jobject jobj,
            jint context_id )
    {
        UNUSEDPARAM(env);
        UNUSEDPARAM(jobj) 
        int rtn = finalizeCircuit (context_id) ;
        return (jint)rtn ;
    }

    JNIFunction(finalizeAllCircuit)(
            JNIEnv* env, jobject jobj)
    {
        UNUSEDPARAM(env);
        UNUSEDPARAM(jobj) 
        int rtn = finalizeAllCircuit () ;
        return (jint)rtn ;
    }

    


    JNIFunctionString(getLastFunctionMsg)(
            JNIEnv* env, jobject jobj,
            jint context_id )
    {
        UNUSEDPARAM(env) UNUSEDPARAM(jobj) 
        return env->NewStringUTF( getLastFunctionMsg(context_id) );
    }


    #ifdef __cplusplus
    }
    #endif

#endif




