

#include <logging.hpp>

namespace libsnark {
 
    template <typename FieldT , typename ppT_GG , typename ppT_ROM_SE >
    Context<FieldT,ppT_GG,ppT_ROM_SE>::Context( int __id, 
                                                const string & __circuit_name , 
                                                int __proof_system ,
                                                const Config & __config , 
                                                create_circuit_ftn_t __create_circuit_ftn ,
                                                const string & __arith_text_path , 
                                                const string & __inputs_text_path ,
                                                const string & __cs_file_path )
        : Context_base( __id, 
                        __circuit_name, 
                        __proof_system, 
                        __config ,
                        __create_circuit_ftn, 
                        __arith_text_path , 
                        __inputs_text_path, 
                        __cs_file_path ) 
    {
        
        pb = NULL ;
        generator = NULL;
        wire_variable_map = NULL ;
        wire_variable_map_count = 0 ;
        zero_variables_idx = NULL ;
        zero_variables_count = 0 ;
        arith_file_reader = NULL ;
        embedded_generator_reader  = NULL ;
        keypair_ROM_SE = NULL ;
        proof_ROM_SE = NULL ; 
        inputs_evaluated = false ;
        serialization_format = serializeFormatCRV ;

    } 

    template <typename FieldT , typename ppT_GG , typename ppT_ROM_SE >
    Context<FieldT,ppT_GG,ppT_ROM_SE>::~Context(){
        full_assignment.clear();
        primary_input.clear() ; 
        auxiliary_input.clear() ;
        if ( arith_file_reader ){ try{ delete arith_file_reader ; }catch(exception e){} }
        if ( embedded_generator_reader ){ try{ delete embedded_generator_reader ; }catch(exception e){} }
        if ( generator ){ generator->finalize(); try{ delete generator ; }catch(exception e){} }
        if ( wire_variable_map ) { free (wire_variable_map) ; }
        if ( zero_variables_idx ) { free (zero_variables_idx) ; }
        if (keypair_ROM_SE ){ try{ delete keypair_ROM_SE ; }catch(exception e){} }
        if (proof_ROM_SE){ try{ delete proof_ROM_SE ; }catch(exception e){} }
    }

}