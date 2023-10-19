
#pragma once

#include <stdio.h>
#include <string>
#include <map>

#include <Config.hpp>

#include <libsnark/jsnark_interface/CircuitReader.hpp>
#include <libsnark/jsnark_interface/CircuitReader_2.hpp>
#include <libsnark/zk_proof_systems/ppzksnark/r1cs_gg_ppzksnark/r1cs_gg_ppzksnark.hpp>
#include <libsnark/zk_proof_systems/ppzksnark/r1cs_rom_se_ppzksnark/r1cs_rom_se_ppzksnark.hpp>
#include <libsnark/common/default_types/r1cs_gg_ppzksnark_pp.hpp>

#include <api.hpp>
#include "context.hpp"

using namespace std;
using namespace libsnark;

namespace libsnark {

    typedef libsnark::default_r1cs_gg_ppzksnark_pp ppT_GG ;
    typedef libff::default_ec_pp ppT_ROM_SE ;


    enum enum_evaluate_with {
        Generator           = 1 ,
        Reader              = 2 ,
        NoPostEvaluation    = 3
    };

    class Context {
    
    private :
        
        int id ;
        string circuit_name ;
        string using_embedded_circuit_name ;

        int proof_system ;

        enum_evaluate_with evaluate_with ;
        
        CircuitArguments circuit_arguments ;
        CircuitGenerator * generator ;

        r1cs_constraint_system<FieldT> cs ;

        ProtoboardPtr pb ;
        CircuitReader * reader ;
        CircuitReader_2 * reader_2 ;

        wire2VariableMap_t* wire_variable_map ;
        uint32_t wire_variable_map_count ;
        uint32_t* zero_variables_idx ;
        uint32_t zero_variables_count ;

        string arith_text_path ;
        string inputs_text_path ;
        
        bool inputs_evaluated ;

        uint32_t full_assignment_size ;
        r1cs_variable_assignment<FieldT> full_assignment ;
        r1cs_primary_input<FieldT> primary_input ;
        r1cs_auxiliary_input<FieldT> auxiliary_input ;

        r1cs_keypair * keypair_ROM_SE ;
        r1cs_rom_se_ppzksnark_proof<ppT_ROM_SE> * proof_ROM_SE ;
        
        r1cs_gg_ppzksnark_keypair<ppT_GG> keypair_GG ;
        r1cs_gg_ppzksnark_proof<ppT_GG> proof_GG ;
        
        static const int last_function_msg_size = 8192 ;
        char last_function_msg [last_function_msg_size] ;

        int evaluate_inputs( libff::profiling & profile , bool IgnoreFailedAssertion = false );
        
        void clear_last_errmsg();

        int serialization_format ;
        std::string serialization_buffer ;
        string proof_json_str ;
        string vk_json_str ;

        VarIndex_t nextFreeIndex ;

        friend size_t getNextFreeVariableIdx( GlobalContext * gcxt) ;
        friend size_t getVariableIdx( GlobalContext * gcxt) ;

        int read_cs( const char* file_name , libff::profiling & profile );

    
    public:
        
        Context(int id, const char * circuit_name , int proof_system );
        ~Context();
        
        int build_circuit();
        int build_circuit(const char * cs_file_name);
        int build_circuit(const char * arith_text_path, const char * inputs_text_path);

        int update_primary_input( const char* input_name , int value );
        int update_primary_input_strValue( const char* input_name , const char * value_str );
        int update_primary_input_array( const char* input_name , int array_index, int value );
        int update_primary_input_array_strValue( const char* input_name , int array_index, const char * value_str );
        int reset_primary_input_array( const char* input_name , int value );
        int reset_primary_input_array_strValue( const char* input_name , const char * value_str );
        int update_primary_input_from_json(const char* json_str ) ;
        
        int run_setup(  );
        int run_proof(  );
        int run_verify(  );

        int write_cs( const char* file_name, 
                      bool use_compression , 
                      const string & checksum_prefix );

        int verify_cs_chechsum(const char* file_name , const string & checksum_prefix );
        
        int write_circuit_to_file( const char* file_name);
        int write_inputs_to_file( const char* file_name);

        int write_vk( const char* file_name);
        int read_vk( const char* file_name);
        
        int write_pk( const char* file_name);
        int read_pk( const char* file_name);
        
        int write_proof( const char* file_name);
        int read_proof( const char* file_name);
        
        const char* serialize_pk_object();
        int de_serialize_pk_object(const char* json_string);

        const char* serialize_vk_object();
        int de_serialize_vk_object(const char* json_string);

        const char* serialize_proof_object();
        int de_serialize_proof_object(const char* json_string);
        int get_set_serialize_format(int format );

        const char* get_last_function_msg();
        
        int assign_circuit_argument(const char * arg_key , const char * arg_value );

        
    private :
        static void init_globals();
        typedef CircuitGenerator* (*create_circuit_ftn_t)( const CircuitArguments & , const Config & ) ;
        static std::map<std::string , create_circuit_ftn_t> EmbeddedCircuitList ;
        static Config config ;
    
    };

    
    void print_profile_logs(string title , libff::profiling & profile );
    
    void degub_print_VmSize() ;

    #define ContextIdErr LOGD("\n ***  Invalid Constext ID [%d] in [%s] *** \n" , context_id , __FUNCTION__ );

}







