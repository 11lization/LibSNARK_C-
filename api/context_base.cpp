

#include <SubsetSumHashGadget.hpp>
#include <MerkleTreePathGadget.hpp>
#include <MiMC7Gadget.hpp>
#include <MerkleTreePathGadget.hpp>
#include <ECGroupOperationGadget.hpp>
#include <ECGroupGeneratorGadget.hpp>
#include <HashGadget.hpp>
#include <SHA256Gadget.hpp>
#include <MiMC7Gadget.hpp>
#include <PoseidonGadget.hpp>


#include <libff/algebra/curves/alt_bn128/alt_bn128_pp.hpp>
#include <libff/algebra/curves/bls12_381/bls12_381_pp.hpp>


#include "context_base.hpp" 
#include <logging.hpp>

using namespace CircuitBuilder::Gadgets ;

namespace libsnark {

    const string Context_base::empty_string_buffer = "" ;

    Context_base::Context_base( int __id, 
                                const string & __circuit_name , 
                                int __proof_system ,
                                const Config & __config , 
                                create_circuit_ftn_t __create_circuit_ftn ,
                                const string & __arith_text_path , 
                                const string & __inputs_text_path ,
                                const string & __cs_file_path ) 
        : id ( __id ) ,
          circuit_name( __circuit_name ) ,
          proof_system( __proof_system ) ,
          config(__config) , 
          create_circuit_ftn ( __create_circuit_ftn ),
          arith_text_path ( __arith_text_path ) ,
          inputs_text_path ( __inputs_text_path ) ,
          cs_file_path ( __cs_file_path ),
          next_free_gadgetlib2_variable_index ( 0 ) 
    {
        if ( create_circuit_ftn && ! cs_file_path.size() ){
            snprintf (last_function_msg , last_function_msg_size , "success : using embedded [%s] circuit generator" , circuit_name.c_str() ); 
        }else if ( create_circuit_ftn && cs_file_path.size() ){
            snprintf (last_function_msg , last_function_msg_size , "success : using embedded [%s] circuit generator with cs file [%s]" , circuit_name.c_str() , cs_file_path.c_str() ); 
        }else{
            snprintf (last_function_msg , last_function_msg_size , "success : using arbitrary arith/inputs [%s] circuit file " , circuit_name.c_str() ); 
        }
    }
    

    int Context_base::assign_circuit_argument(const char * arg_key , const char * arg_value ){
        circuit_arguments[string(arg_key)] = string(arg_value) ;
        return 0;
    }


    VarIndex_t Context_base::getNextVariableIndex() {
        return next_free_gadgetlib2_variable_index ++ ;
    }


    VarIndex_t Context_base::getLastVariableIndex() {
        return next_free_gadgetlib2_variable_index  ;
    }


    void Context_base::clear_last_errmsg(){
        last_function_msg[0] = '\0' ;
    }

    const char* Context_base::get_last_function_msg(){
        return last_function_msg ;
    }



    void print_profile_logs( string title , libff::profiling & profile ){
    #ifndef SILENT_BUILD
        LOGD("\n\n%s Profiling Info : \n\n" , title.c_str());
        
        std::string line ;
        while(getline(profile.get_profiling_logs(),line)){
            LOGD("%s\n" , line.c_str() );
        }
        
    #else
        libff::UNUSED(title, profile) ;
    #endif
    }
 
}