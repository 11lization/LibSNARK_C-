
#pragma once

#include <stdio.h>
#include <string>
#include <map>

#include "context_base.hpp"

#include <libsnark/jsnark_interface/ArithFileCircuitReader.hpp>
#include <libsnark/jsnark_interface/EmbeddedGeneratorCircuitReader.hpp>

#include <libsnark/zk_proof_systems/ppzksnark/r1cs_gg_ppzksnark/r1cs_gg_ppzksnark.hpp>
#include <libsnark/zk_proof_systems/ppzksnark/r1cs_rom_se_ppzksnark/r1cs_rom_se_ppzksnark.hpp>

using namespace std;
using namespace libsnark;


namespace libsnark {

    enum enum_evaluate_with {
        Generator           = 1 ,
        Reader              = 2 ,
        NoPostEvaluation    = 3
    };


    template <typename FieldT , typename ppT_GG , typename ppT_ROM_SE >
    class Context : public Context_base {
    
    private :
    
        enum_evaluate_with evaluate_with ;

        int serialization_format ;
        std::string serialization_buffer ;
        string proof_json_str ;
        string vk_json_str ;

        ProtoboardPtr<FieldT> pb ;
        r1cs_constraint_system<FieldT> cs ;
        ArithFileCircuitReader<FieldT> * arith_file_reader ;
        EmbeddedGeneratorCircuitReader<FieldT> * embedded_generator_reader ;

        wire2VariableMap_t* wire_variable_map ;
        uint32_t wire_variable_map_count ;
        uint32_t* zero_variables_idx ;
        uint32_t zero_variables_count ;

        uint32_t full_assignment_size ;
        r1cs_variable_assignment<FieldT> full_assignment ;
        r1cs_primary_input<FieldT> primary_input ;
        r1cs_auxiliary_input<FieldT> auxiliary_input ;

        r1cs_keypair * keypair_ROM_SE ;
        r1cs_rom_se_ppzksnark_proof<ppT_ROM_SE> * proof_ROM_SE ;
        
        r1cs_gg_ppzksnark_keypair<ppT_GG> keypair_GG ;
        r1cs_gg_ppzksnark_proof<ppT_GG> proof_GG ;
        
        int build_circuit_with_generator();
        int build_circuit_with_arith();
        int build_circuit_with_cs();

        int evaluate_inputs( libff::profiling & profile , bool IgnoreFailedAssertion = false );
    
        int read_cs( libff::profiling & profile );

    public:
        
        Context(int id, 
                const string & circuit_name , 
                int proof_system ,
                const Config & config , 
                create_circuit_ftn_t create_circuit_ftn ,
                const string & arith_text_path , 
                const string & inputs_text_path ,
                const string & cs_file_path) ;
        
        ~Context();
        
        int build_circuit();
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
        
    };

}

#include <gadgetlib2/adapters.tcc>
#include <gadgetlib2/variable.tcc>
#include <gadgetlib2/constraint.tcc>
#include <gadgetlib2/protoboard.tcc>
#include <gadgetlib2/integration.tcc>

#include <jsnark_interface/ArithFileCircuitReader.tcc>
#include <jsnark_interface/EmbeddedGeneratorCircuitReader.tcc>

#include "context.tcc"
#include "build_circuit.tcc"
#include "eval_inputs.tcc"

#include <libsnark/zk_proof_systems/ppzksnark/r1cs_gg_ppzksnark/run_r1cs_gg_ppzksnark.hpp>
#include <libsnark/zk_proof_systems/ppzksnark/r1cs_rom_se_ppzksnark/run_r1cs_rom_se_ppzksnark.hpp>
#include "setup.tcc"
#include "proof.tcc"
#include "verify.tcc"

#include "read_write_cs.tcc"
#include "read_write.tcc"
#include "serialize.tcc"
