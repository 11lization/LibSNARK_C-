

#pragma once

#include <stdio.h>
#include <stdio.h>
#include <string>
#include <map>

#include <Config.hpp>
#include <CircuitGenerator.hpp>
#include <libff/common/profiling.hpp>

typedef unsigned long VarIndex_t;

namespace libsnark {

    typedef std::map<std::string , std::string> CircuitArguments ;
    typedef CircuitGenerator* (*create_circuit_ftn_t)( const CircuitArguments & , const Config & ) ;

    class Context_base {

    protected:
        
        const int id ;
        const string circuit_name ;
        const int proof_system ;
        const Config config ;
        const create_circuit_ftn_t create_circuit_ftn ;
        const string arith_text_path ;
        const string inputs_text_path ;
        const string cs_file_path ;
        
        CircuitArguments circuit_arguments ;
        CircuitGenerator * generator ;

        bool inputs_evaluated ;

        VarIndex_t next_free_gadgetlib2_variable_index ;

        void clear_last_errmsg();

    public:

        static const int last_function_msg_size = 8192 ;
        static const string empty_string_buffer ; 
        char last_function_msg [last_function_msg_size] ;
 
        Context_base(int id, 
                    const string & circuit_name , 
                    int proof_system ,
                    const Config & config , 
                    create_circuit_ftn_t create_circuit_ftn ,
                    const string & arith_text_path , 
                    const string & inputs_text_path ,
                    const string & cs_file_path) ;

        virtual ~Context_base() {}

        int assign_circuit_argument(const char * arg_key , const char * arg_value );

        virtual int build_circuit() = 0 ;
        virtual int run_setup(  ) = 0 ;
        virtual int run_proof(  ) = 0 ;
        virtual int run_verify(  ) = 0 ;

        virtual int write_cs( const char* file_name, 
                      bool use_compression , 
                      const string & checksum_prefix ) = 0 ;

        virtual int verify_cs_chechsum(const char* file_name , const string & checksum_prefix ) = 0 ;
        
        virtual int write_circuit_to_file( const char* file_name) = 0 ;
        virtual int write_inputs_to_file( const char* file_name) = 0 ;

        virtual int write_vk( const char* file_name) = 0 ;
        virtual int read_vk( const char* file_name) = 0 ;
        
        virtual int write_pk( const char* file_name) = 0 ;
        virtual int read_pk( const char* file_name) = 0 ;
        
        virtual int write_proof( const char* file_name) = 0 ;
        virtual int read_proof( const char* file_name) = 0 ;
        
        virtual const char* serialize_pk_object() = 0 ;
        virtual int de_serialize_pk_object(const char* json_string) = 0 ;

        virtual const char* serialize_vk_object() = 0 ;
        virtual int de_serialize_vk_object(const char* json_string) = 0 ;

        virtual const char* serialize_proof_object() = 0 ;
        virtual int de_serialize_proof_object(const char* json_string) = 0 ;
        virtual int get_set_serialize_format(int format ) = 0 ;


        int update_primary_input( const char* input_name , int value );
        int update_primary_input_strValue( const char* input_name , const char * value_str );
        int update_primary_input_array( const char* input_name , int array_index, int value );
        int update_primary_input_array_strValue( const char* input_name , int array_index, const char * value_str );
        int reset_primary_input_array( const char* input_name , int value );
        int reset_primary_input_array_strValue( const char* input_name , const char * value_str );
        int update_primary_input_from_json(const char* json_str ) ;

        const char* get_last_function_msg();
        
        VarIndex_t getNextVariableIndex() ;
        VarIndex_t getLastVariableIndex() ;
        
    };

    void print_profile_logs(string title , libff::profiling & profile );
    
    void degub_print_VmSize() ;

    #define ContextIdErr LOGD("\n ***  Invalid Constext ID [%d] in [%s] *** \n" , context_id , __FUNCTION__ );

}