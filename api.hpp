
#pragma once

#include <stdio.h>
#include <string.h>
#include <gmp.h>

#ifdef __cplusplus
extern "C" {
#endif

    /** \anchor all_a */
    
    #define R1CS_GG         1 
    #define R1CS_ROM_SE     2 
    
    #define EC_ALT_BN128    1 
    #define EC_BLS12_381    2 

    #define serializeFormatDefault  1
    #define serializeFormatCRV      2 
    #define serializeFormatZKlay    3 
    
    /** \mainpage LibSnark API 
     * \ref grp1_a "Circuit Initialization and Construction Functions." \n
     * \ref grp2_a "Core Snark Functions." \n
     * \ref grp3_a "Primary Inputs Update Functions." \n
     * \ref grp4_a "Serialize Data Structures (proof key, verify key, and proof data)." \n
     * \ref all_a "All API Functions." \n
     */

    /** @defgroup grp1 Circuit Initialization and Construction Functions
     * \anchor grp1_a
     * @{
     */
    
    /**
     * Initialize a circuit context.
     * 
     * The API allows users to create multiple circuits concurrently. \n
     * Use this function to create a separate circuit instance.
     * 
     * @param circuit_name - the name of the circuit. Also selects which of the embedded circuit constructor to use.
     *
     * @param proof_system - select the proof system : {@link #R1CS_GG} or {@link #R1CS_ROM_SE}.
     * 
     * @param ec_selection - select elliptic curve : {@link #EC_ALT_BN128} or {@link #EC_BLS12_381}.
     * 
     * @param arith_text_path - circuit text file
     * 
     * @param inputs_text_path - primary/auxiliary inputs text file
     * 
     * @param cs_file_path - constraint system file ( generated with the {@link #writeConstraintSystem} function )
     * 
     * @return -1   : invalid \b circuit_name \n 
     *         -2   : invalid \b proof_system \n
     *         -3   : invalid \b ec_selection \n
     *         -4   : unknown error
     *         >=1  : success \n
     * 
     */
    int createCircuitContext(const char * circuit_name , 
                             int proof_system ,
                             int ec_selection ,
                             const char * arith_text_path , 
                             const char * inputs_text_path , 
                             const char * cs_file_path );
    
    /**
     * Construct the embedded circuit
     * 
     * \b circuit_name parameter in {@link #createCircuitContext} selects the embedded circuit constructed by this function.
     *
     * @param context_id - circuit instance identifier. returned by {@link #createCircuitContext}
     * 
     * @return  0 : success\n
     *         -1 : invalid \b context_id \n 
     *          1 : error occurred, get the error description with {@link #getLastFunctionMsg}
     */
    int buildCircuit(int context_id );



    /**
     * Set embeddeds circuit argument (key/value) 
     * 
     * @param context_id - circuit instance identifier. returned by {@link #createCircuitContext}
     * 
     * @param arg_key - specifies circuit arguments key
     * 
     * @param arg_value - specifies circuit arguments value
     * 
     * @return  0 : \b success \n
     *         -1 : invalid \b context_id \n 
     *          1 : error occurred, get the error description with {@link #getLastFunctionMsg}
     */
     int assignCircuitArgument(
            int context_id , 
            const char * arg_key , 
            const char * arg_value  );
    /** @} */

    


    /** @defgroup grp2 Core Snark Functions
     * \anchor grp2_a
     * @param context_id - circuit instance identifier. returned by {@link #createCircuitContext}
     *
     * @return 0 : \b success \n
     *        -1 : invalid \b context_id \n
     *         1 : error occurred , get the error description with {@link #getLastFunctionMsg}
     * @{
     */
    int runSetup( int context_id );
    int runProof( int context_id );
    int runVerify( int context_id );
    /** @} */



    /** @defgroup grp3 Primary Inputs Update Functions
     * \anchor grp3_a
     * The following functions work only with the embedded circuit. \n
     * Circuits constructed with arith/inputs files are not supported. \n
     * They update a key/value (Primary input name/BigInteger) map maintained by the circuit constructor.
     * 
     * 
     * @param context_id - circuit instance identifier. returned by {@link #createCircuitContext}
     * 
     * @param input_name - primary input name (or description) as key 
     * 
     * @param value - new value
     * 
     * @param value_str - new value : a BigIngeter value in hexadecimal string
     * 
     * @param array_index - index of a primary input array to update
     * 
     * @param input_json_string - a JSON object, with keys as primary input names and values as BigInteger in a hexadecimal format.
     * 
     * @return  0 : \b success \n 
     *         -1 : invalid \b context_id \n 
     *          1 : invalid \b input_name \n 
     *          2 : \b array_index out-of-range
     *          3 : error occurred , get the error description with {@link #getLastFunctionMsg}
     * @{
     */
    int updatePrimaryInput(int context_id , const char* input_name , int value );
    int updatePrimaryInputStr(int context_id , const char* input_name , const char * value_str );
    int updatePrimaryInputArray(int context_id , const char* input_name , int array_index, int value );
    int updatePrimaryInputArrayStr(int context_id , const char* input_name , int array_index, const char * value_str );
    int resetPrimaryInputArray(int context_id , const char* input_name , int value );
    int resetPrimaryInputArrayStr(int context_id , const char* input_name , const char * value_str );
    int updatePrimaryInputFromJson(int context_id , const char* input_json_string );
    /** @} */



    


    /** @defgroup grp4 Serialize Data Structures (proof key, verify key, and proof data) to/from file or json string
     * \anchor grp4_a
     * @param context_id - circuit instance identifier. returned by {@link #createCircuitContext}
     * 
     * @param file_name - the file name to read or write to . 
     * 
     * @param json_string - a JSON object used to de-serialize (re-construct) the respective object. Probably generated by their serialize function.
     *
     * @return 0 : \b success \n
     *        -1 : invalid \b context_id \n
     *         1 or pointer to an empty string : error occurred , get the error description with {@link #getLastFunctionMsg}
     * @{
     */
    int writeConstraintSystem(int context_id , const char* file_name , int use_compression , const char* checksum_prefix );
    int verifyConstraintSystemFileChecksum(int context_id , const char* file_name , const char* checksum_prefix ) ;
    
    int writeCircuitToFile(int context_id , const char* file_name);
    int writeInputsToFile(int context_id , const char* file_name);

    int writeVK(int context_id , const char* file_name);
    int readVK(int context_id , const char* file_name);
    
    int writePK(int context_id , const char* file_name);
    int readPK(int context_id , const char* file_name);
    
    int writeProof(int context_id , const char* file_name);
    int readProof(int context_id , const char* file_name);
    
    
    const char* serializeVerifyKey(int context_id );
    int deSerializeVerifyKey(int context_id , const char* json_string);

    const char* serializeProofKey(int context_id );
    int deSerializeProofKey(int context_id , const char* json_string);

    const char* serializeProof(int context_id );
    int deSerializeProof(int context_id , const char* json_string);


    
    int serializeFormat( int context_id , int format ) ;
    /** @} */


    /** 
     * Delete constructed circuit and deallocate used memory.\n 
     * Using the same \b context_id in subsequent function calls will fail.
     * 
     * @param context_id - circuit instance identifier. returned by {@link #createCircuitContext}
     * 
     * @return 0 : \b success \n
     *        -1 : invalid \b context_id
     */
    int finalizeCircuit(int context_id );
    int finalizeAllCircuit();

    

    /** 
     * Get the last error description
     * 
     * @param context_id - circuit instance identifier. returned by {@link #createCircuitContext}
     * 
     * @return 0 : \b success \n
     *        -1 : invalid \b context_id 
     */
    const char * getLastFunctionMsg(int context_id);
    
    





    /**
     * @brief Miscellaneous Application Support Functions
     * 
     */

    #ifdef CIRCUIT_BUILDER_ZKLAY
    extern void ECGroupExp(const char* baseX, const char* exp, char* ret);
    #endif

    
    //
    // MiMC7 Hash
    //
    void mimc7_hash( mpz_ptr dst , mpz_srcptr input , mpz_srcptr fieldPrime );


    //
    //  Curve25519
    //
    void Curve25519_init ( mpz_ptr dst_x , mpz_ptr dst_y , mpz_ptr self_x , int isMatch_ressol  );
    void Curve25519_add ( mpz_ptr dst_x , mpz_ptr dst_y , mpz_ptr self_x , mpz_ptr self_y , mpz_ptr other_x , mpz_ptr other_y);
    void Curve25519_sub ( mpz_ptr dst_x , mpz_ptr dst_y , mpz_ptr self_x , mpz_ptr self_y , mpz_ptr other_x , mpz_ptr other_y );
    void Curve25519_mul ( mpz_ptr dst_x , mpz_ptr dst_y , mpz_ptr self_x , mpz_ptr self_y , mpz_srcptr exp_mpz ) ;
    int Curve25519_isMatchRessol ( mpz_srcptr self_x , mpz_srcptr self_y ) ;


#ifdef __cplusplus
}
#endif





//
//  Change Java pacakge and class name as needed
//
#define JavaPackageName Java_com_zKrypto
#define JavaClassName   libSnarkJNI