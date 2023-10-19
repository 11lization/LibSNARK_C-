
#include <logging.hpp>

namespace libsnark {
     
    template <typename FieldT , typename ppT_GG , typename ppT_ROM_SE > 
    int Context<FieldT,ppT_GG,ppT_ROM_SE>::evaluate_inputs ( libff::profiling & profile , bool IgnoreFailedAssertion ) {
        
        if (inputs_evaluated){ return 0 ; }

        LOGD("Evaluating Inputs\n") 

        profile.enter_block("Generate Auxiliary Inputs" );
        
        generator->ignore_failed_assertion = IgnoreFailedAssertion ;
        
        profile.enter_block("generator->evalCircuit()" );
        generator->evalCircuit();
        profile.leave_block("generator->evalCircuit()" );
        
        full_assignment.clear();
        primary_input.clear() ; 
        auxiliary_input.clear() ;


        if ( evaluate_with == Generator ){

            profile.enter_block("convert assignments" );
            
            full_assignment.resize( full_assignment_size , FieldT::zero() );
            
            for( size_t ix = 0 ; ix < wire_variable_map_count ; ix ++ ){
                const wire2VariableMap_t w_v_map = wire_variable_map[ix] ;
                full_assignment[ w_v_map.variable_idx ] = convert2FieldT<FieldT>( generator->getCircuitEvaluator()->getAssignment( w_v_map.wire_idx ) ) ;
            }

            profile.leave_block("convert assignments" );

            primary_input = r1cs_primary_input<FieldT> ( full_assignment.begin(), full_assignment.begin() + cs.num_inputs() );
            auxiliary_input = r1cs_auxiliary_input<FieldT> ( full_assignment.begin() + cs.num_inputs(), full_assignment.end() );


        } else if ( evaluate_with == Reader ){

        
            profile.enter_block("reader_2->evaluate_inputs()" );
            embedded_generator_reader->evaluateInputs( profile );
            profile.leave_block("reader_2->evaluate_inputs()" );


            // extract primary and auxiliary input
            
            const r1cs_variable_assignment<FieldT> full_assignment =
                get_variable_assignment_from_gadgetlib2_2(* pb , getLastVariableIndex() );
            
            cs.primary_input_size = embedded_generator_reader->getNumInputs() + embedded_generator_reader->getNumOutputs();
            cs.auxiliary_input_size = full_assignment.size() - cs.num_inputs();
            
            primary_input = r1cs_primary_input<FieldT> ( full_assignment.begin(), full_assignment.begin() + cs.num_inputs() );
            auxiliary_input = r1cs_auxiliary_input<FieldT> ( full_assignment.begin() + cs.num_inputs(), full_assignment.end() );

            pb->clear_value_mapping();    

            LOGD("Evaluate Inputs Done : Full Assignments:%zu ,  Primary Inputs:%zu , Auxiliary Inputs:%zu \n", 
                 full_assignment.size() , cs.primary_input_size, cs.auxiliary_input_size ) ;

        }
        
        profile.leave_block("Generate Auxiliary Inputs" );
        
        inputs_evaluated = true ;

        return 0;
    }
   
}